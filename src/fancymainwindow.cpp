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
#include "fancymainwindow.h"

// Qt
#include <QtCore/QResource>
#include <QtCore/QEvent>
#include <QtCore/QDebug>
#include <QtWidgets/QTabBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QStyleFactory>

// KDE
#include <klocalizedstring.h>

// Ring
#include "ringapplication.h"
#include "phonewindow.h"

FancyMainWindow::FancyMainWindow() : KXmlGuiWindow()
{
    installEventFilter(this);
    updateTabIcons();
}

FancyMainWindow::~FancyMainWindow()
{
    removeEventFilter(this);
    disconnect();
}

bool FancyMainWindow::eventFilter(QObject *obj, QEvent *event)
{
   auto tabbar = qobject_cast<QTabBar*>(obj);
   auto button = m_hEventFilters[tabbar];

   if ((!tabbar) || (!button))
      return KXmlGuiWindow::eventFilter(obj, event);

   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wswitch"
   switch (event->type()) {
      case QEvent::Resize:
      case QEvent::Show:
      case QEvent::Move:
         return KXmlGuiWindow::eventFilter(obj, event);
   }
   #pragma GCC diagnostic pop

   button->resize(tabbar->width(), tabbar->width());
   button->move(tabbar->x(), tabbar->height() - tabbar->width());

   return KXmlGuiWindow::eventFilter(obj, event);
}

///Qt does not support dock icons by default, this is an hack around this
void FancyMainWindow::updateTabIcons()
{
    if (!m_IsActive)
        return;

    // Load the CSS
    static const QResource tss(":/toolbar/toolbar.css");
    static QByteArray css = QByteArray((char*)tss.data(), tss.size());

    // Twist Qt arm to create those "electron" like tabbar on the left
    QList<QTabBar*> tabBars = findChildren<QTabBar*>();
    if(tabBars.count()) {
        foreach(QTabBar* bar, tabBars) {

            // Only do the funky hack if the tab are on the left //TODO support RTL
            const auto pt = bar->mapTo(this, {0,0});
            const bool isVertical = bar->height() > bar->width();
            const bool isMainToolbar = (dockOptions()&QMainWindow::VerticalTabs)
                && pt.x() < 20 && isVertical;

            // Attach an event filter
//             if (isMainToolbar && !m_hEventFilters.contains(bar)) {
//                 bar->installEventFilter(this);
//
//                 auto tb = new QToolButton(bar);
//                 tb->setAutoFillBackground(false);
//                 tb->setText(i18n("Phone"));
//                 tb->setIconSize({48, 48});
//                 tb->setIcon(QIcon::fromTheme(QStringLiteral("call-start")));
//                 tb->setStyleSheet(QStringLiteral(
//                 "background-color: transparent; background: none;"
//                 ));
//
//                 connect(tb, &QToolButton::clicked, this, &FancyMainWindow::showPhone);
//
//                 m_hEventFilters[bar] = tb;
//             }

            if (isMainToolbar)
                bar->setIconSize(QSize(64, 64));
            else
                bar->setIconSize({});

            static auto s = QStyleFactory::create("windows");

            // The idea is that some native styles (like macOS) will draw
            // things differently. In order to keep a consistent look, force
            // the windows theme.
            bar->setStyle((s && isMainToolbar) ? s : nullptr);

            bar->setStyleSheet(isMainToolbar ? css : QString());

            for (int i=0;i<bar->count();i++) {
                const QString text = bar->tabText(i).replace('&',QString());

                if (text == i18n("Call")) {
                    if (isMainToolbar)
                        bar->setTabIcon(i,QIcon(":/toolbar/call.svg"));
                    else
                        bar->setTabIcon(i,QIcon::fromTheme(QStringLiteral("call-start")));
                }
                else if (text == i18n("Bookmark")) {
                    if (isMainToolbar)
                        bar->setTabIcon(i,QIcon(":/toolbar/bookmark.svg"));
                    else
                        bar->setTabIcon(i,QIcon::fromTheme(QStringLiteral("bookmarks")));
                }
                else if (text == i18n("Contact")) {
                    if (isMainToolbar)
                        bar->setTabIcon(i,QIcon(":/toolbar/contact.svg"));
                    else
                        bar->setTabIcon(i,QIcon::fromTheme(QStringLiteral("folder-publicshare")));
                }
                else if (text == i18n("History")) {
                    if (isMainToolbar)
                        bar->setTabIcon(i,QIcon(":/toolbar/history.svg"));
                    else
                        bar->setTabIcon(i,QIcon::fromTheme(QStringLiteral("view-history")));
                }
                else if (text == i18n("Video")) {
                    bar->setTabIcon(i,QIcon::fromTheme(QStringLiteral("camera-on")));
                }
                else if (text == i18n("Timeline")) {
                    bar->setTabIcon(i,QIcon(":/toolbar/timeline.svg"));
                }

                bar->setTabToolTip(i, text);
            }
        }
    }
}

void FancyMainWindow::setActive(bool a)
{
    m_IsActive = a;
}

void FancyMainWindow::showPhone()
{
//     if (RingApplication::instance()->isPhoneVisible()) {
        RingApplication::instance()->phoneWindow()->show ();
        RingApplication::instance()->phoneWindow()->raise();
//     }
//     else {
//
//         timelineWindow()->setCurrentPage();
//     }
}
