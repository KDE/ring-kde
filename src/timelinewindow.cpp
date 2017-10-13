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
#include <QtCore/QDir>
#include <QtCore/QSortFilterProxyModel>
#include <QQmlApplicationEngine>
#include <QQmlContext>

// KDE
#include <klocalizedstring.h>

// Ring
#include "contactmethod.h"
#include "timeline/recentdock.h"
#include "actioncollection.h"
#include "widgets/dockbase.h"
#include "klib/kcfg_settings.h"
#include "ringapplication.h"

// Models
#include <categorizedcontactmodel.h>

// Delegates
#include "delegates/categorizeddelegate.h"
#include "delegates/contactdelegate.h"
#include "delegates/phonenumberdelegate.h"

TimelineWindow::TimelineWindow()
{
    setObjectName(QStringLiteral("TimelineWindow"));

    //On OSX, QStandardPaths doesn't work as expected, it is better to pack the .ui in the DMG
#ifdef Q_OS_MAC
   setUnifiedTitleAndToolBarOnMac(true);
#endif

    setDockOptions(
        QMainWindow::AnimatedDocks    |
        QMainWindow::VerticalTabs     |
        QMainWindow::AllowNestedDocks |
        QMainWindow::AllowTabbedDocks
    );

    setAutoSaveSettings();

    // Load the welcome message

    QFile file(QStringLiteral(":/assets/welcome.html"));
    if (file.open(QIODevice::ReadOnly))
        RingApplication::engine()->rootContext()->setContextProperty(QStringLiteral("welcomeMessage"), file.readAll());

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
    m_pCategoryDelegate->setParent(this);
    m_pContactMethodDelegate->setParent(this);
    m_pContactDelegate->setParent(this);
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

    connect(m_pContactCD , &QDockWidget::visibilityChanged, this, &FancyMainWindow::updateTabIcons);
    tabifyDockWidget(m_pPeersTimeline, m_pContactCD );

    #ifdef Q_OS_MAC
       QDir dir(QApplication::applicationDirPath());
       dir.cdUp();
       dir.cd("Resources/");
      QTimer::singleShot(0, [this, dir]() {createGUI(dir.path()+"/ring-kdeui.rc");});
    #else
      QTimer::singleShot(0, [this]() {createGUI();});
    #endif

    updateTabIcons();

    m_pPeersTimeline->raise();

    //HACK

    // As of Qt 5.9, there is a race condition when killing the client that may
    // cause the dock to be moved to the bottom. Detect it and fix it.
    if (dockWidgetArea(m_pContactCD) == Qt::BottomDockWidgetArea) {
        removeDockWidget(m_pContactCD);
        addDockWidget(Qt::LeftDockWidgetArea, m_pContactCD);
    }

    // There is an HiDPI issue where the size of the dock remains the same
    // (in pixels) even when the ratio is different than 1. Detect and fix
    if (dockWidgetArea(m_pPeersTimeline) == Qt::LeftDockWidgetArea && m_pPeersTimeline->width() < 4.5*logicalDpiX()) {
        resizeDocks({m_pPeersTimeline}, {(int)4.5*logicalDpiX()}, Qt::Horizontal);
    }
}

TimelineWindow::~TimelineWindow()
{
    delete m_pViewContact;
}

void TimelineWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    emit unregisterWindow();
}

void TimelineWindow::viewContact(ContactMethod* cm)
{
    m_pViewContact->setContactMethod(cm);
}


void TimelineWindow::viewPerson(Person* p)
{
    m_pViewContact->setPerson(p);
}

void TimelineWindow::setCurrentPage(ViewContactDock::Pages page)
{
    m_pViewContact->setCurrentPage(page);
}
