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
#include "bookmarkdock.h"

//Qt
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QMenu>
#include <QtGui/QClipboard>
#include <QMimeData>
#include <QInputDialog>

//KDE
#include <klocalizedstring.h>
#include <QIcon>
#include <QAction>

//Ring
#include "klib/kcfg_settings.h"
#include "mainwindow.h"
#include "klib/helperfunctions.h"
#include <categorizedbookmarkmodel.h>
#include "categorizedhistorymodel.h"
#include "contactmethod.h"
#include "mime.h"
#include "personmodel.h"
#include "phonedirectorymodel.h"
#include "numbercategory.h"
#include "accountmodel.h"
#include "availableaccountmodel.h"
#include "callmodel.h"
#include "categorizedtreeview.h"
#include <conf/account/delegates/categorizeddelegate.h>
#include "../delegates/historydelegate.h"
#include "../menu/contactmethod.h"

///Constructor
BookmarkDock::BookmarkDock(QWidget* parent) : QDockWidget(parent), m_pMenu(nullptr)
{
   setObjectName("bookmarkDock");
   QWidget* mainWidget     = new QWidget   ( this );
   m_pMostUsedCK           = new QCheckBox ( this );
   setupUi(mainWidget);
   m_pSortByCBB   ->setHidden(true);
   setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
   setMinimumSize(250,0);

   CategorizedDelegate* delegate = new CategorizedDelegate(m_pView);
   delegate->setChildDelegate(new HistoryDelegate(m_pView));
   m_pView->setDelegate(delegate);
   m_pView->setSortingEnabled(true);
   m_pProxyModel = new BookmarkSortFilterProxyModel(this);
   m_pProxyModel->setSourceModel          ( CategorizedBookmarkModel::instance() );
   m_pProxyModel->setSortRole             ( static_cast<int>(Call::Role::Name  )        );
   m_pProxyModel->setFilterRole           ( static_cast<int>(Call::Role::Filter)        );
   m_pProxyModel->setFilterCaseSensitivity( Qt::CaseInsensitive       );
   m_pProxyModel->setSortCaseSensitivity  ( Qt::CaseInsensitive       );
   m_pProxyModel->setDynamicSortFilter    ( true                      );
   m_pView->setModel(m_pProxyModel);
   m_pProxyModel->sort(0);
   expandTree();

   connect(CategorizedBookmarkModel::instance() ,SIGNAL(layoutChanged()), this , SLOT(expandTree()));
   connect(m_pFilterLE ,SIGNAL(filterStringChanged(QString)), m_pProxyModel , SLOT(setFilterRegExp(QString)));
   connect(m_pFilterLE ,SIGNAL(textChanged(QString)), this , SLOT(expandTree()));

   m_pFilterLE->setPlaceholderText(i18n("Filter"));

   m_pMostUsedCK->setChecked(ConfigurationSkeleton::displayPopularAsBookmark());

   setWidget(mainWidget);

   m_pMostUsedCK->setText(i18n("Show most called contacts"));
   m_pMostUsedCK->setVisible(false); //FIXME

   splitter->setStretchFactor(0,7);

   setWindowTitle(i18nc("Bookmark dock","Bookmark"));

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
   CategorizedBookmarkModel::instance()->reloadCategories();
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
   Call* call2 = CallModel::instance()->dialingCall(idx.model()->data(idx,static_cast<int>(Call::Role::Number)).toString(), AvailableAccountModel::currentDefaultAccount());
   call2->setDialNumber  ( idx.model()->data(idx,static_cast<int>(Call::Role::Number)).toString() );
   call2->performAction( Call::Action::ACCEPT   );
}

void BookmarkDock::slotContextMenu( const QModelIndex& index )
{
   if (!index.parent().isValid())
      return;

   if (!m_pMenu) {
      m_pMenu = new Menu::ContactMethod( this          );
   }
   //TODO Qt5 use lambdas for this
   m_CurrentIndex = m_pProxyModel->mapToSource(index);
   m_pMenu->exec(QCursor::pos());
}
