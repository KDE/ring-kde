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
#include "historydock.h"

//Qt
#include <QtCore/QString>
#include <QtCore/QDate>
#include <QtCore/QPoint>
#include <QtCore/QProcess>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QMenu>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QKeyEvent>

//KDE
#include <KDebug>
#include <KIcon>
#include <KLineEdit>
#include <KDateWidget>
#include <KComboBox>
#include <KAction>

//SFLPhone
#include "sflphone.h"
#include "widgets/categorizedtreeview.h"
#include "widgets/bookmarkdock.h"
#include "klib/akonadibackend.h"
#include "klib/configurationskeleton.h"
#include "lib/historymodel.h"
#include "lib/accountlist.h"
#include "../delegates/categorizeddelegate.h"
#include "../delegates/historydelegate.h"

//SFLPhone library
#include "klib/helperfunctions.h"
#include "lib/sflphone_const.h"


#define CURRENT_SORTING_MODE m_pSortByCBB->currentIndex()

///Event filter allowing to write text on the Tree widget to filter it.
bool KeyPressEater::eventFilter(QObject *obj, QEvent *event)
{
   if (event->type() == QEvent::KeyPress) {
      m_pDock->keyPressEvent((QKeyEvent*)event);
      return true;
   } else {
      // standard event processing
      return QObject::eventFilter(obj, event);
   }
}

///Constructor
HistoryDock::HistoryDock(QWidget* parent) : QDockWidget(parent),m_pMenu(nullptr)
{
   setObjectName("historyDock");
   QWidget* mainWidget = new QWidget(this);
   setupUi(mainWidget);
   setMinimumSize(250,0);
   setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
   m_pFromL      = new QLabel      ( i18n("From:"), m_pBottomWidget           );
   m_pToL        = new QLabel      ( i18nc("To date:","To:"), m_pBottomWidget );
   m_pFromDW     = new KDateWidget ( m_pBottomWidget                          );
   m_pToDW       = new KDateWidget ( m_pBottomWidget                          );
   m_pAllTimeCB  = new QCheckBox   ( i18n("Display all")                      );
   m_pLinkPB     = new QPushButton ( m_pBottomWidget                          );

   SortedTreeDelegate* delegate = new SortedTreeDelegate(m_pView);
   delegate->setChildDelegate(new HistoryDelegate(m_pView));
   m_pView->setDelegate(delegate);
   m_pProxyModel = new HistorySortFilterProxyModel(this);
   m_pProxyModel->setSourceModel(HistoryModel::self());
   m_pProxyModel->setSortRole(Call::Role::Date);
   m_pProxyModel->setSortLocaleAware(true);
   m_pProxyModel->setFilterRole(Call::Role::Filter);
   m_pProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
   m_pProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
   m_pView->setModel(m_pProxyModel);
   m_pKeyPressEater = new KeyPressEater(this);
   m_pView->installEventFilter(m_pKeyPressEater);
   m_pView->setSortingEnabled(true);
   m_pView->sortByColumn(0,Qt::AscendingOrder);
   connect(m_pView,SIGNAL(contextMenuRequest(QModelIndex)), this, SLOT(slotContextMenu(QModelIndex)));
   connect(m_pView,SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotDoubleClick(QModelIndex)));
   connect(m_pFilterLE ,SIGNAL(textChanged(QString)), m_pProxyModel , SLOT(setFilterRegExp(QString)));
   connect(m_pFilterLE ,SIGNAL(textChanged(QString)), this , SLOT(expandTree()));
   connect(HistoryModel::self() ,SIGNAL(layoutChanged()), this , SLOT(expandTree())                );
   expandTree();

   m_pAllTimeCB->setChecked(!ConfigurationSkeleton::displayDataRange());
   enableDateRange(!ConfigurationSkeleton::displayDataRange());

   m_pSortByL->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred);
   m_pSortByCBB->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
   m_pLinkPB->setMaximumSize(20,9999999);
   m_pLinkPB->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
   m_pLinkPB->setCheckable(true);

   m_pFilterLE->setPlaceholderText(i18n("Filter"));
   m_pFilterLE->setClearButtonShown(true);

   QStringList sortBy;
   sortBy << i18nc("Sort by date","Date") << i18nc("Sort by Name","Name") << i18nc("Sort by Popularity","Popularity") << i18nc("Sort by Length","Length");
   m_pSortByCBB->addItems(sortBy);

   setWidget(mainWidget);
   m_pTopWidget->layout()->addWidget(m_pAllTimeCB);

   QGridLayout* mainLayout = new QGridLayout();
   mainLayout->addWidget(m_pLinkPB    ,1,2,3,1 );
   mainLayout->addWidget(m_pFromL     ,0,0,1,2 );
   mainLayout->addWidget(m_pFromDW    ,1,0,1,2 );
   mainLayout->addWidget(m_pToL       ,2,0,1,2 );
   mainLayout->addWidget(m_pToDW      ,3,0,1,2 );
   splitter->setStretchFactor(0,99);
   m_pBottomWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
   m_pBottomWidget->layout()->addItem(mainLayout);

   setWindowTitle(i18n("History"));

   QDate date(2000,1,1);
   m_pFromDW->setDate(date);

   m_CurrentFromDate = m_pFromDW->date();
   m_CurrentToDate   = m_pToDW->date();

   m_pSortByCBB->setCurrentIndex(ConfigurationSkeleton::historySortMode());

   connect(m_pAllTimeCB,                   SIGNAL(toggled(bool)),            this, SLOT(enableDateRange(bool))      );
//    connect(m_pFilterLE,                    SIGNAL(textChanged(QString)),     this, SLOT(filter(QString))            );
//    connect(m_pFromDW  ,                    SIGNAL(changed(QDate)),           this, SLOT(updateLinkedFromDate(QDate)));
//    connect(m_pToDW    ,                    SIGNAL(changed(QDate)),           this, SLOT(updateLinkedToDate(QDate))  );
   connect(m_pSortByCBB,                   SIGNAL(currentIndexChanged(int)), this, SLOT(slotSetSortRole(int))       );
//    connect(AkonadiBackend::instance(),  SIGNAL(collectionChanged()),      this, SLOT(updateContactInfo())        );
//    connect(HistoryModel::self()         ,  SIGNAL(newHistoryCall(Call*)),    this, SLOT(newHistoryCall(Call*))      );

} //HistoryDock

///Destructor
HistoryDock::~HistoryDock()
{
   delete m_pFilterLE     ;
   delete m_pSortByCBB    ;
   delete m_pSortByL      ;
   delete m_pFromL        ;
   delete m_pToL          ;
   delete m_pFromDW       ;
   delete m_pToDW         ;
   delete m_pAllTimeCB    ;
   delete m_pLinkPB       ;
   delete m_pKeyPressEater;
} //~HistoryDock


/*****************************************************************************
 *                                                                           *
 *                                  Getters                                  *
 *                                                                           *
 ****************************************************************************/


/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/


///Enable the ability to set a date range like 1 month to limit history
void HistoryDock::enableDateRange(bool disable)
{
   m_pBottomWidget->setHidden(disable);
   ConfigurationSkeleton::setDisplayDataRange(!disable);
}

void HistoryDock::slotSetSortRole(int role)
{
   switch (role) {
      case HistoryDock::Role::Date:
         HistoryModel::self()->setCategoryRole(Call::Role::FuzzyDate);
         m_pProxyModel->setSortRole(Call::Role::Date);
         break;
      case HistoryDock::Role::Name:
         HistoryModel::self()->setCategoryRole(Call::Role::Name);
         m_pProxyModel->setSortRole(Call::Role::Name);
         break;
      case HistoryDock::Role::Popularity:
         //          m_pProxyModel->setSortRole(Call::Role::Name);
         //TODO
         break;
      case HistoryDock::Role::Length:
         HistoryModel::self()->setCategoryRole(Call::Role::Length);
         m_pProxyModel->setSortRole(Call::Role::Length);
         break;
   }
}

/*****************************************************************************
 *                                                                           *
 *                             Drag and drop                                 *
 *                                                                           *
 ****************************************************************************/

///Generate serializerd version of the content
// QMimeData* HistoryTree::mimeData( const QList<QTreeWidgetItem *> items) const
// {
//    kDebug() << "An history call is being dragged";
//    if (items.size() < 1) {
//       return nullptr;
//    }
// 
//    QMimeData *mimeData = new QMimeData();
// 
//    //Contact
//    if (dynamic_cast<QNumericTreeWidgetItem*>(items[0])) {
//       QNumericTreeWidgetItem* item = dynamic_cast<QNumericTreeWidgetItem*>(items[0]);
//       if (item->widget != 0) {
//          mimeData->setData(MIME_PHONENUMBER, item->widget->call()->getPeerPhoneNumber().toUtf8());
//       }
//    }
//    else {
//       kDebug() << "the item is not a call";
//    }
//    return mimeData;
// } //mimeData

///Handle what happen when serialized data is dropped
// bool HistoryTree::dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action)
// {
//    Q_UNUSED( index  )
//    Q_UNUSED( action )
//    Q_UNUSED( parent )
// 
//    QByteArray encodedData = data->data(MIME_CALLID);
// 
//    kDebug() << "In history import"<< QString(encodedData);
// 
//    return false;
// }


/*****************************************************************************
 *                                                                           *
 *                              Keyboard handling                            *
 *                                                                           *
 ****************************************************************************/

///Handle keyboard input and redirect them to the filterbox
void HistoryDock::keyPressEvent(QKeyEvent* event) {
   int key = event->key();
   if(key == Qt::Key_Escape)
      m_pFilterLE->setText(QString());
   else if (key == Qt::Key_Return || key == Qt::Key_Enter) {
      slotDoubleClick(m_pView->selectionModel()->currentIndex());
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
void HistoryDock::expandTree()
{
   m_pView->expandToDepth(1);
}


/*****************************************************************************
 *                                                                           *
 *                                Menu handling                              *
 *                                                                           *
 ****************************************************************************/


void HistoryDock::slotContextMenu(const QModelIndex& index)
{
   QModelIndex idx = (static_cast<const HistorySortFilterProxyModel*>(index.model()))->mapToSource(index);
   if (((HistoryTreeBackend*)idx.internalPointer())->type3() != HistoryTreeBackend::Type::CALL)
      return;
   if (!m_pMenu) {
      m_pCallAgain    = new KAction(this);
      m_pAddContact   = new KAction(this);
      m_pCopy         = new KAction(this);
      m_pEmail        = new KAction(this);
      m_pAddToContact = new KAction(this);
      m_pBookmark     = new KAction(this);

      m_pCallAgain->setShortcut    ( Qt::Key_Enter                  );
      m_pCallAgain->setText        ( i18n("Call Again")             );
      m_pCallAgain->setIcon        ( KIcon("call-start")            );

      m_pAddToContact->setShortcut ( Qt::CTRL + Qt::Key_E           );
      m_pAddToContact->setText     ( i18n("Add Number to Contact")  );
      m_pAddToContact->setIcon     ( KIcon("list-resource-add")     );
      m_pAddToContact->setDisabled ( true                           );

      m_pAddContact->setShortcut   ( Qt::CTRL + Qt::Key_E           );
      m_pAddContact->setText       ( i18n("Add Contact")            );
      m_pAddContact->setIcon       ( KIcon("contact-new")           );

      m_pCopy->setShortcut         ( Qt::CTRL + Qt::Key_C           );
      m_pCopy->setText             ( i18n("Copy")                   );
      m_pCopy->setIcon             ( KIcon("edit-copy")             );

      m_pEmail->setShortcut        ( Qt::CTRL + Qt::Key_M           );
      m_pEmail->setText            ( i18n("Send Email")             );
      m_pEmail->setIcon            ( KIcon("mail-message-new")      );
      m_pEmail->setDisabled        ( true                           );

      m_pBookmark->setShortcut     ( Qt::CTRL + Qt::Key_D           );
      m_pBookmark->setText         ( i18n("Bookmark")               );
      if (!idx.data(Call::Role::IsBookmark).toBool()) {
         m_pBookmark->setText      ( i18n("Bookmark")               );
         m_pBookmark->setIcon      ( KIcon("bookmarks")             );
      }
      else {
         m_pBookmark->setText      ( i18n("Remove bookmark")        );
         m_pBookmark->setIcon      ( KIcon("edit-delete")           );
      }
      
      m_pMenu = new QMenu(this);
      m_pMenu->addAction( m_pCallAgain    );
      m_pMenu->addAction( m_pAddContact   );
      m_pMenu->addAction( m_pAddToContact );
      m_pMenu->addAction( m_pCopy         );
      m_pMenu->addAction( m_pEmail        );
      m_pMenu->addAction( m_pBookmark     );
      connect(m_pCallAgain    , SIGNAL(triggered())                        , this , SLOT(slotCallAgain())        );
      connect(m_pAddContact   , SIGNAL(triggered())                        , this , SLOT(slotAddContact())       );
      connect(m_pCopy         , SIGNAL(triggered())                        , this , SLOT(slotCopy())             );
      connect(m_pEmail        , SIGNAL(triggered())                        , this , SLOT(slotSendEmail())        );
      connect(m_pAddToContact , SIGNAL(triggered())                        , this , SLOT(slotAddToContact())     );
      connect(m_pBookmark     , SIGNAL(triggered())                        , this , SLOT(slotBookmark())         );
   }
   m_pCurrentCall = (Call*)static_cast<HistoryTreeBackend*>(idx.internalPointer())->getSelf();
   m_pMenu->exec(QCursor::pos());
}

void HistoryDock::slotSendEmail()
{
   if (!m_pCurrentCall) return;
   kDebug() << "Sending email";
   QProcess *myProcess = new QProcess(this);
   QStringList arguments;
   Contact* ct = m_pCurrentCall->contact();
   if (ct)
      myProcess->start("xdg-email", (arguments << ct->preferredEmail()));
}

void HistoryDock::slotCallAgain()
{
   if (!m_pCurrentCall) return;
   kDebug() << "Calling "<< m_pCurrentCall->peerPhoneNumber();
   Call* call = SFLPhone::model()->addDialingCall(m_pCurrentCall->peerName(), AccountList::currentAccount());
   if (call) {
      call->setCallNumber  ( m_pCurrentCall->peerPhoneNumber() );
      call->setPeerName    ( m_pCurrentCall->peerName() );
      call->actionPerformed( Call::Action::ACCEPT   );
   }
   else {
      HelperFunctions::displayNoAccountMessageBox(this);
   }
}

void HistoryDock::slotCopy()
{
   if (!m_pCurrentCall) {
      kDebug() << "No call to copy";
      return;
   }

   kDebug() << "Copying contact";
   QMimeData* mimeData = new QMimeData();
   mimeData->setData(MIME_CALLID, m_pCurrentCall->callId().toUtf8());

   mimeData->setData(MIME_PHONENUMBER, m_pCurrentCall->peerPhoneNumber().toUtf8());

   QString numbers,numbersHtml;
   const Contact* ct = m_pCurrentCall->contact();

   if (ct) {
      numbers     = ct->formattedName()+": "+m_pCurrentCall->peerPhoneNumber();
      numbersHtml = "<b>"+ct->formattedName()+"</b><br />"+HelperFunctions::escapeHtmlEntities(m_pCurrentCall->peerPhoneNumber());
   }
   else {
      numbers     = m_pCurrentCall->peerName()+": "+m_pCurrentCall->peerPhoneNumber();
      numbersHtml = "<b>"+m_pCurrentCall->peerName()+"</b><br />"+HelperFunctions::escapeHtmlEntities(m_pCurrentCall->peerPhoneNumber());
   }

   mimeData->setData("text/plain", numbers.toUtf8()    );
   mimeData->setData("text/html",  numbersHtml.toUtf8());

   QApplication::clipboard()->setMimeData(mimeData);
}

void HistoryDock::slotAaddContact()
{
   kDebug() << "Adding contact";
   Contact* aContact = new Contact();
   Contact::PhoneNumbers numbers(aContact);
   numbers << new Contact::PhoneNumber(m_pCurrentCall->peerPhoneNumber(), "Home");
   aContact->setPhoneNumbers(numbers);
   aContact->setFormattedName(m_pCurrentCall->peerName());
   AkonadiBackend::instance()->addNewContact(aContact);
}

void HistoryDock::slotAddToContact()
{
   //TODO
   kDebug() << "Adding to contact";
}

void HistoryDock::slotBookmark()
{
//    if (!m_IsBookmark)
      SFLPhone::app()->bookmarkDock()->addBookmark(m_pCurrentCall->peerPhoneNumber());
//    else
//       SFLPhone::app()->bookmarkDock()->removeBookmark(m_PhoneNumber);
}

void HistoryDock::slotDoubleClick(const QModelIndex& index)
{
   QModelIndex idx = (static_cast<const HistorySortFilterProxyModel*>(index.model()))->mapToSource(index);
   if (!idx.isValid() || !idx.parent().isValid())
      return;
   if (((HistoryTreeBackend*)idx.internalPointer())->type3() != HistoryTreeBackend::Type::CALL)
      return;
   m_pCurrentCall = (Call*)static_cast<HistoryTreeBackend*>(idx.internalPointer())->getSelf();
   slotCallAgain();
}
