/***************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                         *
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
#include <KLocale>
#include <KMessageBox>

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
#include "lib/contactmodel.h"
#include "lib/numbercategory.h"
#include "lib/phonedirectorymodel.h"
#include "lib/phonenumber.h"
#include "lib/accountlistmodel.h"
#include "klib/helperfunctions.h"
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
      QKeyEvent* e = (QKeyEvent*)event;
      if (e->key() != Qt::Key_Up && e->key() != Qt::Key_Down) {
         m_pDock->keyPressEvent(e);
         return true;
      }
   }
   // standard event processing
   return QObject::eventFilter(obj, event);
}

bool ContactSortFilterProxyModel::filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const
{
   const bool status = sourceModel()->index(source_row,0,source_parent).data(ContactModel::Role::Active).toBool();
   if (!status)
      return false;
   else if (!source_parent.isValid() || source_parent.parent().isValid())
      return true;

   return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

///Constructor
ContactDock::ContactDock(QWidget* parent) : QDockWidget(parent),m_pCallAgain(nullptr),m_pMenu(nullptr)
{
   setObjectName("contactDock");
   QWidget* mainWidget = new QWidget(this);
   setupUi(mainWidget);

   QStringList sortType;
   sortType << i18nc("Sort by Name","Name") << i18nc("Sort by Organisation","Organisation") << i18nc("Sort by Recently used","Recently used") << i18nc("Sort by Group","Group") << i18nc("Sort by Department","Department");

   m_pSortByCBB->addItems(sortType);

   setWidget(mainWidget);
   m_pKeyPressEater = new KeyPressEaterC( this               );

   m_pFilterLE->setPlaceholderText(i18n("Filter"));
   m_pFilterLE->setClearButtonShown(true);

   setHistoryVisible(ConfigurationSkeleton::displayContactCallHistory());

   m_pCategoryDelegate = new CategorizedDelegate(m_pView);
   m_pPhoneNumberDelegate = new PhoneNumberDelegate();
   m_pContactDelegate = new ContactDelegate(m_pView);
   m_pPhoneNumberDelegate->setView(m_pView);
   m_pContactDelegate->setChildDelegate(m_pPhoneNumberDelegate);
   m_pCategoryDelegate->setChildDelegate(m_pContactDelegate);
   m_pCategoryDelegate->setChildChildDelegate(m_pPhoneNumberDelegate);
   m_pView->setDelegate(m_pCategoryDelegate);
   m_pView->setViewType(CategorizedTreeView::ViewType::Contact);

   m_pSourceModel = new ContactProxyModel(Qt::DisplayRole,false);
   m_pProxyModel = new ContactSortFilterProxyModel(this);
   m_pProxyModel->setSourceModel(m_pSourceModel);
   m_pProxyModel->setSortRole(Qt::DisplayRole);
   m_pProxyModel->setSortLocaleAware(true);
   m_pProxyModel->setFilterRole(ContactModel::Role::Filter);
   m_pProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
   m_pProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
   m_pView->setModel(m_pProxyModel);
   m_pView->installEventFilter(m_pKeyPressEater);
   m_pView->setSortingEnabled(true);
   m_pView->sortByColumn(0,Qt::AscendingOrder);
   connect(m_pView,SIGNAL(contextMenuRequest(QModelIndex))     , this , SLOT(slotContextMenu(QModelIndex)));
   connect(m_pProxyModel ,SIGNAL(layoutChanged()), this , SLOT(expandTree()));
   connect(m_pFilterLE ,SIGNAL(filterStringChanged(QString)), m_pProxyModel , SLOT(setFilterRegExp(QString)));
   connect(m_pFilterLE ,SIGNAL(textChanged(QString)), this , SLOT(expandTree()));

   splitter->setStretchFactor(0,7);

   m_pSortByCBB->setCurrentIndex(ConfigurationSkeleton::contactSortMode());

   connect(m_pSortByCBB                  ,SIGNAL(currentIndexChanged(int)),                             this, SLOT(setCategory(int))                    );
//    connect(ConfigurationSkeleton::self() ,SIGNAL(configChanged()),                                      this, SLOT(reloadContact())                     );
   connect(m_pView                       ,SIGNAL(doubleClicked(QModelIndex)),                           this, SLOT(slotDoubleClick(QModelIndex))        );
   setWindowTitle(i18nc("Contact tab","Contact"));
} //ContactDock

///Destructor
ContactDock::~ContactDock()
{
   if (m_pMenu) {
      delete m_pMenu        ;
      delete m_pCallAgain   ;
      delete m_pEditContact ;
      delete m_pCopy        ;
      delete m_pEmail       ;
      delete m_pAddPhone    ;
      delete m_pBookmark    ;
      delete m_pRemove      ;
   }
   //Delegates
   delete m_pCategoryDelegate;

   //Models
   delete m_pProxyModel   ;
   delete m_pSourceModel  ;
   delete m_pKeyPressEater;

}


/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/

///Select a number
PhoneNumber* ContactDock::showNumberSelector(bool& ok)
{
   if (m_pCurrentContact && m_pCurrentContact->phoneNumbers().size() > 1 && m_PreselectedNb.isEmpty()) {
      PhoneNumber* number = KPhoneNumberSelector().getNumber(m_pCurrentContact);
      if (number->uri().isEmpty()) {
         kDebug() << "Operation cancelled";
      }
      return number;
   }
   else if (!m_PreselectedNb.isEmpty()) {
      ok = true;
      return PhoneDirectoryModel::instance()->getNumber( m_PreselectedNb);
   }
   else if (m_pCurrentContact&& m_pCurrentContact->phoneNumbers().size() == 1) {
      ok = true;
      return m_pCurrentContact->phoneNumbers()[0];
   }
   else {
      ok = false;
      return nullptr;
   }
}

///Called when someone right click on the 'index'
void ContactDock::slotContextMenu(QModelIndex index)
{
   showContext(index);
}

void ContactDock::slotDoubleClick(const QModelIndex& index)
{
   const QModelIndex idx = (static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index);
   if (!idx.isValid() || !idx.parent().isValid())
      return;
   const CategorizedCompositeNode* modelItem = static_cast<CategorizedCompositeNode*>(idx.internalPointer());
   if (modelItem->type() == CategorizedCompositeNode::Type::NUMBER) {
      const Contact::PhoneNumbers nbs = *static_cast<const Contact::PhoneNumbers*>(modelItem);
      const PhoneNumber*          nb  = nbs[index.row()];
      m_pCurrentContact = nullptr;
      callAgain(nb);
   }
   else if (modelItem->type() == CategorizedCompositeNode::Type::CONTACT) {
      m_pCurrentContact = static_cast<Contact*>((modelItem)->getSelf());
      callAgain();
   }
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

      m_pRemove      = new KAction(this);
      m_pRemove->setShortcut    ( Qt::CTRL + Qt::Key_Shift + Qt::Key_Delete );
      m_pRemove->setText        ( i18n("Delete")               );
      m_pRemove->setIcon        ( KIcon("edit-delete")         );

      connect(m_pCallAgain    , SIGNAL(triggered()) , this,SLOT(callAgain())  );
      connect(m_pEditContact  , SIGNAL(triggered()) , this,SLOT(editContact()));
      connect(m_pCopy         , SIGNAL(triggered()) , this,SLOT(copy())       );
      connect(m_pEmail        , SIGNAL(triggered()) , this,SLOT(sendEmail())  );
      connect(m_pAddPhone     , SIGNAL(triggered()) , this,SLOT(addPhone())   );
      connect(m_pBookmark     , SIGNAL(triggered()) , this,SLOT(bookmark())   );
      connect(m_pRemove       , SIGNAL(triggered()) , this,SLOT(slotDelete()) );
   }
   if (index.parent().isValid()  && !index.parent().parent().isValid()) {
      Contact* ct = (Contact*)((CategorizedCompositeNode*)(static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index).internalPointer())->getSelf();
      m_pCurrentContact = ct;
      m_PreselectedNb.clear();
      if (!ct->preferredEmail().isEmpty()) {
         m_pEmail->setEnabled(true);
      }
      Contact::PhoneNumbers numbers = ct->phoneNumbers();
      m_pBookmark->setEnabled(numbers.count() == 1);
   }
   else if (index.parent().parent().isValid()) {
      m_pCurrentContact = (Contact*)((CategorizedCompositeNode*)(static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index).internalPointer())->getSelf();
      m_PreselectedNb   = m_pCurrentContact->phoneNumbers()[index.row()]->uri();
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
      m_pMenu->addAction( m_pRemove      );
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
void ContactDock::callAgain(const PhoneNumber* n)
{
   kDebug() << "Calling ";
   bool ok = false;
   const PhoneNumber* number = n?n:showNumberSelector(ok);
   if ( (n || ok) && number) {
      const QString name = n?n->contact()->formattedName() : m_pCurrentContact->formattedName();
      Call* call = CallModel::instance()->dialingCall(name, AccountListModel::currentAccount());
      if (call) {
         call->setDialNumber(number);
         call->setAccount(number->account());
         call->setPeerName(name);
         call->performAction(Call::Action::ACCEPT);
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
   mimeData->setData(MIME_CONTACT, m_pCurrentContact->uid());
   QString numbers(m_pCurrentContact->formattedName()+": ");
   QString numbersHtml("<b>"+m_pCurrentContact->formattedName()+"</b><br />");
   foreach (PhoneNumber* number, m_pCurrentContact->phoneNumbers()) {
      numbers     += number->uri()+" ("+number->category()->name()+")  ";
      numbersHtml += number->uri()+" ("+number->category()->name()+")  <br />";
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
   m_pCurrentContact->edit();
}

///Add a new phone number for this contact
//TODO
void ContactDock::addPhone()
{
   kDebug() << "Adding to contact";
   bool ok = false;
   const QString text = KInputDialog::getText( i18n("Enter a new number"), i18n("New number:"), QString(), &ok,this);
   if (ok && !text.isEmpty()) {
      PhoneNumber* n = PhoneDirectoryModel::instance()->getNumber(text,"work");
      m_pCurrentContact->addPhoneNumber(n);
   }
}

///Add this contact to the bookmark list
void ContactDock::bookmark()
{
   const Contact::PhoneNumbers numbers = m_pCurrentContact->phoneNumbers();
   if (numbers.count() == 1) {
      BookmarkModel::instance()->addBookmark(numbers[0]);
   }
}

void ContactDock::slotDelete()
{
   if (!m_pCurrentContact)
      return;
   const int ret = KMessageBox::questionYesNo(this, i18n("Are you sure you want to permanently delete %1?",
      m_pCurrentContact->formattedName()), i18n("Delete contact"));
   if (ret == KMessageBox::Yes) {
      m_pCurrentContact->remove();
   }
}

///Called when a call is dropped on transfer
void ContactDock::transferEvent(QMimeData* data)
{
   if (data->hasFormat( MIME_CALLID)) {
      bool ok = false;
      const PhoneNumber* result = showNumberSelector(ok);
      if (ok && result) {
         Call* call = CallModel::instance()->getCall(data->data(MIME_CALLID));
         if (dynamic_cast<Call*>(call)) {
//             call->changeCurrentState(Call::State::TRANSFERRED);
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
         m_pProxyModel->setSortRole(ContactModel::Role::Organization);
         m_pSourceModel->setRole(ContactModel::Role::Organization);
         m_pSourceModel->setShowAll(true);
         break;
      case SortingCategory::RecentlyUsed:
         m_pSourceModel->setRole(ContactModel::Role::FormattedLastUsed);
         m_pSourceModel->setShowAll(true);
         m_pProxyModel->setSortRole(ContactModel::Role::IndexedLastUsed);
         break;
      case SortingCategory::Group:
         m_pSourceModel->setRole(ContactModel::Role::Group);
         m_pSourceModel->setShowAll(true);
         m_pProxyModel->setSortRole(ContactModel::Role::Group);
         break;
      case SortingCategory::Department:
         m_pSourceModel->setRole(ContactModel::Role::Department);
         m_pSourceModel->setShowAll(true);
         m_pProxyModel->setSortRole(ContactModel::Role::Department);
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
   m_pView->expandToDepth( 2 );
}
