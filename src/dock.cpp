/***************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
 *   Copyright (C) 2015 by Emmanuel Lepage Vallee                          *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
#include "dock.h"

//Qt
#include <QtWidgets/QMainWindow>
#include <QtCore/QSortFilterProxyModel>

//Delegates
#include <conf/account/delegates/categorizeddelegate.h>
#include "delegates/contactdelegate.h"
#include "delegates/phonenumberdelegate.h"
#include "delegates/historydelegate.h"

//Widgets
#include "widgets/dockbase.h"

//Models
#include <categorizedcontactmodel.h>
#include <categorizedhistorymodel.h>
#include <categorizedbookmarkmodel.h>

//Ring
#include "mainwindow.h"
#include "actioncollection.h"
#include "klib/kcfg_settings.h"

class BookmarkSortFilterProxyModel : public QSortFilterProxyModel
{
   Q_OBJECT
public:
   explicit BookmarkSortFilterProxyModel(QObject* parent) : QSortFilterProxyModel(parent) {
      setSourceModel          ( &CategorizedBookmarkModel::instance() );
      setSortRole             ( static_cast<int>(Call::Role::Name  )        );
      setFilterRole           ( static_cast<int>(Call::Role::Filter)        );
      setFilterCaseSensitivity( Qt::CaseInsensitive       );
      setSortCaseSensitivity  ( Qt::CaseInsensitive       );
      setDynamicSortFilter    ( true                      );
      sort(0);
   }
protected:
   virtual bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const override
   {
      if (!source_parent.isValid() ) { //Is a category
         for (int i=0;i<CategorizedBookmarkModel::instance().rowCount(CategorizedBookmarkModel::instance().index(source_row,0,source_parent));i++) {
            if (filterAcceptsRow(i, CategorizedBookmarkModel::instance().index(source_row,0,source_parent)))
               return true;
         }
      }

      return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
   }
};

Dock::Dock(QMainWindow* w) : QObject(w)
{
   //Contact dock
   m_pContactCD       = new DockBase  ( w );
   m_pContactCD->setObjectName("contactDock");
   m_pContactCD->setWindowTitle(i18nc("Contact tab","Contact"));
   auto m_pCategoryDelegate = new CategorizedDelegate(m_pContactCD->view());
   auto m_pContactMethodDelegate = new ContactMethodDelegate();
   auto m_pContactDelegate = new ContactDelegate(m_pContactCD->view());
   m_pContactMethodDelegate->setView(m_pContactCD->view());
   m_pContactDelegate->setChildDelegate(m_pContactMethodDelegate);
   m_pCategoryDelegate->setChildDelegate(m_pContactDelegate);
   m_pCategoryDelegate->setChildChildDelegate(m_pContactMethodDelegate);
   m_pContactCD->setDelegate(m_pCategoryDelegate);

   CategorizedContactModel::instance().setUnreachableHidden(ConfigurationSkeleton::hidePersonWithoutPhone());
   QSortFilterProxyModel* proxy = CategorizedContactModel::SortedProxy::instance().model();
   m_pContactCD->setProxyModel(proxy);
   m_pContactCD->setSortingModel(
      CategorizedContactModel::SortedProxy::instance().categoryModel(),
      CategorizedContactModel::SortedProxy::instance().categorySelectionModel()
   );

   CategorizedContactModel::SortedProxy::instance().categorySelectionModel()->setCurrentIndex(
      CategorizedContactModel::SortedProxy::instance().categoryModel()->index(
         ConfigurationSkeleton::contactSortMode() , 0
      ), QItemSelectionModel::ClearAndSelect
   );

   connect(CategorizedContactModel::SortedProxy::instance().categorySelectionModel(), & QItemSelectionModel::currentChanged,[](const QModelIndex& idx) {
      if (idx.isValid())
         ConfigurationSkeleton::setContactSortMode(idx.row());
   });

   //History dock
   m_pHistoryDW       = new DockBase  ( w );
   m_pHistoryDW->setObjectName("historyDock");
   m_pHistoryDW->setWindowTitle(i18nc("History tab","History"));
   CategorizedDelegate* delegate = new CategorizedDelegate(m_pHistoryDW->view());
   delegate->setChildDelegate(new HistoryDelegate(m_pHistoryDW->view()));
   m_pHistoryDW->setDelegate(delegate);
   proxy = CategorizedHistoryModel::SortedProxy::instance().model();
   m_pHistoryDW->setProxyModel(proxy);
   m_pHistoryDW->setSortingModel(
      CategorizedHistoryModel::SortedProxy::instance().categoryModel         (),
      CategorizedHistoryModel::SortedProxy::instance().categorySelectionModel()
   );

   CategorizedHistoryModel::SortedProxy::instance().categorySelectionModel()->setCurrentIndex(
      CategorizedHistoryModel::SortedProxy::instance().categoryModel()->index(
         ConfigurationSkeleton::historySortMode() , 0
      ), QItemSelectionModel::ClearAndSelect
   );

   connect(CategorizedHistoryModel::SortedProxy::instance().categorySelectionModel(), & QItemSelectionModel::currentChanged,[](const QModelIndex& idx) {
      if (idx.isValid())
         ConfigurationSkeleton::setHistorySortMode(idx.row());
   });

   //Bookmark dock
   m_pBookmarkDW      = new DockBase ( w );
   m_pBookmarkDW->setObjectName("bookmarkDock");
   m_pBookmarkDW->setWindowTitle(i18nc("Bookmark tab","Bookmark"));
   CategorizedDelegate* delegate2 = new CategorizedDelegate(m_pBookmarkDW->view());
   delegate2->setChildDelegate(new HistoryDelegate(m_pHistoryDW->view()));
   m_pBookmarkDW->setDelegate(delegate2);
   m_pBookmarkDW->setProxyModel(new BookmarkSortFilterProxyModel(this));


   //GUI
   w->addDockWidget( Qt::BottomDockWidgetArea, m_pContactCD  ,Qt::Horizontal);
   w->addDockWidget( Qt::BottomDockWidgetArea, m_pHistoryDW  ,Qt::Horizontal);
   w->addDockWidget( Qt::BottomDockWidgetArea, m_pBookmarkDW ,Qt::Horizontal);

   w->tabifyDockWidget(m_pBookmarkDW,m_pHistoryDW );
   w->tabifyDockWidget(m_pBookmarkDW,m_pContactCD );

   //Force the dock widget aspect ratio, doing this is an HACK
   m_pHistoryDW ->setMinimumSize(350,0);
   m_pContactCD ->setMinimumSize(350,0);
   m_pBookmarkDW->setMinimumSize(350,0);

   m_pHistoryDW ->setMaximumSize(350,999999);
   m_pContactCD ->setMaximumSize(350,999999);
   m_pBookmarkDW->setMaximumSize(350,999999);

   connect(m_pContactCD ,SIGNAL(visibilityChanged(bool)),this,SLOT(updateTabIcons()));
   connect(m_pHistoryDW ,SIGNAL(visibilityChanged(bool)),this,SLOT(updateTabIcons()));
   connect(m_pBookmarkDW,SIGNAL(visibilityChanged(bool)),this,SLOT(updateTabIcons()));

   m_pContactCD-> setVisible(ConfigurationSkeleton::displayContactDock() );
   m_pHistoryDW-> setVisible(ConfigurationSkeleton::displayHistoryDock() );
   m_pBookmarkDW->setVisible(ConfigurationSkeleton::displayBookmarkDock());

   connect(ActionCollection::instance()->showContactDockAction(), SIGNAL(toggled(bool)),m_pContactCD, SLOT(setVisible(bool)));
   connect(ActionCollection::instance()->showHistoryDockAction(), SIGNAL(toggled(bool)),m_pHistoryDW, SLOT(setVisible(bool)));
   connect(ActionCollection::instance()->showBookmarkDockAction(),SIGNAL(toggled(bool)),m_pBookmarkDW,SLOT(setVisible(bool)));

}

Dock::~Dock()
{
   m_pContactCD ->setDelegate  (nullptr);
   m_pHistoryDW ->setDelegate  (nullptr);
   m_pBookmarkDW->setDelegate  (nullptr);
   m_pContactCD ->setProxyModel(nullptr);
   m_pHistoryDW ->setProxyModel(nullptr);
   m_pBookmarkDW->setProxyModel(nullptr);

   if (!MainWindow::app()->isHidden()) {
      ConfigurationSkeleton::setDisplayContactDock ( m_pContactCD->isVisible()  );
      ConfigurationSkeleton::setDisplayHistoryDock ( m_pHistoryDW->isVisible()  );
      ConfigurationSkeleton::setDisplayBookmarkDock( m_pBookmarkDW->isVisible() );
   }
}

///Return the contact dock
DockBase* Dock::contactDock()
{
   return m_pContactCD;
}

///Return the history dock
DockBase* Dock::historyDock()
{
   return m_pHistoryDW;
}

///Return the bookmark dock
DockBase* Dock::bookmarkDock()
{
   return m_pBookmarkDW;
}

///Qt does not support dock icons by default, this is an hack around this
void Dock::updateTabIcons()
{
   QList<QTabBar*> tabBars = parent()->findChildren<QTabBar*>();
   if(tabBars.count())
   {
      foreach(QTabBar* bar, tabBars) {
         for (int i=0;i<bar->count();i++) {
            QString text = bar->tabText(i);
            if (text == i18n("Call")) {
               bar->setTabIcon(i,QIcon::fromTheme("call-start"));
            }
            else if (text == i18n("Bookmark")) {
               bar->setTabIcon(i,QIcon::fromTheme("bookmarks"));
            }
            else if (text == i18n("Contact")) {
               bar->setTabIcon(i,QIcon::fromTheme("folder-publicshare"));
            }
            else if (text == i18n("History")) {
               bar->setTabIcon(i,QIcon::fromTheme("view-history"));
            }
         }
      }
   }
} //updateTabIcons

#include <dock.moc>
