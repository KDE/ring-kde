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
#include "bookmarkdock.h"

//Qt
#include <QtGui/QVBoxLayout>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QTreeWidget>
#include <QtGui/QSplitter>
#include <QtGui/QCheckBox>
#include <QtGui/QMenu>
#include <QStandardItemModel>

//KDE
#include <KLocalizedString>
#include <KIcon>
#include <KLineEdit>
#include <KLocale>
#include <KAction>

//SFLPhone
#include "klib/kcfg_settings.h"
#include "sflphone.h"
#include "klib/helperfunctions.h"
#include "klib/bookmarkmodel.h"
#include "lib/historymodel.h"
#include "lib/accountlistmodel.h"
#include "lib/callmodel.h"
#include "categorizedtreeview.h"
#include "../delegates/categorizeddelegate.h"
#include "../delegates/historydelegate.h"

///Constructor
BookmarkDock::BookmarkDock(QWidget* parent) : QDockWidget(parent),m_pMenu(nullptr),
   m_pCallAgain(nullptr)
{
   setObjectName("bookmarkDock");
   QWidget* mainWidget     = new QWidget   ( this );
   m_pMostUsedCK           = new QCheckBox ( this );
   setupUi(mainWidget);
   m_pBottomWidget->setHidden(true);
   m_pSortByCBB   ->setHidden(true);
   m_pSortByL     ->setHidden(true);
   setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
   setMinimumSize(250,0);

   CategorizedDelegate* delegate = new CategorizedDelegate(m_pView);
   delegate->setChildDelegate(new HistoryDelegate(m_pView));
   m_pView->setDelegate(delegate);
   m_pProxyModel = new BookmarkSortFilterProxyModel(this);
   m_pProxyModel->setSourceModel          ( BookmarkModel::instance() );
   m_pProxyModel->setSortRole             ( Qt::DisplayRole           );
   m_pProxyModel->setFilterRole           ( Call::Role::Filter        );
   m_pProxyModel->setFilterCaseSensitivity( Qt::CaseInsensitive       );
   m_pView->setModel(m_pProxyModel);
   expandTree();

   connect(BookmarkModel::instance() ,SIGNAL(layoutChanged()), this , SLOT(expandTree()));
   connect(m_pFilterLE ,SIGNAL(filterStringChanged(QString)), m_pProxyModel , SLOT(setFilterRegExp(QString)));
   connect(m_pFilterLE ,SIGNAL(textChanged(QString)), this , SLOT(expandTree()));

   m_pFilterLE->setPlaceholderText(i18n("Filter"));

   m_pMostUsedCK->setChecked(ConfigurationSkeleton::displayPopularAsBookmark());

   setWidget(mainWidget);

   m_pMostUsedCK->setText(i18n("Show most called contacts"));

   m_pTopWidget->layout()->addWidget ( m_pMostUsedCK );

   splitter->setStretchFactor(0,7);

   setWindowTitle(i18n("Bookmark"));

//    connect(m_pFilterLE                    , SIGNAL(textChanged(QString))            , this , SLOT(filter(QString))             );
   connect(m_pMostUsedCK                  , SIGNAL(toggled(bool))                   , this , SLOT(reload())                    );
   connect(m_pView                        , SIGNAL(doubleClicked(QModelIndex))      , this , SLOT(slotDoubleClick(QModelIndex)));
   connect(m_pView                        , SIGNAL(contextMenuRequest(QModelIndex)) , this , SLOT(slotContextMenu(QModelIndex)));
   reload();
} //BookmarkDock

///Destructor
BookmarkDock::~BookmarkDock()
{
   delete m_pMostUsedCK;
   delete m_pProxyModel;
   if (m_pMenu) delete m_pMenu;
}


/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/

///Show the most popular items
void BookmarkDock::reload()
{
   ConfigurationSkeleton::setDisplayPopularAsBookmark(m_pMostUsedCK->isChecked());
   BookmarkModel::instance()->reloadCategories();
} //reload

///Expand the tree according to the user preferences
void BookmarkDock::expandTree()
{
   m_pView->expandToDepth(0);
}

void BookmarkDock::slotDoubleClick(const QModelIndex& index)
{
   QModelIndex idx = (static_cast<const BookmarkSortFilterProxyModel*>(index.model()))->mapToSource(index);
   if (!idx.isValid() || !idx.parent().isValid())
      return;
   if (((CategorizedCompositeNode*)idx.internalPointer())->type() != CategorizedCompositeNode::Type::BOOKMARK)
      return;
   Call* call2 = CallModel::instance()->addDialingCall(idx.model()->data(idx,Call::Role::Number).toString(), AccountListModel::currentAccount());
   call2->setDialNumber  ( idx.model()->data(idx,Call::Role::Number).toString() );
   call2->performAction( Call::Action::ACCEPT   );
}

void BookmarkDock::slotContextMenu( const QModelIndex& index )
{
   if (!index.parent().isValid())
      return;
   if (!m_pCallAgain) {
      m_pCallAgain   = new KAction(this);
      m_pCallAgain->setText       ( i18n("Call Again")         );
      m_pCallAgain->setIcon       ( KIcon("call-start")        );

      m_pEditContact = new KAction(this);
      m_pEditContact->setText     ( i18n("Edit contact")       );
      m_pEditContact->setIcon     ( KIcon("contact-new")       );

      m_pCopy        = new KAction(this);
      m_pCopy->setText            ( i18n("Copy")               );
      m_pCopy->setIcon            ( KIcon("edit-copy")         );

      m_pEmail       = new KAction(this);
      m_pEmail->setText           ( i18n("Send Email")         );
      m_pEmail->setIcon           ( KIcon("mail-message-new")  );
      m_pEmail->setEnabled        ( false                      );

      m_pAddPhone    = new KAction(this);
      m_pAddPhone->setText        ( i18n("Add Phone Number")   );
      m_pAddPhone->setIcon        ( KIcon("list-resource-add") );
      m_pEmail->setEnabled        ( false                      );

      m_pBookmark    = new KAction(this);
      m_pBookmark->setText        ( i18n("Remove Bookmark")    );
      m_pBookmark->setIcon        ( KIcon("list-remove")       );

      connect(m_pCallAgain   , SIGNAL(triggered()) , this,SLOT(callAgain())  );
      connect(m_pEditContact , SIGNAL(triggered()) , this,SLOT(editContact()));
      connect(m_pCopy        , SIGNAL(triggered()) , this,SLOT(copy())       );
      connect(m_pEmail       , SIGNAL(triggered()) , this,SLOT(sendEmail())  );
      connect(m_pAddPhone    , SIGNAL(triggered()) , this,SLOT(addPhone())   );
      connect(m_pBookmark    , SIGNAL(triggered()) , this,SLOT(bookmark())   );
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
   m_CurrentIndex = m_pProxyModel->mapToSource(index);
   m_pMenu->exec(QCursor::pos());
}

void BookmarkDock::bookmark()
{
   if (m_CurrentIndex.isValid()) {
      BookmarkModel::instance()->remove(m_CurrentIndex);
      m_CurrentIndex = QModelIndex();
      expandTree();
   }
}
