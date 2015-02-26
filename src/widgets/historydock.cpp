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
#include "historydock.h"

//Qt
#include <QtCore/QString>
#include <QtCore/QDate>
#include <QtCore/QProcess>
#include <QtCore/QTimer>
#include <QtCore/QMimeData>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMenu>
#include <QtWidgets/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QKeyEvent>

//KDE
#include <QDebug>
#include <QIcon>
#include <KDatePicker>
#include <klocalizedstring.h>
#include <QAction>
#include <KColorScheme>

//Ring
#include "ring.h"
#include "widgets/categorizedtreeview.h"
#include "widgets/bookmarkdock.h"
#include "klib/kcfg_settings.h"
#include "historymodel.h"
#include "accountmodel.h"
#include "availableaccountmodel.h"
#include "callmodel.h"
#include "mime.h"
#include "contactmethod.h"
#include "phonedirectorymodel.h"
#include "collectioninterface.h"
#include "personmodel.h"
#include "../delegates/categorizeddelegate.h"
#include "../delegates/historydelegate.h"

//Ring library
#include <bookmarkmodel.h>
#include "klib/helperfunctions.h"


#define CURRENT_SORTING_MODE m_pSortByCBB->currentIndex()

///Event filter allowing to write text on the Tree widget to filter it.
bool KeyPressEater::eventFilter(QObject *obj, QEvent *event)
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

bool HistorySortFilterProxyModel::filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const
{
   if (!source_parent.isValid() ) { //Is a category
      for (int i=0;i<HistoryModel::instance()->rowCount(HistoryModel::instance()->index(source_row,0,source_parent));i++) {
         if (filterAcceptsRow(i, HistoryModel::instance()->index(source_row,0,source_parent)))
            return true;
      }
   }
   ///If date range is enabled, display only this range
   else if (ConfigurationSkeleton::displayDataRange() && false/*FIXME force disabled for 1.3.0, can SEGFAULT*/) {
      const int start = source_parent.child(source_row,0).data(Call::Role::StartTime).toInt();
      const int stop  = source_parent.child(source_row,0).data(Call::Role::StopTime ).toInt();
      if (!(start > m_pParent->startTime()) || !(m_pParent->stopTime() > stop))
         return false;
   }

   return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

QVariant HistorySortFilterProxyModel::data(const QModelIndex& index, int role) const
{
   //If the user turned on highlight missed, then set a background color
   static const bool highlightMissedIn  = ConfigurationSkeleton::highlightMissedIncomingCalls();
   static const bool highlightMissedOut = ConfigurationSkeleton::highlightMissedOutgoingCalls();
   const bool missed = QSortFilterProxyModel::data(index,Call::Role::Missed).toBool();
   const Call::Direction dir = static_cast<Call::Direction>(QSortFilterProxyModel::data(index,Call::Role::Direction2).toInt());

   if (index.isValid()
      && role == Qt::BackgroundRole 
      && missed  &&  (
         (dir == Call::Direction::INCOMING && highlightMissedIn)
         ||
         (dir == Call::Direction::OUTGOING && highlightMissedOut)
   )) {
      static bool initColor = false;
      static QColor awayBrush = KStatefulBrush( KColorScheme::Window, KColorScheme::NegativeText ).brush(QPalette::Normal).color();

      if (!initColor) {
         awayBrush.setAlpha(30);
         initColor = true;
      }
      return awayBrush;
   }

   return QSortFilterProxyModel::data(index,role);
}

///Constructor
HistoryDock::HistoryDock(QWidget* parent) : QDockWidget(parent),m_pMenu(nullptr),m_pRemove(nullptr),
m_pCallAgain(nullptr)
{
   setObjectName("historyDock");
   QWidget* mainWidget = new QWidget(this);
   setupUi(mainWidget);
   setMinimumSize(250,0);
   setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
   m_pFromL      = new QLabel      ( i18n("From:"), m_pBottomWidget           );
   m_pToL        = new QLabel      ( i18nc("To date:","To:"), m_pBottomWidget );
   m_pFromDW     = new KDatePicker ( m_pBottomWidget                          );
   m_pToDW       = new KDatePicker ( m_pBottomWidget                          );
   m_pAllTimeCB  = new QCheckBox   ( i18n("Display all")                      );
   m_pLinkPB     = new QPushButton ( m_pBottomWidget                          );

   CategorizedDelegate* delegate = new CategorizedDelegate(m_pView);
   delegate->setChildDelegate(new HistoryDelegate(m_pView));
   m_pView->setDelegate(delegate);
   m_pView->setViewType(CategorizedTreeView::ViewType::History);
   m_pProxyModel = new HistorySortFilterProxyModel(this);
   m_pProxyModel->setSourceModel(HistoryModel::instance());
   m_pProxyModel->setSortRole(Call::Role::Date);
   m_pProxyModel->setSortLocaleAware(true);
   m_pProxyModel->setFilterRole(Call::Role::Filter);
   m_pProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
   m_pProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
   m_pView->setModel(m_pProxyModel);
   m_pKeyPressEater = new KeyPressEater(this);
   m_pView->installEventFilter(m_pKeyPressEater);
   m_pView->setSortingEnabled(true);
   m_pView->sortByColumn(0,Qt::DescendingOrder);
   connect(m_pView,SIGNAL(contextMenuRequest(QModelIndex)), this, SLOT(slotContextMenu(QModelIndex)));
   connect(m_pView,SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotDoubleClick(QModelIndex)));
   connect(m_pFilterLE ,SIGNAL(filterStringChanged(QString)), m_pProxyModel , SLOT(setFilterRegExp(QString)));
   connect(m_pFilterLE ,SIGNAL(textChanged(QString)), this , SLOT(expandTree()));
   connect(m_pProxyModel,SIGNAL(modelReset()), this , SLOT(expandTree()));
   connect(HistoryModel::instance() ,SIGNAL(layoutChanged()), this , SLOT(expandTree())                );
   expandTree();

   m_pAllTimeCB->setChecked(!ConfigurationSkeleton::displayDataRange());
   m_pAllTimeCB->setVisible(false);
   enableDateRange(!ConfigurationSkeleton::displayDataRange());

   m_pSortByL->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred);
   m_pSortByCBB->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
   m_pLinkPB->setMaximumSize(20,9999999);
   m_pLinkPB->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
   m_pLinkPB->setCheckable(true);

   m_pFilterLE->setPlaceholderText(i18n("Filter"));
   m_pFilterLE->setClearButtonEnabled(true);

   QStringList sortBy;
   sortBy << i18nc("Sort by date","Date") << i18nc("Sort by Name","Name") << i18nc("Sort by Popularity","Popularity") << i18nc("Sort by Length","Length") << i18nc("sort by spent time","Spent time");
   m_pSortByCBB->addItems(sortBy);

   setWidget(mainWidget);
   m_pTopWidget->layout()->addWidget(m_pAllTimeCB);

   QGridLayout* mainLayout = new QGridLayout();
   mainLayout->addWidget(m_pLinkPB ,1,2,3,1 );
   mainLayout->addWidget(m_pFromL  ,0,0,1,2 );
   mainLayout->addWidget(m_pFromDW ,1,0,1,2 );
   mainLayout->addWidget(m_pToL    ,2,0,1,2 );
   mainLayout->addWidget(m_pToDW   ,3,0,1,2 );
   splitter->setStretchFactor(0,99);
   m_pBottomWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
   m_pBottomWidget->layout()->addItem(mainLayout);

   setWindowTitle(i18nc("History tab","History"));

   QDate date(2000,1,1);
   m_pFromDW->setDate(date);

   m_pSortByCBB->setCurrentIndex(ConfigurationSkeleton::historySortMode());

   connect(m_pAllTimeCB, SIGNAL(toggled(bool)),            this, SLOT(enableDateRange(bool)) );
   connect(m_pSortByCBB, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSetSortRole(int))  );
   connect(m_pToDW,SIGNAL(changed(QDate)),this,SLOT(slotDateRangeCanched()));
   connect(m_pFromDW,SIGNAL(changed(QDate)),this,SLOT(slotDateRangeCanched()));
   QTimer::singleShot(0,this,SLOT(slotDateRangeCanched()));

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

   if (m_pCallAgain) {
      delete m_pCallAgain    ;
      delete m_pAddPerson   ;
      delete m_pCopy         ;
      delete m_pEmail        ;
      delete m_pRemove       ;
      delete m_pAddToPerson ;
      delete m_pBookmark     ;
   }
} //~HistoryDock


/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/


///Enable the ability to set a date range like 1 month to limit history
void HistoryDock::enableDateRange(bool disable)
{
   m_pBottomWidget->setHidden(disable || true /*FIXME disabled for 1.3.0*/);
   ConfigurationSkeleton::setDisplayDataRange(!disable && false);
}

void HistoryDock::slotSetSortRole(int role)
{
   switch (role) {
      case HistoryDock::Role::Date:
         HistoryModel::instance()->setCategoryRole(Call::Role::FuzzyDate);
         m_pProxyModel->setSortRole(Call::Role::Date);
         break;
      case HistoryDock::Role::Name:
         HistoryModel::instance()->setCategoryRole(Call::Role::Name);
         m_pProxyModel->setSortRole(Call::Role::Name);
         break;
      case HistoryDock::Role::Popularity:
         HistoryModel::instance()->setCategoryRole(Call::Role::CallCount);
         m_pProxyModel->setSortRole(Call::Role::CallCount);
         break;
      case HistoryDock::Role::Length:
         HistoryModel::instance()->setCategoryRole(Call::Role::Length);
         m_pProxyModel->setSortRole(Call::Role::Length);
         break;
      case HistoryDock::Role::SpentTime:
         HistoryModel::instance()->setCategoryRole(Call::Role::TotalSpentTime);
         m_pProxyModel->setSortRole(Call::Role::TotalSpentTime);
         break;
   }
}


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
   if (((CategorizedCompositeNode*)idx.internalPointer())->type() != CategorizedCompositeNode::Type::CALL)
      return;
   if (!m_pMenu) {
      m_pCallAgain    = new QAction(this);
      m_pAddPerson   = new QAction(this);
      m_pCopy         = new QAction(this);
      m_pEmail        = new QAction(this);
      m_pRemove       = new QAction(this);
      m_pAddToPerson = new QAction(this);
      m_pBookmark     = new QAction(this);

      m_pCallAgain->setShortcut    ( Qt::Key_Enter                  );
      m_pCallAgain->setText        ( i18n("Call Again")             );
      m_pCallAgain->setIcon        ( QIcon::fromTheme("call-start")            );

      m_pAddToPerson->setShortcut ( Qt::CTRL + Qt::Key_E           );
      m_pAddToPerson->setText     ( i18n("Add Number to Person")  );
      m_pAddToPerson->setIcon     ( QIcon::fromTheme("list-resource-add")     );
      m_pAddToPerson->setDisabled ( true                           );

      m_pAddPerson->setShortcut   ( Qt::CTRL + Qt::Key_E           );
      m_pAddPerson->setText       ( i18n("Add Person")            );
      m_pAddPerson->setIcon       ( QIcon::fromTheme("contact-new")           );

      m_pCopy->setShortcut         ( Qt::CTRL + Qt::Key_C           );
      m_pCopy->setText             ( i18n("Copy")                   );
      m_pCopy->setIcon             ( QIcon::fromTheme("edit-copy")             );

      m_pEmail->setShortcut        ( Qt::CTRL + Qt::Key_M           );
      m_pEmail->setText            ( i18n("Send Email")             );
      m_pEmail->setIcon            ( QIcon::fromTheme("mail-message-new")      );
      m_pEmail->setDisabled        ( true                           );

      m_pRemove->setShortcut       ( Qt::Key_Shift + Qt::Key_Delete );
      m_pRemove->setText           ( i18n("Remove")                 );
      m_pRemove->setIcon           ( QIcon::fromTheme("edit-delete")           );

      m_pBookmark->setShortcut     ( Qt::CTRL + Qt::Key_D           );
      m_pBookmark->setText         ( i18n("Bookmark")               );
      if (!idx.data(Call::Role::IsBookmark).toBool()) {
         m_pBookmark->setText      ( i18n("Bookmark")               );
         m_pBookmark->setIcon      ( QIcon::fromTheme("bookmarks")             );
      }
      else {
         m_pBookmark->setText      ( i18n("Remove bookmark")        );
         m_pBookmark->setIcon      ( QIcon::fromTheme("edit-delete")           );
      }

      m_pMenu = new QMenu(this);
      m_pMenu->addAction( m_pCallAgain    );
      m_pMenu->addAction( m_pAddPerson   );
      m_pMenu->addAction( m_pAddToPerson );
      m_pMenu->addAction( m_pCopy         );
      m_pMenu->addAction( m_pEmail        );
      m_pMenu->addAction( m_pRemove       );
      m_pMenu->addAction( m_pBookmark     );
      connect(m_pCallAgain   , SIGNAL(triggered()) , this , SLOT(slotCallAgain())   );
      connect(m_pAddPerson   , SIGNAL(triggered()) , this , SLOT(slotAddPerson())   );
      connect(m_pCopy        , SIGNAL(triggered()) , this , SLOT(slotCopy())        );
      connect(m_pEmail       , SIGNAL(triggered()) , this , SLOT(slotSendEmail())   );
      connect(m_pRemove      , SIGNAL(triggered()) , this , SLOT(slotRemove())      );
      connect(m_pAddToPerson , SIGNAL(triggered()) , this , SLOT(slotAddToPerson()) );
      connect(m_pBookmark    , SIGNAL(triggered()) , this , SLOT(slotBookmark())    );
   }
   m_pCurrentCall = static_cast<Call*>(static_cast<CategorizedCompositeNode*>(idx.internalPointer())->getSelf());
   enableRemove();
   m_pMenu->exec(QCursor::pos());
}

void HistoryDock::enableRemove()
{
   if (m_pRemove)
      m_pRemove->setEnabled(m_pCurrentCall && m_pCurrentCall->collection() && m_pCurrentCall->collection()->supportedFeatures() & CollectionInterface::SupportedFeatures::REMOVE);
}

void HistoryDock::slotSendEmail()
{
   if (!m_pCurrentCall) return;
   qDebug() << "Sending email";
   QProcess *myProcess = new QProcess(this);
   QStringList arguments;
   Person* ct = m_pCurrentCall->peerContactMethod()->contact();
   if (ct)
      myProcess->start("xdg-email", (arguments << ct->preferredEmail()));
}

void HistoryDock::slotRemove()
{
   if (m_pCurrentCall && m_pCurrentCall->collection()->supportedFeatures() & CollectionInterface::SupportedFeatures::REMOVE) {
      HistoryModel::instance()->deleteItem(m_pCurrentCall); //TODO add add and remove to the manager
   }
}

void HistoryDock::slotCallAgain()
{
   if (!m_pCurrentCall) return;
   qDebug() << "Calling "<< m_pCurrentCall->peerContactMethod();
   Call* call = CallModel::instance()->dialingCall(m_pCurrentCall->peerName(), AvailableAccountModel::currentDefaultAccount());
   if (call) {
      call->setDialNumber  ( m_pCurrentCall->peerContactMethod() );
      call->setAccount     ( m_pCurrentCall->account()         );
      call->setPeerName    ( m_pCurrentCall->peerName()        );
      call->performAction  ( Call::Action::ACCEPT              );
   }
   else {
      HelperFunctions::displayNoAccountMessageBox(this);
   }
}

void HistoryDock::slotCopy()
{
   if (!m_pCurrentCall) {
      qDebug() << "No call to copy";
      return;
   }

   qDebug() << "Copying contact";
   QMimeData* mimeData = new QMimeData();
   mimeData->setData(RingMimes::CALLID, CallModel::instance()->getMime(m_pCurrentCall));

   mimeData->setData(RingMimes::PHONENUMBER, m_pCurrentCall->peerContactMethod()->uri().toUtf8());

   QString numbers,numbersHtml;
   const Person* ct = m_pCurrentCall->peerContactMethod()->contact();

   if (ct) {
      numbers     = ct->formattedName()+": "+m_pCurrentCall->peerContactMethod()->uri();
      numbersHtml = "<b>"+ct->formattedName()+"</b><br />"+HelperFunctions::escapeHtmlEntities(m_pCurrentCall->peerContactMethod()->uri());
   }
   else {
      numbers     = m_pCurrentCall->peerName()+": "+m_pCurrentCall->peerContactMethod()->uri();
      numbersHtml = "<b>"+m_pCurrentCall->peerName()+"</b><br />"+HelperFunctions::escapeHtmlEntities(m_pCurrentCall->peerContactMethod()->uri());
   }

   mimeData->setData("text/plain", numbers.toUtf8()    );
   mimeData->setData("text/html",  numbersHtml.toUtf8());

   QApplication::clipboard()->setMimeData(mimeData);
}

void HistoryDock::slotAddPerson()
{
   qDebug() << "Adding contact";
   Person* aPerson = new Person();
   Person::ContactMethods numbers(aPerson);
   numbers << PhoneDirectoryModel::instance()->getNumber(m_pCurrentCall->peerContactMethod()->uri(),aPerson,nullptr, "Home");//new ContactMethod(m_pCurrentCall->peerContactMethod(), "Home");
   aPerson->setContactMethods(numbers);
   aPerson->setFormattedName(m_pCurrentCall->peerName());
   PersonModel::instance()->addNewPerson(aPerson);
}

void HistoryDock::slotAddToPerson()
{
   //TODO
   qDebug() << "Adding to contact";
}

void HistoryDock::slotBookmark()
{
   BookmarkModel::instance()->addBookmark(m_pCurrentCall->peerContactMethod());
}

void HistoryDock::slotDoubleClick(const QModelIndex& index)
{
   if (!index.isValid())
      return;
   QModelIndex idx = (static_cast<const HistorySortFilterProxyModel*>(index.model()))->mapToSource(index);
   if (!idx.isValid() || !idx.parent().isValid())
      return;
   if (((CategorizedCompositeNode*)idx.internalPointer())->type() != CategorizedCompositeNode::Type::CALL)
      return;
   m_pCurrentCall = static_cast<Call*>(static_cast<CategorizedCompositeNode*>(idx.internalPointer())->getSelf());
   enableRemove();
   slotCallAgain();
}

void HistoryDock::slotDateRangeCanched()
{
   m_StopTime  = QDateTime(m_pToDW->date  ()).toTime_t() + 24*3600 - 1;
   m_StartTime = QDateTime(m_pFromDW->date()).toTime_t();
   m_pProxyModel->invalidate();
}

time_t HistoryDock::stopTime () const
{
   return m_StopTime;
}

time_t HistoryDock::startTime() const
{
   return m_StartTime;
}
