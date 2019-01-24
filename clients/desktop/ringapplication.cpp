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
#include "ringapplication.h"

//Qt
#include <QtCore/QDebug>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QQmlComponent>

//KDE
#include <KDeclarative/KDeclarative>

//LRC
#include <itemdataroles.h>
#include <session.h>
#include <accountmodel.h>

//Ring
#include "kcfg_settings.h"
#include "cmd.h"
#include "jamikdeintegration/src/windowevent.h"

KDeclarative::KDeclarative* RingApplication::m_pDeclarative {nullptr};
RingApplication* RingApplication::m_spInstance {nullptr};

/**
 * The application constructor
 */
RingApplication::RingApplication(int & argc, char ** argv) : QApplication(argc,argv),m_StartIconified(false)
{
   Q_ASSERT(argc != -1);

   m_spInstance = this;
}

/**
 * Destructor
 */
RingApplication::~RingApplication()
{
   delete m_pDeclarative;
   delete engine();
   m_spInstance = nullptr;
}

RingApplication* RingApplication::instance()
{
   Q_ASSERT(m_spInstance);
   return m_spInstance;
}

///Parse command line arguments
int RingApplication::newInstance()
{
   static bool displayWizard  = ConfigurationSkeleton::enableWizard() || ConfigurationSkeleton::showSplash();
   const  bool displayOnStart = ConfigurationSkeleton::displayOnStart() && !startIconified();

   static bool init = true;
   //Only call on the first instance
   if (init) {
      init = false;

      initDesktopWindow();
   }

   // The first run wizard
   if (displayOnStart && displayWizard) {
      // Also add this object
      engine()->rootContext()->setContextProperty(
         QStringLiteral("RingApplication"), this
      );

      if (ConfigurationSkeleton::enableWizard())
         WindowEvent::instance()->showWizard();

      if (!Session::instance()->accountModel()->size())
         WindowEvent::instance()->showWizard();

      ConfigurationSkeleton::setEnableWizard(false);
      displayWizard = false;
      return 0;
   }

   return 0;
}

bool RingApplication::startIconified() const
{
   return m_StartIconified;
}

void RingApplication::setIconify(bool iconify)
{
   m_StartIconified = iconify;
}

/// Create a QML engine for various canvas widgets
QQmlApplicationEngine* RingApplication::engine()
{
   static QQmlApplicationEngine* e = nullptr;
   static std::atomic_flag engineInit = ATOMIC_FLAG_INIT;

   if (!engineInit.test_and_set()) {
      e = new QQmlApplicationEngine(QGuiApplication::instance());

      // Setup the icon theme provider and ki18n
      m_pDeclarative = new KDeclarative::KDeclarative;
      m_pDeclarative->setDeclarativeEngine(e);
   }
   return e;
}

void RingApplication::initDesktopWindow()
{
   static QQuickWindow* dw = nullptr;
   if (!dw) {
      QQmlComponent component(engine());
      component.loadUrl(QUrl(QStringLiteral("qrc:/desktopview/qml/desktopwindow.qml")));
      if ( component.isReady() ) {
         qDebug() << "Previous error" << component.errorString();

         auto obj2 = component.create();

         // I have *no* clue why this is needed... A race somewhere
         while (component.errorString().isEmpty() && !obj2)
            obj2 = component.create();

         if (!(dw = qobject_cast<QQuickWindow*>(obj2)))
            qWarning() << "Failed to load:" << component.errorString();
      }
      else
         qWarning() << component.errorString();
   }

   Q_ASSERT(dw);
}

// kate: space-indent on; indent-width 3; replace-tabs on;
