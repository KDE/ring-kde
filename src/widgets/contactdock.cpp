/***************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Valle <emmanuel.lepage@savoirfairelinux.com >*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/

//Parent
#include "contactdock.h"

//Qt
#include <QtCore/QDateTime>
#include <QtCore/QMap>
#include <QtCore/QTimer>
#include <QtCore/QProcess>
#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QClipboard>
#include <QtGui/QListWidget>
#include <QtGui/QKeyEvent>

//KDE
#include <KDebug>
#include <KLocalizedString>
#include <KIcon>
#include <KInputDialog>
#include <KAction>

//SFLPhone
#include "sflphone.h"
#include "sflphoneview.h"
#include "bookmarkdock.h"
#include "kphonenumberselector.h"

//SFLPhone library
#include "categorizedtreeview.h"
#include "lib/historymodel.h"
#include "lib/call.h"
#include "lib/contact.h"
#include "lib/accountlist.h"
#include "klib/helperfunctions.h"
#include "klib/akonadibackend.h"
#include "klib/kcfg_settings.h"
#include "../lib/contactproxymodel.h"
#include "../delegates/categorizeddelegate.h"
#include "../delegates/contactdelegate.h"
#include "../delegates/phonenumberdelegate.h"

#define CURRENT_SORTING_MODE m_pSortByCBB->currentIndex()

///Forward keypresses to the filter line edit
bool KeyPressEaterC::eventFilter(QObject *obj, QEvent *event)
{
   if (event->type() == QEvent::KeyPress) {
      m_pDock->keyPressEvent((QKeyEvent*)event);
      return true;
   } else {
      // standard event processing
      return QObject::eventFilter(obj, event);
   }
} //eventFilter



///Constructor
ContactDock::ContactDock(QWidget* parent) : QDockWidget(parent),m_pCallAgain(nullptr),m_pMenu(nullptr)
{
   setObjectName("contactDock");
   QWidget* mainWidget = new QWidget(this);
   setupUi(mainWidget);

   m_pCallView     = new QListWidget ( this              );
   m_pShowHistoCK  = new QCheckBox   ( this              );

   QStringList sortType;
   sortType << i18nc("Sort by Name","Name") << i18nc("Sort by Organisation","Organisation") << i18nc("Sort by Recently used","Recently used") << i18nc("Sort by Group","Group") << i18nc("Sort by Department","Department");

   m_pSortByCBB->addItems(sortType);

   setWidget(mainWidget);
   KeyPressEaterC *keyPressEater = new KeyPressEaterC( this               );

   m_pFilterLE->setPlaceholderText(i18n("Filter"));
   m_pFilterLE->setClearButtonShown(true);

   m_pShowHistoCK->setChecked(ConfigurationSkeleton::displayContactCallHistory());
   m_pShowHistoCK->setText(i18n("Display history"));
   m_pTopWidget->layout()->addWidget(m_pShowHistoCK);

   setHistoryVisible(ConfigurationSkeleton::displayContactCallHistory());

   SortedTreeDelegate* delegate = new SortedTreeDelegate(m_pView);
   delegate->setChildDelegate(new ContactDelegate());
   delegate->setChildChildDelegate(new PhoneNumberDelegate());
   m_pView->setDelegate(delegate);

   m_pSourceModel = new ContactProxyModel(AkonadiBackend::instance(),Qt::DisplayRole,false);
   m_pProxyModel = new ContactSortFilterProxyModel(this);
   m_pProxyModel->setSourceModel(m_pSourceModel);
   m_pProxyModel->setSortRole(Qt::DisplayRole);
   m_pProxyModel->setSortLocaleAware(true);
   m_pProxyModel->setFilterRole(AbstractContactBackend::Role::Filter);
   m_pProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
   m_pProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
   m_pView->setModel(m_pProxyModel);
   m_pView->installEventFilter(keyPressEater);
   m_pView->setSortingEnabled(true);
   m_pView->sortByColumn(0,Qt::AscendingOrder);
   connect(m_pView,SIGNAL(contextMenuRequest(QModelIndex))     , this , SLOT(slotContextMenu(QModelIndex)));
   connect(m_pProxyModel ,SIGNAL(layoutChanged()), this , SLOT(expandTree())                );
   connect(m_pFilterLE ,SIGNAL(filterStringChanged(QString)), m_pProxyModel , SLOT(setFilterRegExp(QString)));
   connect(m_pFilterLE ,SIGNAL(textChanged(QString)), this , SLOT(expandTree()));
   m_pBottomWidget->layout()->addWidget ( m_pCallView    );

   splitter->setStretchFactor(0,7);

//    QTimer* timer = new QTimer(this);

   m_pSortByCBB->setCurrentIndex(ConfigurationSkeleton::contactSortMode());

//    connect(AkonadiBackend::instance() ,SIGNAL(collectionChanged()),                                  this, SLOT(reloadContact())                     );
   connect(m_pSortByCBB                  ,SIGNAL(currentIndexChanged(int)),                             this, SLOT(setCategory(int))                    );
//    connect(m_pFilterLE,                   SIGNAL(textChanged(QString)),                                 this, SLOT(filter(QString))                     );
   connect(m_pShowHistoCK,                SIGNAL(toggled(bool)),                                        this, SLOT(setHistoryVisible(bool))             );
//    connect(timer                         ,SIGNAL(timeout()),                                            this, SLOT(reloadHistoryConst())                );
//    connect(ConfigurationSkeleton::self() ,SIGNAL(configChanged()),                                      this, SLOT(reloadContact())                     );
   connect(m_pView                       ,SIGNAL(doubleClicked(QModelIndex)),                           this, SLOT(slotDoubleClick(QModelIndex))        );
//    timer->start(1800*1000); //30 minutes
   setWindowTitle(i18n("Contact"));
} //ContactDock

///Destructor
ContactDock::~ContactDock()
{
   delete m_pCallView   ;
   delete m_pShowHistoCK;
   
   if (m_pMenu) {
      delete m_pMenu        ;
      delete m_pCallAgain   ;
      delete m_pEditContact ;
      delete m_pCopy        ;
      delete m_pEmail       ;
      delete m_pAddPhone    ;
      delete m_pBookmark    ;
   }
}


/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/

///Select a number
QString ContactDock::showNumberSelector(bool& ok)
{
   if (m_pCurrentContact && m_pCurrentContact->phoneNumbers().size() > 1 && m_PreselectedNb.isEmpty()) {
      const Contact::PhoneNumber number = KPhoneNumberSelector().getNumber(m_pCurrentContact->uid());
      if (number.number().isEmpty()) {
         kDebug() << "Operation cancelled";
      }
      return number.number();
   }
   else if (!m_PreselectedNb.isEmpty()) {
      ok = true;
      return m_PreselectedNb;
   }
   else if (m_pCurrentContact&& m_pCurrentContact->phoneNumbers().size() == 1) {
      ok = true;
      return m_pCurrentContact->phoneNumbers()[0]->number();
   }
   else {
      ok = false;
      return "";
   }
}

///Query the call history for all items related to this contact
// void ContactDock::loadContactHistory(QTreeWidgetItem* item)
// {
//    if (m_pShowHistoCK->isChecked()) {
//       m_pCallView->clear();
//       if (dynamic_cast<QNumericTreeWidgetItem_hist*>(item) != nullptr) {
//          QNumericTreeWidgetItem_hist* realItem = dynamic_cast<QNumericTreeWidgetItem_hist*>(item);
//          foreach (Call* call, HistoryModel::getHistory()) {
//             if (realItem->widget != 0) {
//                foreach (Contact::PhoneNumber* number, realItem->widget->getContact()->getPhoneNumbers()) {
//                   if (number->getNumber() == call->getPeerPhoneNumber()) {
//                      m_pCallView->addItem(QDateTime::fromTime_t(call->getStartTimeStamp().toUInt()).toString());
//                   }
//                }
//             }
//          }
//       }
//    }
// } //loadContactHistory

// void ContactDock::reloadHistoryConst()
// {
//    switch (CURRENT_SORTING_MODE) {
//       case Recently_used:
//          reloadContact();
//          break;
//    }
// }

///Called when someone right click on the 'index'
void ContactDock::slotContextMenu(QModelIndex index)
{
   showContext(index);
}

void ContactDock::slotDoubleClick(const QModelIndex& index)
{
   QModelIndex idx = (static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index);
   if (!idx.isValid() || !idx.parent().isValid())
      return;
   if (((ContactTreeBackend*)idx.internalPointer())->type() != ContactTreeBackend::Type::CONTACT)
      return;
   m_pCurrentContact = (Contact*)static_cast<ContactTreeBackend*>(idx.internalPointer())->self();
   callAgain();
}


///Show the context menu
void ContactDock::showContext(const QModelIndex& index)
{
   if (!index.parent().isValid())
      return;
   if (!m_pCallAgain) {
      m_pCallAgain   = new KAction(this);
      m_pCallAgain->setShortcut   ( Qt::CTRL + Qt::Key_Enter   );
      m_pCallAgain->setText       ( i18n("Call Again")         );
      m_pCallAgain->setIcon       ( KIcon("call-start")        );

      m_pEditContact = new KAction(this);
      m_pEditContact->setShortcut ( Qt::CTRL + Qt::Key_E       );
      m_pEditContact->setText     ( i18n("Edit contact")       );
      m_pEditContact->setIcon     ( KIcon("contact-new")       );

      m_pCopy        = new KAction(this);
      m_pCopy->setShortcut        ( Qt::CTRL + Qt::Key_C       );
      m_pCopy->setText            ( i18n("Copy")               );
      m_pCopy->setIcon            ( KIcon("edit-copy")         );

      m_pEmail       = new KAction(this);
      m_pEmail->setShortcut       ( Qt::CTRL + Qt::Key_M       );
      m_pEmail->setText           ( i18n("Send Email")         );
      m_pEmail->setIcon           ( KIcon("mail-message-new")  );
      m_pEmail->setEnabled        ( false                      );

      m_pAddPhone    = new KAction(this);
      m_pAddPhone->setShortcut    ( Qt::CTRL + Qt::Key_N       );
      m_pAddPhone->setText        ( i18n("Add Phone Number")   );
      m_pAddPhone->setIcon        ( KIcon("list-resource-add") );
      m_pEmail->setEnabled        ( false                      );

      m_pBookmark    = new KAction(this);
      m_pBookmark->setShortcut    ( Qt::CTRL + Qt::Key_D       );
      m_pBookmark->setText        ( i18n("Bookmark")           );
      m_pBookmark->setIcon        ( KIcon("bookmarks")         );

      connect(m_pCallAgain    , SIGNAL(triggered()) , this,SLOT(callAgain())  );
      connect(m_pEditContact  , SIGNAL(triggered()) , this,SLOT(editContact()));
      connect(m_pCopy         , SIGNAL(triggered()) , this,SLOT(copy())       );
      connect(m_pEmail        , SIGNAL(triggered()) , this,SLOT(sendEmail())  );
      connect(m_pAddPhone     , SIGNAL(triggered()) , this,SLOT(addPhone())   );
      connect(m_pBookmark     , SIGNAL(triggered()) , this,SLOT(bookmark())   );
   }
   if (index.parent().isValid()  && !index.parent().parent().isValid()) {
      Contact* ct = (Contact*)((ContactTreeBackend*)(static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index).internalPointer())->self();
      m_pCurrentContact = ct;
      m_PreselectedNb.clear();
      if (!ct->preferredEmail().isEmpty()) {
         m_pEmail->setEnabled(true);
      }
      Contact::PhoneNumbers numbers = ct->phoneNumbers();
      m_pBookmark->setEnabled(numbers.count() == 1);
   }
   else if (index.parent().parent().isValid()) {
      m_pCurrentContact = (Contact*)((ContactTreeBackend*)(static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index).internalPointer())->self();
      m_PreselectedNb   = m_pCurrentContact->phoneNumbers()[index.row()]->number();
   }
   else {
      m_pCurrentContact = nullptr;
      m_PreselectedNb.clear();
   }
   if (!m_pMenu) {
      m_pMenu = new QMenu( this          );
      m_pMenu->addAction( m_pCallAgain   );
      m_pMenu->addAction( m_pEditContact );
      m_pMenu->addAction( m_pAddPhone    );
      m_pMenu->addAction( m_pCopy        );
      m_pMenu->addAction( m_pEmail       );
      m_pMenu->addAction( m_pBookmark    );
   }
   m_pMenu->exec(QCursor::pos());
} //showContext

///Send an email
void ContactDock::sendEmail()
{
   kDebug() << "Sending email";
   QProcess *myProcess = new QProcess(this);
   QStringList arguments;
   myProcess->start("xdg-email", (arguments << m_pCurrentContact->preferredEmail()));
}

///Call the same number again
void ContactDock::callAgain()
{
   kDebug() << "Calling ";
   bool ok;
   QString number = showNumberSelector(ok);
   if (ok) {
      Call* call = CallModel::instance()->addDialingCall(m_pCurrentContact->formattedName(), AccountList::currentAccount());
      if (call) {
         call->setCallNumber(number);
         call->setPeerName(m_pCurrentContact->formattedName());
         call->actionPerformed(Call::Action::ACCEPT);
      }
      else {
         HelperFunctions::displayNoAccountMessageBox(this);
      }
   }
}

///Copy contact to clipboard
void ContactDock::copy()
{
   kDebug() << "Copying contact";
   QMimeData* mimeData = new QMimeData();
   mimeData->setData(MIME_CONTACT, m_pCurrentContact->uid().toUtf8());
   QString numbers(m_pCurrentContact->formattedName()+": ");
   QString numbersHtml("<b>"+m_pCurrentContact->formattedName()+"</b><br />");
   foreach (Contact::PhoneNumber* number, m_pCurrentContact->phoneNumbers()) {
      numbers     += number->number()+" ("+number->type()+")  ";
      numbersHtml += number->number()+" ("+number->type()+")  <br />";
   }
   mimeData->setData("text/plain", numbers.toUtf8());
   mimeData->setData("text/html", numbersHtml.toUtf8());
   QClipboard* clipboard = QApplication::clipboard();
   clipboard->setMimeData(mimeData);
}

///Edit this contact
void ContactDock::editContact()
{
   kDebug() << "Edit contact";
   AkonadiBackend::instance()->editContact(m_pCurrentContact);
}

///Add a new phone number for this contact
//TODO
void ContactDock::addPhone()
{
   kDebug() << "Adding to contact";
   bool ok;
   const QString text = KInputDialog::getText( i18n("Enter a new number"), i18n("New number:"), QString(), &ok,this);
   if (ok && !text.isEmpty()) {
      AkonadiBackend::instance()->addPhoneNumber(m_pCurrentContact,text,"work");
   }
}

///Add this contact to the bookmark list
void ContactDock::bookmark()
{
   const Contact::PhoneNumbers numbers = m_pCurrentContact->phoneNumbers();
   if (numbers.count() == 1)
      SFLPhone::app()->bookmarkDock()->addBookmark(numbers[0]->number());
}

///Called when a call is dropped on transfer
void ContactDock::transferEvent(QMimeData* data)
{
   if (data->hasFormat( MIME_CALLID)) {
      bool ok;
      const QString result = showNumberSelector(ok);
      if (ok) {
         Call* call = CallModel::instance()->getCall(data->data(MIME_CALLID));
         if (dynamic_cast<Call*>(call)) {
            call->changeCurrentState(Call::State::TRANSFERRED);
            CallModel::instance()->transfer(call, result);
         }
      }
   }
   else
      kDebug() << "Invalid mime data";
//    m_pBtnTrans->setHoverState(false);
//    m_pBtnTrans->setVisible(false);
}

/*****************************************************************************
 *                                                                           *
 *                                Drag and Drop                              *
 *                                                                           *
 ****************************************************************************/

///Serialize information to be used for drag and drop
// QMimeData* ContactTree::mimeData( const QList<QTreeWidgetItem *> items) const
// {
//    kDebug() << "An history call is being dragged";
//    if (items.size() < 1) {
//       return nullptr;
//    }
// 
//    QMimeData *mimeData = new QMimeData();
// 
//    //Contact
//    if (dynamic_cast<QNumericTreeWidgetItem_hist*>(items[0])) {
//       QNumericTreeWidgetItem_hist* item = dynamic_cast<QNumericTreeWidgetItem_hist*>(items[0]);
//       if (item->widget != 0) {
//          mimeData->setData(MIME_CONTACT, item->widget->getContact()->getUid().toUtf8());
//       }
//       else if (!item->number.isEmpty()) {
//          mimeData->setData(MIME_PHONENUMBER, item->number.toUtf8());
//       }
//    }
//    else {
//       kDebug() << "the item is not a call";
//    }
//    return mimeData;
// } //mimeData

///Handle data being dropped on the widget
// bool ContactTree::dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action)
// {
//    Q_UNUSED(index )
//    Q_UNUSED(action)
//    Q_UNUSED(parent)
// 
//    QByteArray encodedData = data->data(MIME_CALLID);
// 
//    kDebug() << "In history import"<< QString(encodedData);
// 
//    return false;
// }


/*****************************************************************************
 *                                                                           *
 *                                  Setters                                  *
 *                                                                           *
 ****************************************************************************/

///Show or hide the history list
void ContactDock::setHistoryVisible(bool visible)
{
   kDebug() << "Toggling history visibility";
   m_pBottomWidget->setVisible(visible);
   ConfigurationSkeleton::setDisplayContactCallHistory(visible);
}

///Set sorting category
void ContactDock::setCategory(int index)
{
   switch(index) {
      case SortingCategory::Name:
         m_pSourceModel->setRole(Qt::DisplayRole);
         m_pSourceModel->setShowAll(false);
         m_pProxyModel->setSortRole(Qt::DisplayRole);
         break;
      case SortingCategory::Organization:
         m_pProxyModel->setSortRole(AbstractContactBackend::Role::Organization);
         m_pSourceModel->setRole(AbstractContactBackend::Role::Organization);
         m_pSourceModel->setShowAll(true);
         break;
      case SortingCategory::RecentlyUsed:
         m_pSourceModel->setRole(AbstractContactBackend::Role::FormattedLastUsed);
         m_pSourceModel->setShowAll(true);
         m_pProxyModel->setSortRole(AbstractContactBackend::Role::IndexedLastUsed);
         break;
      case SortingCategory::Group:
         m_pSourceModel->setRole(AbstractContactBackend::Role::Group);
         m_pSourceModel->setShowAll(true);
         m_pProxyModel->setSortRole(AbstractContactBackend::Role::Group);
         break;
      case SortingCategory::Department:
         m_pSourceModel->setRole(AbstractContactBackend::Role::Department);
         m_pSourceModel->setShowAll(true);
         m_pProxyModel->setSortRole(AbstractContactBackend::Role::Department);
         break;
   };
}


/*****************************************************************************
 *                                                                           *
 *                             Keyboard handling                             *
 *                                                                           *
 ****************************************************************************/

///Handle keypresses ont the dock
void ContactDock::keyPressEvent(QKeyEvent* event) {
   int key = event->key();
   if(key == Qt::Key_Escape)
      m_pFilterLE->setText(QString());
   else if(key == Qt::Key_Return || key == Qt::Key_Enter) {
//       if (m_pContactView->selectedItems().size() && m_pContactView->selectedItems()[0] && m_pContactView->itemWidget(m_pContactView->selectedItems()[0],0)) {
//          QNumericTreeWidgetItem_hist* item = dynamic_cast<QNumericTreeWidgetItem_hist*>(m_pContactView->selectedItems()[0]);
//          if (item) {
//             Call* call = nullptr;
//             SFLPhone::app()->view()->selectCallPhoneNumber(&call,item->widget->getContact());
//          }
//       }
   }
   else if((key == Qt::Key_Backspace) && (m_pFilterLE->text().size()))
      m_pFilterLE->setText(m_pFilterLE->text().left( m_pFilterLE->text().size()-1 ));
   else if (!event->text().isEmpty() && !(key == Qt::Key_Backspace))
      m_pFilterLE->setText(m_pFilterLE->text()+event->text());
} //keyPressEvent

/*****************************************************************************
 *                                                                           *
 *                                  Helpers                                  *
 *                                                                           *
 ****************************************************************************/

///Expand the tree according to the user preferences
void ContactDock::expandTree()
{
   m_pView->expandToDepth( 0 + (ConfigurationSkeleton::alwaysShowPhoneNumber()==true));
}
