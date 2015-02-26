/***************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
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
#include <QProcess>
#include <QtCore/QMimeData>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>
#include <QtGui/QClipboard>
#include <QtWidgets/QListWidget>
#include <QtGui/QKeyEvent>
#include <QInputDialog>

//KDE
#include <QDebug>
#include <klocalizedstring.h>
#include <QIcon>
#include <QAction>
#include <klocalizedstring.h>
#include <kmessagebox.h>

//Ring
#include "ring.h"
#include "view.h"
#include "bookmarkdock.h"
#include "kphonenumberselector.h"

//Ring library
#include <bookmarkmodel.h>
#include "categorizedtreeview.h"
#include "historymodel.h"
#include "call.h"
#include "person.h"
#include "mime.h"
#include "personmodel.h"
#include "numbercategory.h"
#include "phonedirectorymodel.h"
#include "contactmethod.h"
#include "accountmodel.h"
#include "availableaccountmodel.h"
#include "klib/helperfunctions.h"
#include "klib/kcfg_settings.h"
#include "contactproxymodel.h"
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

bool PersonSortFilterProxyModel::filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const
{
   const bool status = sourceModel()->index(source_row,0,source_parent).data(PersonModel::Role::Active).toBool();
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
   m_pFilterLE->setClearButtonEnabled(true);

   setHistoryVisible(ConfigurationSkeleton::displayPersonCallHistory());

   m_pCategoryDelegate = new CategorizedDelegate(m_pView);
   m_pContactMethodDelegate = new ContactMethodDelegate();
   m_pContactDelegate = new ContactDelegate(m_pView);
   m_pContactMethodDelegate->setView(m_pView);
   m_pContactDelegate->setChildDelegate(m_pContactMethodDelegate);
   m_pCategoryDelegate->setChildDelegate(m_pContactDelegate);
   m_pCategoryDelegate->setChildChildDelegate(m_pContactMethodDelegate);
   m_pView->setDelegate(m_pCategoryDelegate);
   m_pView->setViewType(CategorizedTreeView::ViewType::Person);

   m_pSourceModel = new ContactProxyModel(Qt::DisplayRole);
   m_pProxyModel = new PersonSortFilterProxyModel(this);
   m_pProxyModel->setSourceModel(m_pSourceModel);
   m_pProxyModel->setSortRole(Qt::DisplayRole);
   m_pProxyModel->setSortLocaleAware(true);
   m_pProxyModel->setFilterRole(PersonModel::Role::Filter);
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
//    connect(ConfigurationSkeleton::self() ,SIGNAL(configChanged()),                                      this, SLOT(reloadPerson())                     );
   connect(m_pView                       ,SIGNAL(doubleClicked(QModelIndex)),                           this, SLOT(slotDoubleClick(QModelIndex))        );
   setWindowTitle(i18nc("Contact tab","Contact"));
} //ContactDock

///Destructor
ContactDock::~ContactDock()
{
   if (m_pMenu) {
      delete m_pMenu        ;
      delete m_pCallAgain   ;
      delete m_pEditPerson ;
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
ContactMethod* ContactDock::showNumberSelector(bool& ok)
{
   if (m_pCurrentPerson && m_pCurrentPerson->phoneNumbers().size() > 1 && m_PreselectedNb.isEmpty()) {
      ContactMethod* number = KPhoneNumberSelector().getNumber(m_pCurrentPerson);
      if (number->uri().isEmpty()) {
         qDebug() << "Operation cancelled";
      }
      return number;
   }
   else if (!m_PreselectedNb.isEmpty()) {
      ok = true;
      return PhoneDirectoryModel::instance()->getNumber( m_PreselectedNb);
   }
   else if (m_pCurrentPerson&& m_pCurrentPerson->phoneNumbers().size() == 1) {
      ok = true;
      return m_pCurrentPerson->phoneNumbers()[0];
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
      const Person::ContactMethods nbs = *static_cast<const Person::ContactMethods*>(modelItem);
      const ContactMethod*          nb  = nbs[index.row()];
      m_pCurrentPerson = nullptr;
      callAgain(nb);
   }
   else if (modelItem->type() == CategorizedCompositeNode::Type::CONTACT) {
      m_pCurrentPerson = static_cast<Person*>((modelItem)->getSelf());
      callAgain();
   }
}


///Show the context menu
void ContactDock::showContext(const QModelIndex& index)
{
   if (!index.parent().isValid())
      return;
   if (!m_pCallAgain) {
      m_pCallAgain   = new QAction(this);
      m_pCallAgain->setShortcut   ( Qt::CTRL + Qt::Key_Enter   );
      m_pCallAgain->setText       ( i18n("Call Again")         );
      m_pCallAgain->setIcon       ( QIcon::fromTheme("call-start")        );

      m_pEditPerson = new QAction(this);
      m_pEditPerson->setShortcut ( Qt::CTRL + Qt::Key_E       );
      m_pEditPerson->setText     ( i18n("Edit contact")       );
      m_pEditPerson->setIcon     ( QIcon::fromTheme("contact-new")       );

      m_pCopy        = new QAction(this);
      m_pCopy->setShortcut        ( Qt::CTRL + Qt::Key_C       );
      m_pCopy->setText            ( i18n("Copy")               );
      m_pCopy->setIcon            ( QIcon::fromTheme("edit-copy")         );

      m_pEmail       = new QAction(this);
      m_pEmail->setShortcut       ( Qt::CTRL + Qt::Key_M       );
      m_pEmail->setText           ( i18n("Send Email")         );
      m_pEmail->setIcon           ( QIcon::fromTheme("mail-message-new")  );
      m_pEmail->setEnabled        ( false                      );

      m_pAddPhone    = new QAction(this);
      m_pAddPhone->setShortcut    ( Qt::CTRL + Qt::Key_N       );
      m_pAddPhone->setText        ( i18n("Add Phone Number")   );
      m_pAddPhone->setIcon        ( QIcon::fromTheme("list-resource-add") );
      m_pEmail->setEnabled        ( false                      );

      m_pBookmark    = new QAction(this);
      m_pBookmark->setShortcut    ( Qt::CTRL + Qt::Key_D       );
      m_pBookmark->setText        ( i18n("Bookmark")           );
      m_pBookmark->setIcon        ( QIcon::fromTheme("bookmarks")         );

      m_pRemove      = new QAction(this);
      m_pRemove->setShortcut    ( Qt::CTRL + Qt::Key_Shift + Qt::Key_Delete );
      m_pRemove->setText        ( i18n("Delete")               );
      m_pRemove->setIcon        ( QIcon::fromTheme("edit-delete")         );

      connect(m_pCallAgain    , SIGNAL(triggered()) , this,SLOT(callAgain())  );
      connect(m_pEditPerson  , SIGNAL(triggered()) , this,SLOT(editPerson()));
      connect(m_pCopy         , SIGNAL(triggered()) , this,SLOT(copy())       );
      connect(m_pEmail        , SIGNAL(triggered()) , this,SLOT(sendEmail())  );
      connect(m_pAddPhone     , SIGNAL(triggered()) , this,SLOT(addPhone())   );
      connect(m_pBookmark     , SIGNAL(triggered()) , this,SLOT(bookmark())   );
      connect(m_pRemove       , SIGNAL(triggered()) , this,SLOT(slotDelete()) );
   }
   if (index.parent().isValid()  && !index.parent().parent().isValid()) {
      Person* ct = (Person*)((CategorizedCompositeNode*)(static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index).internalPointer())->getSelf();
      m_pCurrentPerson = ct;
      m_PreselectedNb.clear();
      if (!ct->preferredEmail().isEmpty()) {
         m_pEmail->setEnabled(true);
      }
      Person::ContactMethods numbers = ct->phoneNumbers();
      m_pBookmark->setEnabled(numbers.count() == 1);
   }
   else if (index.parent().parent().isValid()) {
      m_pCurrentPerson = (Person*)((CategorizedCompositeNode*)(static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index).internalPointer())->getSelf();
      m_PreselectedNb   = m_pCurrentPerson->phoneNumbers()[index.row()]->uri();
   }
   else {
      m_pCurrentPerson = nullptr;
      m_PreselectedNb.clear();
   }
   if (!m_pMenu) {
      m_pMenu = new QMenu( this          );
      m_pMenu->addAction( m_pCallAgain   );
      m_pMenu->addAction( m_pEditPerson );
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
   qDebug() << "Sending email";
   QProcess *myProcess = new QProcess(this);
   QStringList arguments;
   myProcess->start("xdg-email", (arguments << m_pCurrentPerson->preferredEmail()));
}

///Call the same number again
void ContactDock::callAgain(const ContactMethod* n)
{
   qDebug() << "Calling ";
   bool ok = false;
   const ContactMethod* number = n?n:showNumberSelector(ok);
   if ( (n || ok) && number) {
      const QString name = n?n->contact()->formattedName() : m_pCurrentPerson->formattedName();
      Call* call = CallModel::instance()->dialingCall(name, AvailableAccountModel::currentDefaultAccount());
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
   qDebug() << "Copying contact";
   QMimeData* mimeData = new QMimeData();
   mimeData->setData(RingMimes::CONTACT, m_pCurrentPerson->uid());
   QString numbers(m_pCurrentPerson->formattedName()+": ");
   QString numbersHtml("<b>"+m_pCurrentPerson->formattedName()+"</b><br />");
   foreach (ContactMethod* number, m_pCurrentPerson->phoneNumbers()) {
      numbers     += number->uri()+" ("+number->category()->name()+")  ";
      numbersHtml += number->uri()+" ("+number->category()->name()+")  <br />";
   }
   mimeData->setData("text/plain", numbers.toUtf8());
   mimeData->setData("text/html", numbersHtml.toUtf8());
   QClipboard* clipboard = QApplication::clipboard();
   clipboard->setMimeData(mimeData);
}

///Edit this contact
void ContactDock::editPerson()
{
   qDebug() << "Edit contact";
   m_pCurrentPerson->edit();
}

///Add a new phone number for this contact
//TODO
void ContactDock::addPhone()
{
   qDebug() << "Adding to contact";
   bool ok = false;
   const QString text = QInputDialog::getText(this, i18n("Enter a new number"), i18n("New number:"), QLineEdit::Normal, QString(), &ok);
   if (ok && !text.isEmpty()) {
//       ContactMethod* n = PhoneDirectoryModel::instance()->getNumber(text,"work");
//       m_pCurrentPerson->addContactMethod(n); //TODO fixme
   }
}

///Add this contact to the bookmark list
void ContactDock::bookmark()
{
   const Person::ContactMethods numbers = m_pCurrentPerson->phoneNumbers();
   if (numbers.count() == 1) {
      BookmarkModel::instance()->addBookmark(numbers[0]);
   }
}

void ContactDock::slotDelete()
{
   if (!m_pCurrentPerson)
      return;
   const int ret = KMessageBox::questionYesNo(this, i18n("Are you sure you want to permanently delete %1?",
      m_pCurrentPerson->formattedName()), i18n("Delete contact"));
   if (ret == KMessageBox::Yes) {
      m_pCurrentPerson->remove();
   }
}

///Called when a call is dropped on transfer
void ContactDock::transferEvent(QMimeData* data)
{
   if (data->hasFormat( RingMimes::CALLID)) {
      bool ok = false;
      const ContactMethod* result = showNumberSelector(ok);
      if (ok && result) {
         Call* call = CallModel::instance()->fromMime(data->data(RingMimes::CALLID));
         if (dynamic_cast<Call*>(call)) {
//             call->changeCurrentState(Call::State::TRANSFERRED);
            CallModel::instance()->transfer(call, result);
         }
      }
   }
   else
      qDebug() << "Invalid mime data";
//    m_pBtnTrans->setHoverState(false);
//    m_pBtnTrans->setVisible(false);
}

/*****************************************************************************
 *                                                                           *
 *                                Drag and Drop                              *
 *                                                                           *
 ****************************************************************************/

///Serialize information to be used for drag and drop
// QMimeData* PersonTree::mimeData( const QList<QTreeWidgetItem *> items) const
// {
//    qDebug() << "An history call is being dragged";
//    if (items.size() < 1) {
//       return nullptr;
//    }
// 
//    QMimeData *mimeData = new QMimeData();
// 
//    //Person
//    if (dynamic_cast<QNumericTreeWidgetItem_hist*>(items[0])) {
//       QNumericTreeWidgetItem_hist* item = dynamic_cast<QNumericTreeWidgetItem_hist*>(items[0]);
//       if (item->widget != 0) {
//          mimeData->setData(RingMimes::CONTACT, item->widget->getPerson()->getUid().toUtf8());
//       }
//       else if (!item->number.isEmpty()) {
//          mimeData->setData(RingMimes::PHONENUMBER, item->number.toUtf8());
//       }
//    }
//    else {
//       qDebug() << "the item is not a call";
//    }
//    return mimeData;
// } //mimeData

///Handle data being dropped on the widget
// bool PersonTree::dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action)
// {
//    Q_UNUSED(index )
//    Q_UNUSED(action)
//    Q_UNUSED(parent)
// 
//    QByteArray encodedData = data->data(RingMimes::CALLID);
// 
//    qDebug() << "In history import"<< QString(encodedData);
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
   qDebug() << "Toggling history visibility";
   m_pBottomWidget->setVisible(visible);
   ConfigurationSkeleton::setDisplayPersonCallHistory(visible);
}

///Set sorting category
void ContactDock::setCategory(int index)
{
   switch(index) {
      case SortingCategory::Name:
         m_pSourceModel->setRole(Qt::DisplayRole);
         m_pProxyModel->setSortRole(Qt::DisplayRole);
         break;
      case SortingCategory::Organization:
         m_pProxyModel->setSortRole(PersonModel::Role::Organization);
         m_pSourceModel->setRole(PersonModel::Role::Organization);
         break;
      case SortingCategory::RecentlyUsed:
         m_pSourceModel->setRole(PersonModel::Role::FormattedLastUsed);
         m_pProxyModel->setSortRole(PersonModel::Role::IndexedLastUsed);
         break;
      case SortingCategory::Group:
         m_pSourceModel->setRole(PersonModel::Role::Group);
         m_pProxyModel->setSortRole(PersonModel::Role::Group);
         break;
      case SortingCategory::Department:
         m_pSourceModel->setRole(PersonModel::Role::Department);
         m_pProxyModel->setSortRole(PersonModel::Role::Department);
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
//             Ring::app()->view()->selectCallContactMethod(&call,item->widget->getPerson());
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
