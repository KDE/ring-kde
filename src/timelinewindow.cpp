/***************************************************************************
 *   Copyright (C) 2015-2017 by Emmanuel Lepage Vallee                     *
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
#include "timelinewindow.h"

// Qt
#include <QtCore/QTimer>
#include <QtCore/QSortFilterProxyModel>

// KDE
#include <klocalizedstring.h>

// Ring
#include "contactmethod.h"
#include "timeline/recentdock.h"
#include "timeline/viewcontactdock.h"
#include "actioncollection.h"
#include "widgets/dockbase.h"
#include "klib/kcfg_settings.h"

// Models
#include <categorizedcontactmodel.h>

// Delegates
#include <conf/account/delegates/categorizeddelegate.h>
#include "delegates/contactdelegate.h"
#include "delegates/phonenumberdelegate.h"

TimelineWindow::TimelineWindow()
{
    setObjectName("TimelineWindow");

    //On OSX, QStandardPaths doesn't work as expected, it is better to pack the .ui in the DMG
#ifdef Q_OS_MAC
   QDir dir(QApplication::applicationDirPath());
   dir.cdUp();
   dir.cd("Resources/kxmlgui5/ring-kde/");
   setXMLFile(dir.path()+"/ring-kdeui.rc");
   setUnifiedTitleAndToolBarOnMac(true);
#endif

    setDockOptions(
        QMainWindow::AnimatedDocks    |
        QMainWindow::VerticalTabs     |
        QMainWindow::AllowNestedDocks |
        QMainWindow::AllowTabbedDocks
    );

    setAutoSaveSettings();

    ActionCollection::instance()->setupAction(this);
    // MainWindow
    m_pViewContact = new ViewContactDock(this);
    m_pViewContact->setObjectName(QStringLiteral("viewContact"));
    m_pViewContact->setWindowTitle(i18nc("View contact tab","View contact"));
    setCentralWidget(m_pViewContact);

    //Timeline dock
    m_pPeersTimeline = new RecentDock(this);
    m_pPeersTimeline->setObjectName(QStringLiteral("timelineDock"));
    m_pPeersTimeline->setWindowTitle(i18nc("Bookmark tab","Timeline"));
    addDockWidget( Qt::LeftDockWidgetArea, m_pPeersTimeline, Qt::Vertical);
    connect(m_pPeersTimeline, &RecentDock::viewContactMethod, this, &TimelineWindow::viewContact);
    connect(m_pPeersTimeline, &QDockWidget::visibilityChanged, this, &FancyMainWindow::updateTabIcons);

    //Contact dock
    m_pContactCD = new DockBase  ( nullptr );
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
    addDockWidget( Qt::LeftDockWidgetArea, m_pContactCD, Qt::Vertical);
    connect(m_pContactCD , &QDockWidget::visibilityChanged, this, &FancyMainWindow::updateTabIcons);
    tabifyDockWidget(m_pPeersTimeline, m_pContactCD );

    createGUI();
    updateTabIcons();
}

TimelineWindow::~TimelineWindow()
{

}

void TimelineWindow::viewContact(ContactMethod* cm)
{
    m_pViewContact->setContactMethod(cm);
}
