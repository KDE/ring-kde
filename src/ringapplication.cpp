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
#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtCore/QCommandLineParser>
#include <QtCore/QDebug>
#include <QtCore/QStandardPaths>
#include <QtWidgets/QAction>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QQmlComponent>

//KDE
#include <KIconLoader>
#include <KActionCollection>
#include <kmessagebox.h>
#include <KDeclarative/KDeclarative>

//LRC
#include <callmodel.h>
#include <accountmodel.h>
#include <account.h>
#include <phonedirectorymodel.h>
#include <categorizedhistorymodel.h>
#include <categorizedcontactmodel.h>
#include <ringdevicemodel.h>
#include <namedirectory.h>
#include <categorizedbookmarkmodel.h>
#include <numbercompletionmodel.h>
#include <useractionmodel.h>
#include <pendingcontactrequestmodel.h>
#include <video/configurationproxy.h>
#include <video/sourcemodel.h>
#include <contactmethod.h>
#include <media/recordingmodel.h>
#include <peerstimelinemodel.h>
#include <peertimelinemodel.h>
#include <video/previewmanager.h>
#include <media/recording.h>
#include <media/textrecording.h>
#include <media/media.h>

//Ring
#include "klib/kcfg_settings.h"
#include "cmd.h"
#include "phonewindow.h"
#include "errormessage.h"
#include "callmodel.h"
#include "implementation.h"
#include "wizard/welcome.h"
#include "callview/videowidget.h"
#include "notification.h"
#include "actioncollection.h"

//Models
#include <profilemodel.h>
#include <certificatemodel.h>
#include <availableaccountmodel.h>
#include <numbercategorymodel.h>
// #include <presencestatusmodel.h>
#include <personmodel.h>

//Collections
#include <foldercertificatecollection.h>
#include <fallbackpersoncollection.h>
#include <peerprofilecollection2.h>
#include <localhistorycollection.h>
#include <localbookmarkcollection.h>
#include <localrecordingcollection.h>
#include <localprofilecollection.h>
#include <localtextrecordingcollection.h>
#include <globalinstances.h>

//Configurators
#include "configurator/localhistoryconfigurator.h"
#include "configurator/audiorecordingconfigurator.h"
#include "configurator/fallbackpersonconfigurator.h"
#include "configurator/peerprofileconfigurator.h"
#include "configurator/bookmarkconfigurator.h"

//Delegates
#include <delegates/kdepixmapmanipulation.h>
// #include <delegates/accountinfodelegate.h>
#include <interfaces/itemmodelstateserializeri.h>
#include "klib/itemmodelserialization.h"
#include "extensions/presencecollectionextension.h"

//QML
#include "qmlwidgets/plugin.h"
#include "qmlwidgets/recentfilemodel.h"
#include "photoselector/photoplugin.h"
#include "canvasindicators/canvasindicator.h"
#include "canvasindicators/ringingimageprovider.h"
#include "desktopview/desktopviewplugin.h"
#include "contactview/contactviewplugin.h"
#include "dialview/dialviewplugin.h"

//Other
#include <unistd.h>

KDeclarative::KDeclarative* RingApplication::m_pDeclarative {nullptr};
RingQmlWidgets* RingApplication::m_pQmlWidget {nullptr};
PhotoSelectorPlugin* RingApplication::m_pPhotoSelector {nullptr};
DesktopView* RingApplication::m_pDesktopView {nullptr};
ContactView* RingApplication::m_pContactView {nullptr};
DialView* RingApplication::m_pDialView {nullptr};
CanvasIndicator* RingApplication::m_pCanvasIndicator {nullptr};
RingApplication* RingApplication::m_spInstance {nullptr};

/**
 * The application constructor
 */
RingApplication::RingApplication(int & argc, char ** argv) : QApplication(argc,argv),m_StartIconified(false)
{
   Q_ASSERT(argc != -1);
#ifdef ENABLE_VIDEO
   //Necessary to draw OpenGL from a separated thread
   setAttribute(Qt::AA_X11InitThreads,true);
#endif
   setAttribute(Qt::AA_EnableHighDpiScaling);

   m_spInstance = this;
}

void RingApplication::init()
{
   if ((!CallModel::instance().isConnected()) || (!CallModel::instance().isValid())) {
      QTimer::singleShot(5000,this,&RingApplication::daemonTimeout);
   }

   initCollections();

   connect(&CallModel::instance(), &CallModel::callAdded, this, &RingApplication::callAdded);
   connect(&CallModel::instance(), &CallModel::callStateChanged, this, &RingApplication::callAdded);

}

/**
 * Destructor
 */
RingApplication::~RingApplication()
{
   // Delete the GUI before the models to prevent their destructors from
   // accessing the singletons
   if (m_pPhone) {
//       m_pPhone->setActive(false);
      delete m_pPhone;
      m_pPhone = nullptr;
   }

   delete m_pDeclarative;
   delete engine();
   delete m_pCanvasIndicator;
   delete m_pPhotoSelector;
   delete m_pQmlWidget;

   delete &PeersTimelineModel::instance();
   delete &Media::RecordingModel::instance();
   delete &PersonModel::instance();
   delete &CallModel::instance();
   delete &ProfileModel::instance();
   delete &AccountModel::instance();
   delete &PhoneDirectoryModel::instance();
   delete &NumberCategoryModel::instance();
   m_spInstance = nullptr;
}

RingApplication* RingApplication::instance()
{
   Q_ASSERT(m_spInstance);
   return m_spInstance;
}

static void loadNumberCategories()
{
   auto& model = NumberCategoryModel::instance();
   static const QString pathTemplate = QStringLiteral(":/mini/icons/miniicons/%1.png");
#define ICN(name) QPixmap(QString(pathTemplate).arg(QStringLiteral(name)))
   model.addCategory(i18n("Home")     ,ICN("home")     , 1 /*KABC::PhoneNumber::Home */);
   model.addCategory(i18n("Work")     ,ICN("work")     , 2 /*KABC::PhoneNumber::Work */);
   model.addCategory(i18n("Msg")      ,ICN("mail")     , 3 /*KABC::PhoneNumber::Msg  */);
   model.addCategory(i18n("Pref")     ,ICN("call")     , 4 /*KABC::PhoneNumber::Pref */);
   model.addCategory(i18n("Voice")    ,ICN("video")    , 5 /*KABC::PhoneNumber::Voice*/);
   model.addCategory(i18n("Fax")      ,ICN("call")     , 6 /*KABC::PhoneNumber::Fax  */);
   model.addCategory(i18n("Cell")     ,ICN("mobile")   , 7 /*KABC::PhoneNumber::Cell */);
   model.addCategory(i18n("Video")    ,ICN("call")     , 8 /*KABC::PhoneNumber::Video*/);
   model.addCategory(i18n("Bbs")      ,ICN("call")     , 9 /*KABC::PhoneNumber::Bbs  */);
   model.addCategory(i18n("Modem")    ,ICN("call")     , 10/*KABC::PhoneNumber::Modem*/);
   model.addCategory(i18n("Car")      ,ICN("car")      , 11/*KABC::PhoneNumber::Car  */);
   model.addCategory(i18n("Isdn")     ,ICN("call")     , 12/*KABC::PhoneNumber::Isdn */);
   model.addCategory(i18n("Pcs")      ,ICN("call")     , 13/*KABC::PhoneNumber::Pcs  */);
   model.addCategory(i18n("Pager")    ,ICN("pager")    , 14/*KABC::PhoneNumber::Pager*/);
   model.addCategory(i18n("Preferred"),ICN("preferred"), 10000                         );
#undef ICN
#undef IS_ENABLED
}

void RingApplication::initCollections()
{
   GlobalInstances::setInterface<KDEActionExtender>();

   GlobalInstances::setInterface<KDEPixmapManipulation>();

   loadNumberCategories();

   /*******************************************
      *           Set the configurator          *
      ******************************************/

   PersonModel::instance()             .registerConfigarator<PeerProfileCollection2  >    (new PeerProfileConfigurator   (this));
   PersonModel::instance()             .registerConfigarator<FallbackPersonCollection>    (new FallbackPersonConfigurator(this));
   CategorizedHistoryModel::instance() .registerConfigarator<LocalHistoryCollection  >    (new LocalHistoryConfigurator  (this));
   CategorizedBookmarkModel::instance().registerConfigarator<LocalBookmarkCollection >    (new BookmarkConfigurator      (this));
   Media::RecordingModel::instance()   .registerConfigarator<LocalRecordingCollection>    (new AudioRecordingConfigurator(this,
      AudioRecordingConfigurator::Mode::AUDIO
   ));
   Media::RecordingModel::instance()   .registerConfigarator<LocalTextRecordingCollection>(new AudioRecordingConfigurator(this,
      AudioRecordingConfigurator::Mode::TEXT
   ));

   /*******************************************
      *           Load the collections          *
      ******************************************/

   CategorizedHistoryModel::instance().addCollection<LocalHistoryCollection>(LoadOptions::FORCE_ENABLED);

   ProfileModel::instance().addCollection<LocalProfileCollection>(LoadOptions::FORCE_ENABLED);

#ifdef Q_OS_LINUX
   CertificateModel::instance().addCollection<FolderCertificateCollection,QString, FlagPack<FolderCertificateCollection::Options>,QString>(
      QStringLiteral("/usr/share/ca-certificates/"),
      FolderCertificateCollection::Options::ROOT
         | FolderCertificateCollection::Options::RECURSIVE
         | FolderCertificateCollection::Options::READ_ONLY,
      i18n("System root certificates"),
      LoadOptions::FORCE_ENABLED
   );
#endif

   CategorizedBookmarkModel::instance().addCollection<LocalBookmarkCollection>();
   CategorizedBookmarkModel::instance().reloadCategories();

   PersonModel::instance().addCollection<FallbackPersonCollection>(LoadOptions::FORCE_ENABLED);
   auto ppc = PersonModel::instance().addCollection<PeerProfileCollection2>(LoadOptions::FORCE_ENABLED);

   const auto m = static_cast<PeerProfileCollection2::DefaultMode>(ConfigurationSkeleton::defaultPeerProfileMode());
   ppc->setDefaultMode(m);

   GlobalInstances::setInterface<ItemModelStateSerialization>();
   GlobalInstances::itemModelStateSerializer().load();

#ifdef ENABLE_AKONADI
   AkonadiBackend::initCollections();
#endif

//       PresenceCollectionModelExtension* ext = new PresenceCollectionModelExtension(this);
//       PersonModel::instance().backendModel()->addExtension(ext); //FIXME

   ProfileModel::instance();

   Notification::instance();
}

///Parse command line arguments
int RingApplication::newInstance()
{
   static bool displayWizard  = ConfigurationSkeleton::enableWizard() || ConfigurationSkeleton::showSplash();
   const  bool displayOnStart = ConfigurationSkeleton::displayOnStart() && !startIconified();

   // The first run wizard
   if (displayOnStart && displayWizard) {
      // Also add this object
      engine()->rootContext()->setContextProperty(
         QStringLiteral("RingApplication"), this
      );

      engine()->rootContext()->setContextProperty(
         QStringLiteral("wizardWelcomeOnly"), QVariant(!ConfigurationSkeleton::enableWizard())
      );

      auto wiz = new WelcomeDialog();
      wiz->show();
      ConfigurationSkeleton::setEnableWizard(false);
      displayWizard = false;
      return 0;
   }

   static bool init = true;
   //Only call on the first instance
   if (init) {
      init = false;

      PhoneWindow* mw = nullptr;

      // Create the old qtwidgets main window so all classes expecting one still
      // behave as they should //FIXME fix KF5::KXMLGui
      auto mw2 = new KXmlGuiWindow();
      auto col = new KActionCollection(this);

      // Use a QTimer since it can enter in a recursion if there is a shortcut
      // collision creating a warning popup parented on the main window.
      #ifdef Q_OS_MAC
         QDir dir(QApplication::applicationDirPath());
         dir.cdUp();
         dir.cd("Resources/");
         QTimer::singleShot(0, [mw2,dir]() {mw2->createGUI(dir.path()+"/ring-kdeui.rc");});
      #else
         QTimer::singleShot(0, [mw2]() {mw2->createGUI();});
      #endif

      ActionCollection::instance()->setupAction(mw2, col);

      if (m_StartPhone) {
         mw = RingApplication::phoneWindow();

         if (displayOnStart)
            mw->show();
         else
            mw->hide();
      }
      else
         desktopWindow();
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

void RingApplication::setStartTimeline(bool value)
{
   m_StartTimeLine = value;
}

void RingApplication::setStartPhone(bool value)
{
   m_StartPhone = value;
}

#define QML_TYPE(name) qmlRegisterUncreatableType<name>(AppName, 1,0, #name, #name "cannot be instanciated");
#define QML_CRTYPE(name) qmlRegisterType<name>(AppName, 1,0, #name);
#define QML_SINGLETON(name) RingApplication::engine()->rootContext()->setContextProperty(QStringLiteral(#name), &name::instance());
#define QML_SINGLETON2(name) RingApplication::engine()->rootContext()->setContextProperty(QStringLiteral(#name), name::instance());
#define QML_ADD_OBJECT(name, obj) RingApplication::engine()->rootContext()->setContextProperty(QStringLiteral(#name), obj);

constexpr static const char AppName[]= "Ring";


/// Create a QML engine for various canvas widgets
QQmlApplicationEngine* RingApplication::engine()
{
   static QQmlApplicationEngine* e = nullptr;
   static std::atomic_flag engineInit {ATOMIC_FLAG_INIT};

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

      m_pCanvasIndicator = new CanvasIndicator;
      m_pCanvasIndicator->registerTypes("CanvasIndicator");

#ifdef Q_OS_ANDROID
      KirigamiPlugin::getInstance().registerTypes();
#endif

      QML_TYPE( Account           )
      QML_TYPE( const Account     )
      QML_TYPE( Call              )
      QML_TYPE( Person            )
      QML_TYPE( ContactMethod     )
      QML_TYPE( UserActionModel   )
      QML_TYPE( PeerTimelineModel )
      QML_TYPE( RingDeviceModel   )

      QML_TYPE( QAction)

      QML_CRTYPE( PeersTimelineSelectionModel )
      QML_CRTYPE( NumberCompletionModel       )

      e = new QQmlApplicationEngine(QGuiApplication::instance());

      // Setup the icon theme provider and ki18n
      m_pDeclarative = new KDeclarative::KDeclarative;
      m_pDeclarative->setDeclarativeEngine(e);
      m_pDeclarative->setupBindings();

      try {
         QML_SINGLETON( CallModel                );
         QML_SINGLETON( CategorizedHistoryModel  );
         QML_SINGLETON( AccountModel             );
         QML_SINGLETON( CategorizedContactModel  );
         QML_SINGLETON( CategorizedBookmarkModel );
         QML_SINGLETON( NameDirectory            );
         QML_SINGLETON( PeersTimelineModel       );
         QML_SINGLETON( NumberCategoryModel      );
         QML_SINGLETON( PhoneDirectoryModel      );
         QML_SINGLETON( RecentFileModel          );

         QML_SINGLETON2( ActionCollection         );

         QML_ADD_OBJECT(VideoRateSelectionModel      , &Video::ConfigurationProxy::rateSelectionModel      ());
         QML_ADD_OBJECT(VideoResolutionSelectionModel, &Video::ConfigurationProxy::resolutionSelectionModel());
         QML_ADD_OBJECT(VideoChannelSelectionModel   , &Video::ConfigurationProxy::channelSelectionModel   ());
         QML_ADD_OBJECT(VideoDeviceSelectionModel    , &Video::ConfigurationProxy::deviceSelectionModel    ());
         { using namespace Media;
            QML_SINGLETON( RecordingModel        );
            QML_TYPE     ( Recording             );
            QML_TYPE     ( TextRecording         );

         }

         { using namespace Video;
            QML_SINGLETON( PreviewManager        );
            QML_TYPE     ( SourceModel           );
         }

         qmlRegisterUncreatableType<::Media::Media>(
            AppName, 1,0, "Media", QStringLiteral("cannot be instanciated")
         );

         RingApplication::engine()->rootContext()->setContextProperty(
            QStringLiteral("SortedContactModel"),
            CategorizedContactModel::SortedProxy::instance().model()
         );


         auto im = new RingingImageProvider();
         e->addImageProvider( QStringLiteral("RingingImageProvider"), im );
         e->addImportPath(QStringLiteral("qrc:/"));

         VideoWidget3::initProvider();
      }
      catch(char const* e) {
         qDebug() << "Failed to connect to the daemon";
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
#undef QML_SINGLETON
#undef QML_ADD_OBJECT
#undef QML_CRTYPE

PhoneWindow* RingApplication::phoneWindow() const
{
   if (!m_pPhone) {
      m_pPhone = new PhoneWindow(nullptr);
//       connect(m_pPhone, &FancyMainWindow::unregisterWindow, this, [this]() {
//          m_pPhone = nullptr;
//       });
   }

   return m_pPhone;
}

QQuickWindow* RingApplication::desktopWindow() const
{
   static QQuickWindow* dw = nullptr;
   if (!dw) {
      QQmlComponent component(engine());
      component.loadUrl(QUrl(QStringLiteral("qrc:/DesktopWindow.qml")));
      if ( component.isReady() ) {
         if (auto obj = qobject_cast<QQuickItem*>(component.create())) {
            dw =  qobject_cast<QQuickWindow*>(obj);
            qDebug() << "\n\nHERE" << obj <<dw << obj->metaObject()->className() << obj->property("id");
         }
         else
            qWarning() << "FAILED TO LOAD:" << component.errorString();
      }
      else
         qWarning() << component.errorString();

      qDebug() << "\n\nDW!" << dw;
   }

   return dw;
}

bool RingApplication::isPhoneVisible() const
{
   return m_pPhone && m_pPhone->isVisible();
}

///The daemon is not found
void RingApplication::daemonTimeout()
{
   if ((!CallModel::instance().isConnected()) || (!CallModel::instance().isValid())) {
      KMessageBox::error(nullptr, ErrorMessage::NO_DAEMON_ERROR);
      exit(1);
   }
}

void RingApplication::callAdded(Call* c)
{
   if (c && ConfigurationSkeleton::displayOnNewCalls() && (
    c->state() == Call::State::CURRENT   ||
    c->state() == Call::State::INCOMING  ||
    c->state() == Call::State::CONNECTED ||
    c->state() == Call::State::RINGING   ||
    c->state() == Call::State::INITIALIZATION)) {
      if (isPhoneVisible()) {
         RingApplication::instance()->phoneWindow()->show ();
         RingApplication::instance()->phoneWindow()->raise();
      }
      else {
         //timelineWindow()->viewContact(c->peerContactMethod());
         //timelineWindow()->setCurrentPage(ViewContactDock::Pages::MEDIA);
      }
   }
}

void RingApplication::showWizard()
{
   RingApplication::engine()->rootContext()->setContextProperty(
      QStringLiteral("wizardWelcomeOnly"), QVariant(false)
   );

   auto wiz = new WelcomeDialog();
   wiz->show();
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

// kate: space-indent on; indent-width 3; replace-tabs on;
