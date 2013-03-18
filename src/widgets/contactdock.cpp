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
#include <QtGui/QVBoxLayout>
#include <QtGui/QListWidget>
#include <QtGui/QSpacerItem>
#include <QtGui/QHeaderView>
#include <QtGui/QCheckBox>
#include <QtGui/QSplitter>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QClipboard>

//KDE
#include <KDebug>
#include <KLineEdit>
#include <KLocalizedString>
#include <KIcon>
#include <KComboBox>
#include <KInputDialog>
#include <KAction>

//SFLPhone
#include "contactitemwidget.h"
#include "sflphone.h"
#include "callview.h"
#include "sflphoneview.h"
#include "bookmarkdock.h"

//SFLPhone library
#include "lib/historymodel.h"
#include "lib/call.h"
#include "lib/contact.h"
#include "klib/helperfunctions.h"
#include "klib/akonadibackend.h"
#include "klib/configurationskeleton.h"

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
   m_pFilterLE     = new KLineEdit   (                   );
   m_pSplitter     = new QSplitter   ( Qt::Vertical,this );
   m_pSortByCBB    = new KComboBox   ( this              );
   m_pCallView     = new QListWidget ( this              );
   m_pShowHistoCK  = new QCheckBox   ( this              );

   QStringList sortType;
   sortType << i18nc("Sort by Name","Name") << i18nc("Sort by Organisation","Organisation") << i18nc("Sort by Recently used","Recently used") << i18nc("Sort by Group","Group") << i18nc("Sort by Department","Department");

   m_pSortByCBB->addItems(sortType);

   QWidget* mainWidget = new QWidget(this);
   setWidget(mainWidget);
   KeyPressEaterC *keyPressEater = new KeyPressEaterC( this               );

   m_pFilterLE->setPlaceholderText(i18n("Filter"));
   m_pFilterLE->setClearButtonShown(true);

   m_pShowHistoCK->setChecked(ConfigurationSkeleton::displayContactCallHistory());
   m_pShowHistoCK->setText(i18n("Display history"));

   setHistoryVisible(ConfigurationSkeleton::displayContactCallHistory());

   QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
   
   m_pView = new CategorizedTreeView(this);
   m_pSourceModel = new ContactByNameProxyModel(AkonadiBackend::getInstance(),Qt::DisplayRole,false);
   m_pProxyModel = new ContactSortFilterProxyModel(this);
   m_pProxyModel->setSourceModel(m_pSourceModel);
   m_pProxyModel->setSortRole(Qt::DisplayRole);
   m_pProxyModel->setSortLocaleAware(true);
   m_pProxyModel->setFilterRole(ContactBackend::Role::Filter);
   m_pProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
   m_pProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
   m_pView->setModel(m_pProxyModel);
   m_pView->installEventFilter(keyPressEater);
   mainLayout->addWidget(m_pView);
   m_pView->setSortingEnabled(true);
   m_pView->sortByColumn(0,Qt::AscendingOrder);
   connect(m_pView,SIGNAL(contextMenuRequest(QModelIndex))     , this , SLOT(slotContextMenu(QModelIndex)));
   connect(m_pProxyModel ,SIGNAL(layoutChanged()), this , SLOT(expandTree())                );
   connect(m_pFilterLE ,SIGNAL(textChanged(QString)), m_pProxyModel , SLOT(setFilterRegExp(QString)));
   connect(m_pFilterLE ,SIGNAL(textChanged(QString)), this , SLOT(expandTree()));

   mainLayout->addWidget  ( m_pSortByCBB   );
   mainLayout->addWidget  ( m_pShowHistoCK );
   mainLayout->addWidget  ( m_pSplitter    );
   m_pSplitter->addWidget ( m_pView        );
   m_pSplitter->addWidget ( m_pCallView    );
   mainLayout->addWidget  ( m_pFilterLE    );

   m_pSplitter->setChildrenCollapsible(true);
   m_pSplitter->setStretchFactor(0,7);

   QTimer* timer = new QTimer(this);

   m_pSortByCBB->setCurrentIndex(ConfigurationSkeleton::contactSortMode());

   connect(AkonadiBackend::getInstance() ,SIGNAL(collectionChanged()),                                  this, SLOT(reloadContact())                     );
   connect(m_pSortByCBB                  ,SIGNAL(currentIndexChanged(int)),                             this, SLOT(setCategory(int))                    );
   connect(m_pFilterLE,                   SIGNAL(textChanged(QString)),                                 this, SLOT(filter(QString))                     );
   connect(m_pShowHistoCK,                SIGNAL(toggled(bool)),                                        this, SLOT(setHistoryVisible(bool))             );
   connect(timer                         ,SIGNAL(timeout()),                                            this, SLOT(reloadHistoryConst())                );
   connect(ConfigurationSkeleton::self() ,SIGNAL(configChanged()),                                      this, SLOT(reloadContact())                     );
   timer->start(1800*1000); //30 minutes
   setWindowTitle(i18n("Contact"));
} //ContactDock

///Destructor
ContactDock::~ContactDock()
{
   /*foreach (ContactItemWidget* w, m_Contacts) {
      delete w;
   }
   
   delete m_pFilterLE   ;
   delete m_pSplitter   ;
   delete m_pContactView;
   delete m_pCallView   ;
   delete m_pSortByCBB  ;
   delete m_pShowHistoCK;*/
   
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
   if (m_pCurrentContact && m_pCurrentContact->getPhoneNumbers().size() > 1 && m_PreselectedNb.isEmpty()) {
      QStringList list;
      QHash<QString,QString> map;
      foreach (Contact::PhoneNumber* number, m_pCurrentContact->getPhoneNumbers()) {
         map[number->getType()+" ("+number->getNumber()+')'] = number->getNumber();
         list << number->getType()+" ("+number->getNumber()+')';
      }
      QString result = KInputDialog::getItem ( i18n("Select phone number"), i18n("This contact has many phone numbers, please select the one you wish to call"), list, 0, false, &ok,this);

      if (!ok) {
         kDebug() << "Operation cancelled";
      }
      return map[result];
   }
   else if (!m_PreselectedNb.isEmpty()) {
      ok = true;
      return m_PreselectedNb;
   }
   else if (m_pCurrentContact->getPhoneNumbers().size() == 1) {
      ok = true;
      return m_pCurrentContact->getPhoneNumbers()[0]->getNumber();
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
   qDebug() << "HERE" << index.parent().isValid() << index.parent().parent().isValid();
   showContext(index);
}


///Show the context menu
void ContactDock::showContext(const QModelIndex& index)
{
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
      Contact* ct = (Contact*)((ContactTreeBackend*)(static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index).internalPointer())->getSelf();
      m_pCurrentContact = ct;
      m_PreselectedNb.clear();
      if (!ct->getPreferredEmail().isEmpty()) {
         m_pEmail->setEnabled(true);
      }
      Contact::PhoneNumbers numbers = ct->getPhoneNumbers();
      m_pBookmark->setEnabled(numbers.count() == 1);
   }
   else if (index.parent().parent().isValid()) {
      m_pCurrentContact = (Contact*)((ContactTreeBackend*)(static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index).internalPointer())->getSelf();
      m_PreselectedNb   = m_pCurrentContact->getPhoneNumbers()[index.row()]->getNumber();
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
qDebug() << "HERE";
   m_pMenu->exec(QCursor::pos());
} //showContext

///Send an email
void ContactDock::sendEmail()
{
   kDebug() << "Sending email";
   QProcess *myProcess = new QProcess(this);
   QStringList arguments;
   myProcess->start("xdg-email", (arguments << m_pCurrentContact->getPreferredEmail()));
}

///Call the same number again
void ContactDock::callAgain()
{
   kDebug() << "Calling ";
   bool ok;
   QString number = showNumberSelector(ok);
   if (ok) {
      Call* call = SFLPhone::model()->addDialingCall(m_pCurrentContact->getFormattedName(), AccountList::getCurrentAccount());
      if (call) {
         call->setCallNumber(number);
         call->setPeerName(m_pCurrentContact->getFormattedName());
         call->actionPerformed(CALL_ACTION_ACCEPT);
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
   mimeData->setData(MIME_CONTACT, m_pCurrentContact->getUid().toUtf8());
   QString numbers(m_pCurrentContact->getFormattedName()+": ");
   QString numbersHtml("<b>"+m_pCurrentContact->getFormattedName()+"</b><br />");
   foreach (Contact::PhoneNumber* number, m_pCurrentContact->getPhoneNumbers()) {
      numbers     += number->getNumber()+" ("+number->getType()+")  ";
      numbersHtml += number->getNumber()+" ("+number->getType()+")  <br />";
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
   AkonadiBackend::getInstance()->editContact(m_pCurrentContact);
}

///Add a new phone number for this contact
//TODO
void ContactDock::addPhone()
{
   kDebug() << "Adding to contact";
   bool ok;
   QString text = KInputDialog::getText( i18n("Enter a new number"), i18n("New number:"), QString(), &ok,this);
   if (ok && !text.isEmpty()) {
      AkonadiBackend::getInstance()->addPhoneNumber(m_pCurrentContact,text,"work");
   }
}

///Add this contact to the bookmark list
void ContactDock::bookmark()
{
   Contact::PhoneNumbers numbers = m_pCurrentContact->getPhoneNumbers();
   if (numbers.count() == 1)
      SFLPhone::app()->bookmarkDock()->addBookmark(numbers[0]->getNumber());
}

///Called when a call is dropped on transfer
void ContactDock::transferEvent(QMimeData* data)
{
   if (data->hasFormat( MIME_CALLID)) {
      bool ok;
      QString result = showNumberSelector(ok);
      if (ok) {
         Call* call = SFLPhone::model()->getCall(data->data(MIME_CALLID));
         if (dynamic_cast<Call*>(call)) {
            call->changeCurrentState(CALL_STATE_TRANSFERRED);
            SFLPhone::model()->transfer(call, result);
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
   m_pCallView->setVisible(visible);
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
         m_pProxyModel->setSortRole(ContactBackend::Role::Organization);
         m_pSourceModel->setRole(ContactBackend::Role::Organization);
         m_pSourceModel->setShowAll(true);
         break;
      case SortingCategory::RecentlyUsed:
         m_pSourceModel->setRole(ContactBackend::Role::FormattedLastUsed);
         m_pSourceModel->setShowAll(true);
         m_pProxyModel->setSortRole(ContactBackend::Role::IndexedLastUsed);
         break;
      case SortingCategory::Group:
         m_pSourceModel->setRole(ContactBackend::Role::Group);
         m_pSourceModel->setShowAll(true);
         m_pProxyModel->setSortRole(ContactBackend::Role::Group);
         break;
      case SortingCategory::Department:
         m_pSourceModel->setRole(ContactBackend::Role::Department);
         m_pSourceModel->setShowAll(true);
         m_pProxyModel->setSortRole(ContactBackend::Role::Department);
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
