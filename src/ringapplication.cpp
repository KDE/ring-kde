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
#include <QtCore/QStandardPaths>
#include <QtCore/QItemSelectionModel>
#include <QtWidgets/QAction>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QQmlExtensionPlugin>
#include <QQmlComponent>

//KDE
#include <KIconLoader>
#include <KActionCollection>
#include <kmessagebox.h>
#include <KDeclarative/KDeclarative>

//LRC
#include <itemdataroles.h>
#include <session.h>
#include <callmodel.h>
#include <accountmodel.h>
#include <account.h>
#include <bookmarkmodel.h>
#include <libcard/historyimporter.h>

//Ring
#include "kcfg_settings.h"
#include "cmd.h"
#include "errormessage.h"
#include "wizard/welcome.h"
#include "jamikdeintegration/src/windowevent.h"
#include "callview/videowidget.h"

//Models
#include <profilemodel.h>
#include <callhistorymodel.h>
#include <certificatemodel.h>
#include <numbercategorymodel.h>
#include <persondirectory.h>

//Delegates
#include "extensions/presencecollectionextension.h"

//QML
#include <KQuickItemViews/plugin.h>
#include "qmlwidgets/plugin.h"
#include "qmlwidgets/symboliccolorizer.h"
#include "photoselector/photoplugin.h"
#include "canvasindicators/canvasindicator.h"
#include "canvasindicators/ringingimageprovider.h"
#include "desktopview/desktopviewplugin.h"
#include "contactview/contactviewplugin.h"
#include "dialview/dialviewplugin.h"
#include "timeline/timelineplugin.h"

KDeclarative::KDeclarative* RingApplication::m_pDeclarative {nullptr};
RingQmlWidgets* RingApplication::m_pQmlWidget {nullptr};
PhotoSelectorPlugin* RingApplication::m_pPhotoSelector {nullptr};
DesktopView* RingApplication::m_pDesktopView {nullptr};
ContactView* RingApplication::m_pContactView {nullptr};
DialView* RingApplication::m_pDialView {nullptr};
TimelinePlugin* RingApplication::m_pTimeline {nullptr};
CanvasIndicator* RingApplication::m_pCanvasIndicator {nullptr};
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

void RingApplication::init()
{
   if ((!Session::instance()->callModel()->isConnected()) || (!Session::instance()->callModel()->isValid())) {
      QTimer::singleShot(5000,this,&RingApplication::daemonTimeout);
   }
}

/**
 * Destructor
 */
RingApplication::~RingApplication()
{
   delete m_pDeclarative;
   delete engine();
   delete m_pCanvasIndicator;
   delete m_pPhotoSelector;
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

#define QML_TYPE(name) qmlRegisterUncreatableType<name>(AppName, 1,0, #name, #name "cannot be instantiated");
#define QML_NS(name) qmlRegisterUncreatableMetaObject( name :: staticMetaObject, #name, 1, 0, #name, "Namespaces cannot be instantiated" );
#define QML_CRTYPE(name) qmlRegisterType<name>(AppName, 1,0, #name);

constexpr static const char AppName[]= "Ring";

/// Create a QML engine for various canvas widgets
QQmlApplicationEngine* RingApplication::engine()
{
   static QQmlApplicationEngine* e = nullptr;
   static std::atomic_flag engineInit = ATOMIC_FLAG_INIT;

   if (!engineInit.test_and_set()) {
      m_pQmlWidget = new RingQmlWidgets;
      m_pQmlWidget->registerTypes("RingQmlWidgets");

      m_pPhotoSelector = new PhotoSelectorPlugin;
      m_pPhotoSelector->registerTypes("PhotoSelectorPlugin");

      m_pDesktopView = new DesktopView;
      m_pDesktopView->registerTypes("DesktopView");

      m_pContactView = new ContactView;
      m_pContactView->registerTypes("ContactView");

      m_pDialView = new DialView;
      m_pDialView->registerTypes("DialView");

      m_pTimeline = new TimelinePlugin;
      m_pTimeline->registerTypes("TimeLine");

      m_pCanvasIndicator = new CanvasIndicator;
      m_pCanvasIndicator->registerTypes("CanvasIndicator");

      QML_TYPE( QAction)

      QML_CRTYPE( QItemSelectionModel )

      QML_NS(Ring)

      e = new QQmlApplicationEngine(QGuiApplication::instance());

      // Setup the icon theme provider and ki18n
      m_pDeclarative = new KDeclarative::KDeclarative;
      m_pDeclarative->setDeclarativeEngine(e);
      try {

         auto im = new RingingImageProvider();
         e->addImageProvider( QStringLiteral("RingingImageProvider"), im );
         e->addImportPath(QStringLiteral("qrc:/"));

         auto im2 = new SymbolicColorizer();
         e->addImageProvider( QStringLiteral("SymbolicColorizer"), im2 );

         VideoWidget3::initProvider();
      }
      catch(char const* e) {
         qDebug() << "Failed to connect to the daemon" << e;
         sync();
         ::exit(1);
      }
      catch(...) {
         qDebug() << "Failed to connect to the daemon with an unknown problem";
         ::exit(2);
      }
   }
   return e;
}

#undef QML_TYPE
#undef QML_CRTYPE

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

///The daemon is not found
void RingApplication::daemonTimeout()
{
   if ((!Session::instance()->callModel()->isConnected()) || (!Session::instance()->callModel()->isValid())) {
      KMessageBox::error(nullptr, ErrorMessage::NO_DAEMON_ERROR);
      exit(1);
   }
}

///Exit gracefully
bool RingApplication::notify (QObject* receiver, QEvent* e)
{
   try {
      return QApplication::notify(receiver,e);
   }
   catch (const Call::State& state) {
      qDebug() << ErrorMessage::GENERIC_ERROR << "CallState" << state;
      QTimer::singleShot(2500, this, &RingApplication::daemonTimeout);
   }
   catch (const Call::Action& state) {
      qDebug() << ErrorMessage::GENERIC_ERROR << "Call Action" << state;
      QTimer::singleShot(2500, this, &RingApplication::daemonTimeout);
   }
   catch (const QString& errorMessage) {
      KMessageBox::error(nullptr,errorMessage);
      QTimer::singleShot(2500, this, &RingApplication::daemonTimeout);
   }
   catch (const std::exception& e) {
      qDebug() << ErrorMessage::GENERIC_ERROR << e.what();
      KMessageBox::error(nullptr,ErrorMessage::GENERIC_ERROR);
      QTimer::singleShot(2500, this, &RingApplication::daemonTimeout);

   }
   catch (...) {
      Q_ASSERT(false);
      qDebug() << ErrorMessage::GENERIC_ERROR;
      KMessageBox::error(nullptr, ErrorMessage::GENERIC_ERROR);
      QTimer::singleShot(2500, this, &RingApplication::daemonTimeout);
   }
   return false;
}

bool RingApplication::mayHaveFocus()
{
   return m_HasFocus;
}

#include <ringapplication.moc>

// kate: space-indent on; indent-width 3; replace-tabs on;
