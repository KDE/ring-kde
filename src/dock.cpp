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
#include <QtCore/QTimer>
#include <QtCore/QResource>

//KDE
#include <KColorScheme>

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
#include "phonewindow.h"
#include "view.h"
#include "actioncollection.h"
#include "klib/kcfg_settings.h"
#include <proxies/deduplicateproxy.h>
#include <proxies/roletransformationproxy.h>

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

Dock::Dock(PhoneWindow* w) : QObject(w)
{
   //Contact dock
   m_pContactCD       = new DockBase  ( nullptr );
   m_pContactCD->setObjectName(QStringLiteral("contactDock"));
   m_pContactCD->setWindowTitle(i18nc("Contact tab","Contact"));
   auto m_pCategoryDelegate = new CategorizedDelegate(m_pContactCD->view());
   auto m_pContactMethodDelegate = new ContactMethodDelegate();
   auto m_pContactDelegate = new ContactDelegate(m_pContactCD->view());
   m_pContactMethodDelegate->setView(m_pContactCD->view());
   m_pContactDelegate->setChildDelegate(m_pContactMethodDelegate);
   m_pCategoryDelegate->setChildDelegate(m_pContactDelegate);
   m_pCategoryDelegate->setChildChildDelegate(m_pContactMethodDelegate);
   m_pContactCD->setDelegate(m_pCategoryDelegate);

   // Load later to speed up the process (avoid showing while inserting items)
   QTimer::singleShot(10, [this]() {
      CategorizedContactModel::instance().setUnreachableHidden(ConfigurationSkeleton::hideUnreachable());
      auto proxy = CategorizedContactModel::SortedProxy::instance().model();
      m_pContactCD->setProxyModel(proxy, proxy);
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
   });

   //History dock
   m_pHistoryDW       = new DockBase  ( nullptr );
   m_pHistoryDW->setObjectName(QStringLiteral("historyDock"));
   m_pHistoryDW->setWindowTitle(i18nc("History tab","History"));
   m_pHCategoryDelegate = new CategorizedDelegate(m_pHistoryDW->view());
   m_pHCategoryDelegate->setChildDelegate(new HistoryDelegate(m_pHistoryDW->view()));
   m_pHistoryDW->setDelegate(m_pHCategoryDelegate);


   QTimer::singleShot(1000, [this]() {
      // De-duplicate by name and date
      auto proxy = CategorizedHistoryModel::SortedProxy::instance().model();
      RoleTransformationProxy* highlight = nullptr;
      auto dedup =  ConfigurationSkeleton::mergeSameDayPeer() ? new DeduplicateProxy(proxy) : nullptr;

      if (dedup)
         dedup->addFilterRole(static_cast<int>(Call::Role::DateOnly));

      // Highlight missed calls
      static const bool highlightMissedIn  = ConfigurationSkeleton::highlightMissedIncomingCalls();
      static const bool highlightMissedOut = ConfigurationSkeleton::highlightMissedOutgoingCalls();

      if (highlightMissedOut || highlightMissedIn) {
         static QColor awayBrush = KStatefulBrush( KColorScheme::Window, KColorScheme::NegativeText ).brush(QPalette::Normal).color();
         awayBrush.setAlpha(30);
         static QVariant missedBg(awayBrush);

         highlight = new RoleTransformationProxy(dedup ? dedup : proxy);

         highlight->setRole(Qt::BackgroundRole, [](const QModelIndex& idx) {
            if (idx.data((int)Call::Role::Missed).toBool()) {
               const Call::Direction d = qvariant_cast<Call::Direction>(
                  idx.data((int)Call::Role::Direction)
               );

               if ((highlightMissedIn && d == Call::Direction::INCOMING)
                 || (highlightMissedOut && d == Call::Direction::OUTGOING))
                  return missedBg;
            }

            return QVariant();
         });

         highlight->setSourceModel(proxy);

         if (dedup)
            dedup->setSourceModel(highlight);
      }
      else if (dedup)
         dedup->setSourceModel(proxy);

      if (dedup)
         m_pHistoryDW->setProxyModel(dedup    , proxy );
      else if (highlight)
         m_pHistoryDW->setProxyModel(highlight, proxy );
      else
         m_pHistoryDW->setProxyModel(proxy    , proxy );

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
   });

   //Bookmark dock
   m_pBookmarkDW      = new DockBase ( nullptr );
   m_pBookmarkDW->setObjectName(QStringLiteral("bookmarkDock"));
   m_pBookmarkDW->setWindowTitle(i18nc("Bookmark tab","Bookmark"));
   m_pCategoryDelegate = new CategorizedDelegate(m_pBookmarkDW->view());
   m_pCategoryDelegate->setChildDelegate(new HistoryDelegate(m_pHistoryDW->view()));
   m_pBookmarkDW->setDelegate(m_pCategoryDelegate);
   auto m = new BookmarkSortFilterProxyModel(this);
   m_pBookmarkDW->setProxyModel(m, m);

   m_pContactCD-> setVisible(ConfigurationSkeleton::displayContactDock() );
   m_pHistoryDW-> setVisible(ConfigurationSkeleton::displayHistoryDock() );
   m_pBookmarkDW->setVisible(ConfigurationSkeleton::displayBookmarkDock());

   connect(ActionCollection::instance()->showContactDockAction(), &QAction::toggled,m_pContactCD, &QWidget::setVisible);
   connect(ActionCollection::instance()->showHistoryDockAction(), &QAction::toggled,m_pHistoryDW, &QWidget::setVisible);
   connect(ActionCollection::instance()->showBookmarkDockAction(),&QAction::toggled,m_pBookmarkDW,&QWidget::setVisible);

   connect( ActionCollection::instance()->focusHistory (), &QAction::triggered, this, &Dock::focusHistory  );
   connect( ActionCollection::instance()->focusContact (), &QAction::triggered, this, &Dock::focusContact  );
   connect( ActionCollection::instance()->focusCall    (), &QAction::triggered, this, &Dock::focusCall     );
   connect( ActionCollection::instance()->focusBookmark(), &QAction::triggered, this, &Dock::focusBookmark );

   // Show the "pretty" sidebars
   connect(m_pContactCD , &QDockWidget::visibilityChanged, w, &FancyMainWindow::updateTabIcons);
   connect(m_pHistoryDW , &QDockWidget::visibilityChanged, w, &FancyMainWindow::updateTabIcons);
   connect(m_pBookmarkDW, &QDockWidget::visibilityChanged, w, &FancyMainWindow::updateTabIcons);
   w->updateTabIcons();
}

Dock::~Dock()
{
   m_pContactCD ->setDelegate  (nullptr);
   m_pHistoryDW ->setDelegate  (nullptr);
   m_pBookmarkDW->setDelegate  (nullptr);

   m_pContactCD ->setProxyModel(nullptr, nullptr);
   m_pHistoryDW ->setProxyModel(nullptr, nullptr);
   m_pBookmarkDW->setProxyModel(nullptr, nullptr);

   m_pContactCD ->deleteLater();
   m_pHistoryDW ->deleteLater();
   m_pBookmarkDW->deleteLater();

   if (!PhoneWindow::app()->isHidden()) {
      ConfigurationSkeleton::setDisplayContactDock ( m_pContactCD->isVisible()  );
      ConfigurationSkeleton::setDisplayHistoryDock ( m_pHistoryDW->isVisible()  );
      ConfigurationSkeleton::setDisplayBookmarkDock( m_pBookmarkDW->isVisible() );
   }

   delete m_pCategoryDelegate;
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

void Dock::focusHistory()
{
   m_pHistoryDW->raise();
   ActionCollection::instance()->raiseClient(false);
   m_pHistoryDW->m_pFilterLE->setFocus(Qt::OtherFocusReason);
}

void Dock::focusContact()
{
   m_pContactCD->raise();
   ActionCollection::instance()->raiseClient(false);
   m_pContactCD->m_pFilterLE->setFocus(Qt::OtherFocusReason);
}

void Dock::focusCall()
{
   PhoneWindow::app()->view()->raise();
   ActionCollection::instance()->raiseClient(true);
}

void Dock::focusBookmark()
{
   m_pBookmarkDW->raise();
   ActionCollection::instance()->raiseClient(false);
   m_pBookmarkDW->m_pFilterLE->setFocus(Qt::OtherFocusReason);
}

#include <dock.moc>

// kate: space-indent on; indent-width 3; replace-tabs on;
