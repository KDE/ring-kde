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
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMenu>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>

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
#include "widgets/historytreeitem.h"
#include "widgets/bookmarkdock.h"
#include "klib/akonadibackend.h"
#include "klib/configurationskeleton.h"
#include "lib/historymodel.h"
#include "../delegates/categorizeddelegate.h"
#include "../delegates/historydelegate.h"

//SFLPhone library
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
HistoryDock::HistoryDock(QWidget* parent) : QDockWidget(parent),m_LastNewCall(0),m_pMenu(nullptr)
{
   setObjectName("historyDock");
   setMinimumSize(250,0);
   setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
   m_pFilterLE   = new KLineEdit   (                        );
   m_pSortByCBB  = new KComboBox   (                        );
   m_pSortByL    = new QLabel      ( i18n("Sort by:")       );
   m_pFromL      = new QLabel      ( i18n("From:")          );
   m_pToL        = new QLabel      ( i18nc("To date:","To:"));
   m_pFromDW     = new KDateWidget (                        );
   m_pToDW       = new KDateWidget (                        );
   m_pAllTimeCB  = new QCheckBox   ( i18n("Display all")    );
   m_pLinkPB     = new QPushButton ( this                   );
   
   m_pView = new CategorizedTreeView(this);
   SortedTreeDelegate* delegate = new SortedTreeDelegate(m_pView);
   delegate->setChildDelegate(new HistoryDelegate(m_pView));
   m_pView->setDelegate(delegate);
   m_pView->setModel(HistoryModel::self());
   m_pKeyPressEater = new KeyPressEater(this);
   m_pView->installEventFilter(m_pKeyPressEater);
   connect(m_pView,SIGNAL(contextMenuRequest(QModelIndex)), this, SLOT(slotContextMenu(QModelIndex)));
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

   QWidget* mainWidget = new QWidget(this);
   setWidget(mainWidget);

   QGridLayout* mainLayout = new QGridLayout(mainWidget);

   mainLayout->addWidget(m_pSortByL   ,0,0     );
   mainLayout->addWidget(m_pSortByCBB ,0,1,1,2 );
   mainLayout->addWidget(m_pAllTimeCB ,1,0,1,3 );
   mainLayout->addWidget(m_pLinkPB    ,3,2,3,1 );
   mainLayout->addWidget(m_pFromL     ,2,0,1,2 );
   mainLayout->addWidget(m_pFromDW    ,3,0,1,2 );
   mainLayout->addWidget(m_pToL       ,4,0,1,2 );
   mainLayout->addWidget(m_pToDW      ,5,0,1,2 );
   mainLayout->addWidget(m_pView      ,6,0,1,3 );
   mainLayout->addWidget(m_pFilterLE  ,8,0,1,3 );

   setWindowTitle(i18n("History"));

   QDate date(2000,1,1);
   m_pFromDW->setDate(date);

   m_CurrentFromDate = m_pFromDW->date();
   m_CurrentToDate   = m_pToDW->date();
   
   m_pSortByCBB->setCurrentIndex(ConfigurationSkeleton::historySortMode());

   connect(m_pAllTimeCB,                   SIGNAL(toggled(bool)),            this, SLOT(enableDateRange(bool))      );
   connect(m_pFilterLE,                    SIGNAL(textChanged(QString)),     this, SLOT(filter(QString))            );
   connect(m_pFromDW  ,                    SIGNAL(changed(QDate)),           this, SLOT(updateLinkedFromDate(QDate)));
   connect(m_pToDW    ,                    SIGNAL(changed(QDate)),           this, SLOT(updateLinkedToDate(QDate))  );
   connect(m_pSortByCBB,                   SIGNAL(currentIndexChanged(int)), this, SLOT(reload())                   );
   connect(AkonadiBackend::getInstance(),  SIGNAL(collectionChanged()),      this, SLOT(updateContactInfo())        );
   connect(HistoryModel::self()         ,  SIGNAL(newHistoryCall(Call*)),    this, SLOT(newHistoryCall(Call*))      );

} //HistoryDock

///Destructor
HistoryDock::~HistoryDock()
{
   foreach (HistoryTreeItem* w, m_History) {
      delete w;
   }
   m_History.clear();
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
   m_pFromL->setVisible (!disable);
   m_pToL->setVisible   (!disable);
   m_pFromDW->setVisible(!disable);
   m_pToDW->setVisible  (!disable);
   m_pLinkPB->setVisible(!disable);

   ConfigurationSkeleton::setDisplayDataRange(!disable);
}

///Filter the history
void HistoryDock::filter(QString text)
{
//    QString lower = text.toLower();
//    foreach(HistoryTreeItem* item, m_History) {
//       bool visible = ( HelperFunctions::normStrippped( item->getName()        ).indexOf( lower ) != -1)
//                   || ( HelperFunctions::normStrippped( item->getPhoneNumber() ).indexOf( lower ) != -1);
//       item->getItem()-> setHidden(!visible);
//    }
//    m_pItemView->expandAll();
}

///When the data range is linked, change the opposite value when editing the first

///The signals have to be disabled to prevent an ifinite loop
void HistoryDock::updateLinkedFromDate(QDate date)
{
//    disconnect (m_pToDW  ,  SIGNAL(changed(QDate)),       this, SLOT(updateLinkedToDate(QDate)));
//    updateLinkedDate(m_pToDW,m_CurrentFromDate,date);
//    connect    (m_pToDW  ,  SIGNAL(changed(QDate)),       this, SLOT(updateLinkedToDate(QDate)));
}

///The signals have to be disabled to prevent an ifinite loop
void HistoryDock::updateLinkedToDate(QDate date)
{
//    disconnect(m_pFromDW  ,  SIGNAL(changed(QDate)),       this, SLOT(updateLinkedFromDate(QDate)));
//    updateLinkedDate(m_pFromDW,m_CurrentToDate,date);
//    connect   (m_pFromDW  ,  SIGNAL(changed(QDate)),       this, SLOT(updateLinkedFromDate(QDate)));
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
//    else if ((key == Qt::Key_Return || key == Qt::Key_Enter) && m_pItemView->selectedItems().size() > 0) {
//       if (m_pItemView->selectedItems()[0] && m_pItemView->itemWidget(m_pItemView->selectedItems()[0],0)) {
//          QNumericTreeWidgetItem* item = dynamic_cast<QNumericTreeWidgetItem*>(m_pItemView->selectedItems()[0]);
//          if (item) {
//             SFLPhone::model()->addDialingCall(item->widget->getName(), AccountList::getCurrentAccount())->setCallNumber(item->widget->getPhoneNumber());
//          }
//       }
//    }
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
   if (static_cast<HistoryTreeBackend*>(index.internalPointer())->type3() != HistoryTreeBackend::Type::CALL)
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
//       if (!m_IsBookmark) {
         m_pBookmark->setText      ( i18n("Bookmark")               );
         m_pBookmark->setIcon      ( KIcon("bookmarks")             );
//       }
//       else {
//          m_pBookmark->setText      ( i18n("Remove bookmark")        );
//          m_pBookmark->setIcon      ( KIcon("edit-delete")           );
//       }
      
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
   m_pCurrentCall = (Call*)static_cast<HistoryTreeBackend*>(index.internalPointer())->getSelf();
   m_pMenu->exec(QCursor::pos());
}

void HistoryDock::slotSendEmail()
{
   if (!m_pCurrentCall) return;
   kDebug() << "Sending email";
   QProcess *myProcess = new QProcess(this);
   QStringList arguments;
   Contact* ct = m_pCurrentCall->getContact();
   if (ct)
      myProcess->start("xdg-email", (arguments << ct->getPreferredEmail()));
}

void HistoryDock::slotCallAgain()
{
   if (!m_pCurrentCall) return;
   kDebug() << "Calling "<< m_pCurrentCall->getPeerPhoneNumber();
   Call* call = SFLPhone::model()->addDialingCall(m_pCurrentCall->getPeerName(), AccountList::getCurrentAccount());
   if (call) {
      call->setCallNumber  ( m_pCurrentCall->getPeerPhoneNumber() );
      call->setPeerName    ( m_pCurrentCall->getPeerName() );
      call->actionPerformed( CALL_ACTION_ACCEPT   );
   }
   else {
      HelperFunctions::displayNoAccountMessageBox(this);
   }
}

void HistoryDock::slotCopy()
{
   kDebug() << "Copying contact";
   QMimeData* mimeData = new QMimeData();
   if (m_pCurrentCall)
      mimeData->setData(MIME_CALLID, m_pCurrentCall->getCallId().toUtf8());

   mimeData->setData(MIME_PHONENUMBER, m_pCurrentCall->getPeerPhoneNumber().toUtf8());
   
   QString numbers;
   QString numbersHtml;
   Contact* ct = m_pCurrentCall->getContact();
   if (ct) {
      numbers     = ct->getFormattedName()+": "+m_pCurrentCall->getPeerPhoneNumber();
      numbersHtml = "<b>"+ct->getFormattedName()+"</b><br />"+HelperFunctions::escapeHtmlEntities(m_pCurrentCall->getPeerPhoneNumber());
   }
   else if (m_pCurrentCall) {
      numbers     = m_pCurrentCall->getPeerName()+": "+m_pCurrentCall->getPeerPhoneNumber();
      numbersHtml = "<b>"+m_pCurrentCall->getPeerName()+"</b><br />"+HelperFunctions::escapeHtmlEntities(m_pCurrentCall->getPeerPhoneNumber());
   }
   mimeData->setData("text/plain", numbers.toUtf8()    );
   mimeData->setData("text/html",  numbersHtml.toUtf8());
   QClipboard* clipboard = QApplication::clipboard();
   clipboard->setMimeData(mimeData);
}

void HistoryDock::slotAaddContact()
{
   kDebug() << "Adding contact";
   Contact* aContact = new Contact();
   Contact::PhoneNumbers numbers(aContact);
   numbers << new Contact::PhoneNumber(m_pCurrentCall->getPeerPhoneNumber(), "Home");
   aContact->setPhoneNumbers(numbers);
   aContact->setFormattedName(m_pCurrentCall->getPeerName());
   AkonadiBackend::getInstance()->addNewContact(aContact);
}

void HistoryDock::slotAddToContact()
{
   //TODO
   kDebug() << "Adding to contact";
}

void HistoryDock::slotBookmark()
{
//    if (!m_IsBookmark)
      SFLPhone::app()->bookmarkDock()->addBookmark(m_pCurrentCall->getPeerPhoneNumber());
//    else
//       SFLPhone::app()->bookmarkDock()->removeBookmark(m_PhoneNumber);
}
