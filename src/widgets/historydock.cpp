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
#include <QtWidgets/QDialog>
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
#include "categorizedhistorymodel.h"
#include "accountmodel.h"
#include "availableaccountmodel.h"
#include "callmodel.h"
#include "mime.h"
#include "contactmethod.h"
#include "phonedirectorymodel.h"
#include "collectioninterface.h"
#include "personmodel.h"
#include "conf/dlgprofiles.h"
#include "../delegates/categorizeddelegate.h"
#include "../delegates/historydelegate.h"

//Ring library
#include <categorizedbookmarkmodel.h>
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

/*QVariant HistorySortFilterProxyModel::data(const QModelIndex& index, int role) const
{
   //If the user turned on highlight missed, then set a background color
   static const bool highlightMissedIn  = ConfigurationSkeleton::highlightMissedIncomingCalls();
   static const bool highlightMissedOut = ConfigurationSkeleton::highlightMissedOutgoingCalls();
   const bool missed = QSortFilterProxyModel::data(index,static_cast<int>(Call::Role::Missed)).toBool();
   const Call::Direction dir = qvariant_cast<Call::Direction>(QSortFilterProxyModel::data(index,static_cast<int>(Call::Role::Direction)));

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
}*/

///Constructor
HistoryDock::HistoryDock(QWidget* parent) : QDockWidget(parent),m_pMenu(nullptr),m_pRemove(nullptr),
m_pCallAgain(nullptr)
{
   setObjectName("historyDock");
   QWidget* mainWidget = new QWidget(this);
   setupUi(mainWidget);
   setMinimumSize(250,0);
   setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

   m_pSortByCBB->bindToModel(
      CategorizedHistoryModel::SortedProxy::instance()->categoryModel         (),
      CategorizedHistoryModel::SortedProxy::instance()->categorySelectionModel()
   );

   CategorizedDelegate* delegate = new CategorizedDelegate(m_pView);
   delegate->setChildDelegate(new HistoryDelegate(m_pView));
   m_pView->setDelegate(delegate);
   m_pView->setViewType(CategorizedTreeView::ViewType::History);

   QSortFilterProxyModel* proxy = CategorizedHistoryModel::SortedProxy::instance()->model();

   m_pView->setModel(proxy);
   m_pKeyPressEater = new KeyPressEater(this);
   m_pView->installEventFilter(m_pKeyPressEater);
   m_pView->setSortingEnabled(true);
   m_pView->sortByColumn(0,Qt::DescendingOrder);
   connect(m_pView,SIGNAL(contextMenuRequest(QModelIndex)), this, SLOT(slotContextMenu(QModelIndex)));
   connect(m_pView,SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotDoubleClick(QModelIndex)));
   connect(m_pFilterLE ,SIGNAL(filterStringChanged(QString)), proxy , SLOT(setFilterRegExp(QString)));
   connect(m_pFilterLE ,SIGNAL(textChanged(QString)), this , SLOT(expandTree()));
   connect(proxy,SIGNAL(modelReset()), this , SLOT(expandTree()));
   connect(CategorizedHistoryModel::instance() ,SIGNAL(layoutChanged()), this , SLOT(expandTree())                );
   expandTree();

   m_pSortByCBB->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

   setWidget(mainWidget);

   setWindowTitle(i18nc("History tab","History"));

   m_pSortByCBB->setCurrentIndex(ConfigurationSkeleton::historySortMode());

} //HistoryDock

///Destructor
HistoryDock::~HistoryDock()
{
   delete m_pKeyPressEater;

   if (m_pCallAgain) {
      delete m_pCallAgain    ;
      delete m_pCopy         ;
      delete m_pEmail        ;
      delete m_pRemove       ;
      delete m_pBookmark     ;
   }
} //~HistoryDock


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
   QModelIndex idx = (static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index);
   if (((CategorizedCompositeNode*)idx.internalPointer())->type() != CategorizedCompositeNode::Type::CALL)
      return;
   if (!m_pMenu) {
      m_pCallAgain    = new QAction(this);
      m_pCopy         = new QAction(this);
      m_pEmail        = new QAction(this);
      m_pRemove       = new QAction(this);
      m_pBookmark     = new QAction(this);

      m_pCallAgain->setShortcut    ( Qt::Key_Enter                  );
      m_pCallAgain->setText        ( i18n("Call Again")             );
      m_pCallAgain->setIcon        ( QIcon::fromTheme("call-start")            );

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
      if (!idx.data(static_cast<int>(Call::Role::IsBookmark)).toBool()) {
         m_pBookmark->setText      ( i18n("Bookmark")               );
         m_pBookmark->setIcon      ( QIcon::fromTheme("bookmarks")             );
      }
      else {
         m_pBookmark->setText      ( i18n("Remove bookmark")        );
         m_pBookmark->setIcon      ( QIcon::fromTheme("edit-delete")           );
      }

      m_pMenu = new QMenu(this);
      m_pMenu->addAction( m_pCallAgain    );

      //Allow to add new contacts
      QMenu* subMenu = nullptr;
      for (CollectionInterface* col : PersonModel::instance()->collections(CollectionInterface::SupportedFeatures::ADD | CollectionInterface::SupportedFeatures::MANAGEABLE)) {
         if (!subMenu)
            subMenu = m_pMenu->addMenu(QIcon::fromTheme("contact-new"), i18n("Add new contact"));
         QAction* a = new QAction(this);
         a->setText(col->name());
         a->setIcon(qvariant_cast<QIcon>(col->icon()));
         subMenu->addAction(a);
         connect(a, &QAction::triggered, [this,col]() {slotAddPerson(col);});
      }

      m_pMenu->addAction( m_pCopy         );
      m_pMenu->addAction( m_pEmail        );
      m_pMenu->addAction( m_pRemove       );
      m_pMenu->addAction( m_pBookmark     );
      connect(m_pCallAgain   , SIGNAL(triggered()) , this , SLOT(slotCallAgain())   );
      connect(m_pCopy        , SIGNAL(triggered()) , this , SLOT(slotCopy())        );
      connect(m_pEmail       , SIGNAL(triggered()) , this , SLOT(slotSendEmail())   );
      connect(m_pRemove      , SIGNAL(triggered()) , this , SLOT(slotRemove())      );
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
      CategorizedHistoryModel::instance()->deleteItem(m_pCurrentCall); //TODO add add and remove to the manager
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

void HistoryDock::slotAddPerson(CollectionInterface* col)
{
   qDebug() << "Adding contact";
   QDialog* d = new QDialog();
   DlgProfiles* p = new DlgProfiles(this,m_pCurrentCall->peerName(),m_pCurrentCall->peerContactMethod()->uri());

   QHBoxLayout* l = new QHBoxLayout(d);
   l->addWidget(p);
   connect(p,&DlgProfiles::requestSave,[p,col,this,d]() {
      Person* aPerson = p->create(col);

      aPerson->setContactMethods({PhoneDirectoryModel::instance()->getNumber(m_pCurrentCall->peerContactMethod()->uri(),aPerson,nullptr, "Home")});

      PersonModel::instance()->addNewPerson(aPerson,col);
      d->close();
   });
   connect(p,&DlgProfiles::requestCancel,[col,d]() {
      d->close();
   });

   d->exec();

}

void HistoryDock::slotBookmark()
{
   CategorizedBookmarkModel::instance()->addBookmark(m_pCurrentCall->peerContactMethod());
}

void HistoryDock::slotDoubleClick(const QModelIndex& index)
{
   if (!index.isValid())
      return;
   QModelIndex idx = (static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index);
   if (!idx.isValid() || !idx.parent().isValid())
      return;
   if (((CategorizedCompositeNode*)idx.internalPointer())->type() != CategorizedCompositeNode::Type::CALL)
      return;
   m_pCurrentCall = static_cast<Call*>(static_cast<CategorizedCompositeNode*>(idx.internalPointer())->getSelf());
   enableRemove();
   slotCallAgain();
}

