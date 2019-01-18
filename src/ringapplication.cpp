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
#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QQmlComponent>

//KDE
#include <KIconLoader>
#include <kmessagebox.h>
#include <KDeclarative/KDeclarative>
#include <KLocalizedString>

//LRC
#include <itemdataroles.h>
#include <session.h>
#include <accountmodel.h>

//Ring
#include "kcfg_settings.h"
#include "cmd.h"
#include "jamikdeintegration/src/windowevent.h"

//QML
#include "qmlwidgets/plugin.h"
#include "qmlwidgets/symboliccolorizer.h"
#include "desktopview/desktopviewplugin.h"


///Error to display when there is nothing else to say
static const QString GENERIC_ERROR = i18n("An unknown error occurred. Ring KDE will now exit. If the problem persist, please report a bug.\n\n"
      "It is known that this message can be caused by trying to open Ring KDE while the Ring daemon is exiting. If so, waiting 15 seconds and "
      "trying again will solve the issue.");


KDeclarative::KDeclarative* RingApplication::m_pDeclarative {nullptr};
RingQmlWidgets* RingApplication::m_pQmlWidget {nullptr};
DesktopView* RingApplication::m_pDesktopView {nullptr};
RingApplication* RingApplication::m_spInstance {nullptr};

//This code detect if the window is active, innactive or minimzed
class PhoneWindowEvent final : public QObject {
   Q_OBJECT
public:
   PhoneWindowEvent(RingApplication* ev) : QObject(ev),m_pParent(ev) {
      QTimer::singleShot(0, [this]() {
         m_pParent->desktopWindow()->installEventFilter(this);
      });
   }
protected:
   virtual bool eventFilter(QObject *obj, QEvent *event)  override {
      Q_UNUSED(obj)
      if (event->type() == QEvent::WindowDeactivate) {
         m_pParent->m_HasFocus = false;
      }
      else if (event->type() == QEvent::WindowActivate) {
         m_pParent->m_HasFocus = true;
      }
      return false;
   }

private:
   RingApplication* m_pParent;

Q_SIGNALS:
   void minimized(bool);
};

/**
 * The application constructor
 */
RingApplication::RingApplication(int & argc, char ** argv) : QApplication(argc,argv),m_StartIconified(false)
{
   Q_ASSERT(argc != -1);

   setAttribute(Qt::AA_EnableHighDpiScaling);

   m_pEventFilter = new PhoneWindowEvent(this);

   m_spInstance = this;
}

/**
 * Destructor
 */
RingApplication::~RingApplication()
{
   delete m_pDeclarative;
   delete engine();
   delete m_pQmlWidget;
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

      desktopWindow();
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
      m_pQmlWidget = new RingQmlWidgets;
      m_pQmlWidget->registerTypes("RingQmlWidgets");

      m_pDesktopView = new DesktopView;
      m_pDesktopView->registerTypes("DesktopView");

      e = new QQmlApplicationEngine(QGuiApplication::instance());

      // Setup the icon theme provider and ki18n
      m_pDeclarative = new KDeclarative::KDeclarative;
      m_pDeclarative->setDeclarativeEngine(e);

      auto im2 = new SymbolicColorizer();
      e->addImageProvider( QStringLiteral("SymbolicColorizer"), im2 );
   }
   return e;
}

QQuickWindow* RingApplication::desktopWindow() const
{
   static QQuickWindow* dw = nullptr;
   if (!dw) {
      QQmlComponent component(engine());
      component.loadUrl(QUrl(QStringLiteral("qrc:/DesktopWindow.qml")));
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

   return dw;
}

///Exit gracefully
bool RingApplication::notify (QObject* receiver, QEvent* e)
{
   try {
      return QApplication::notify(receiver,e);
   }
   catch (...) {
      Q_ASSERT(false);
      qDebug() << GENERIC_ERROR;
      KMessageBox::error(nullptr, GENERIC_ERROR);
   }
   return false;
}

bool RingApplication::mayHaveFocus()
{
   return m_HasFocus;
}

#include <ringapplication.moc>

// kate: space-indent on; indent-width 3; replace-tabs on;
