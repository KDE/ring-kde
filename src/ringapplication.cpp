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
#include <QQmlApplicationEngine>
#include <QQmlContext>

//KDE
#include <KIconLoader>
#include <KMainWindow>
#include <kmessagebox.h>
#include <KDeclarative/KDeclarative>

//LRC
#include <callmodel.h>
#include <accountmodel.h>
#include <account.h>
#include <categorizedhistorymodel.h>
#include <categorizedcontactmodel.h>
#include <namedirectory.h>
#include <categorizedbookmarkmodel.h>
#include <useractionmodel.h>
#include <contactmethod.h>
#include <recentmodel.h>
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
#include "timelinewindow.h"
#include "errormessage.h"
#include "callmodel.h"
#include "implementation.h"
#include "wizard/welcome.h"
#include "video/videowidget.h"
#include "notification.h"

//Models
#include <profilemodel.h>
#include <certificatemodel.h>
#include <availableaccountmodel.h>
#include <numbercategorymodel.h>
// #include <presencestatusmodel.h>
#include <personmodel.h>
#include <macromodel.h>

//Collections
#include <foldercertificatecollection.h>
#include <fallbackpersoncollection.h>
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

//Delegates
#include <delegates/kdepixmapmanipulation.h>
// #include <delegates/accountinfodelegate.h>
#include <interfaces/itemmodelstateserializeri.h>
#include "klib/itemmodelserialization.h"
#include "extensions/presencecollectionextension.h"

//QML
#include "qmlwidgets/plugin.h"
#include "photoselector/photoplugin.h"

//Other
#include <unistd.h>

/**
 * The application constructor
 */
RingApplication::RingApplication(int & argc, char ** argv) : QApplication(argc,argv),m_StartIconified(false)
{
   Q_ASSERT(argc != -1);

   qDebug() << argc << argv << this << argv[0];
#ifdef ENABLE_VIDEO
   //Necessary to draw OpenGL from a separated thread
   setAttribute(Qt::AA_X11InitThreads,true);
#endif
   setAttribute(Qt::AA_EnableHighDpiScaling);

   if ((!CallModel::instance().isConnected()) || (!CallModel::instance().isValid())) {
      QTimer::singleShot(5000,this,&RingApplication::daemonTimeout);
   }

   initCollections();
}

/**
 * Destructor
 */
RingApplication::~RingApplication()
{
   delete &CallModel::instance();
   delete &PersonModel::instance();
}

RingApplication* RingApplication::instance(int& argc, char** argv)
{
   static RingApplication* i = new RingApplication(argc, argv);

   return i;
}

RingApplication* RingApplication::instance()
{
   int i = 0;
   return RingApplication::instance(i, nullptr);
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

   PersonModel::instance()            .registerConfigarator<FallbackPersonCollection>    (new FallbackPersonConfigurator(this));
   Media::RecordingModel::instance()  .registerConfigarator<LocalRecordingCollection>    (new AudioRecordingConfigurator(this));
   Media::RecordingModel::instance()  .registerConfigarator<LocalTextRecordingCollection>(new AudioRecordingConfigurator(this));
   CategorizedHistoryModel::instance().registerConfigarator<LocalHistoryCollection  >    (new LocalHistoryConfigurator  (this));

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
   static bool wizardshown = false;

   // The first run wizard
   if ((!wizardshown) && (ConfigurationSkeleton::enableWizard() || ConfigurationSkeleton::showSplash())) {
      // Also add this object
      engine()->rootContext()->setContextProperty(
         "RingApplication", this
      );

      engine()->rootContext()->setContextProperty(
         "wizardWelcomeOnly", QVariant(!ConfigurationSkeleton::enableWizard())
      );

      auto wiz = new WelcomeDialog();
      wiz->show();
      ConfigurationSkeleton::setEnableWizard(false);
      wizardshown = true;
      return 0;
   }

   static bool init = true;
   //Only call on the first instance
   if (init) {
      init = false;

      FancyMainWindow* mw = nullptr;

      if (m_StartPhone)
         mw = RingApplication::phoneWindow();
      else
         mw = RingApplication::timelineWindow();

      if (ConfigurationSkeleton::displayOnStart() && !startIconified())
         mw->show();
      else
         mw->hide();
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
#define QML_SINGLETON(name) RingApplication::engine()->rootContext()->setContextProperty(#name, &name::instance());

constexpr static const char AppName[]= "Ring";


/// Create a QML engine for various canvas widgets
QQmlApplicationEngine* RingApplication::engine()
{
   static QQmlApplicationEngine* e = nullptr;
   if (!e) {
      auto p1 = new RingQmlWidgets;
      p1->registerTypes("RingQmlWidgets");

      auto p2 = new PhotoSelectorPlugin;
      p2->registerTypes("PhotoSelectorPlugin");

      QML_TYPE( Account           )
      QML_TYPE( const Account     )
      QML_TYPE( Call              )
      QML_TYPE( Person            )
      QML_TYPE( ContactMethod     )
      QML_TYPE( UserActionModel   )
      QML_TYPE( PeerTimelineModel )

      e = new QQmlApplicationEngine(QGuiApplication::instance());

      // Setup the icon theme provider and ki18n
      auto decl = new KDeclarative::KDeclarative;
      decl->setDeclarativeEngine(e);
      decl->setupBindings();

      QML_SINGLETON( CallModel                );
      QML_SINGLETON( CategorizedHistoryModel  );
      QML_SINGLETON( AccountModel             );
      QML_SINGLETON( CategorizedContactModel  );
      QML_SINGLETON( CategorizedBookmarkModel );
      QML_SINGLETON( NameDirectory            );
      QML_SINGLETON( RecentModel              );
      QML_SINGLETON( PeersTimelineModel       );
      QML_SINGLETON( NumberCategoryModel      );

      { using namespace Media;
         QML_SINGLETON( RecordingModel        );
         QML_TYPE     ( Recording             );
         QML_TYPE     ( TextRecording         );

      }

      { using namespace Video;
         QML_SINGLETON( PreviewManager        );
      }

      qmlRegisterUncreatableType<::Media::Media>(
         AppName, 1,0, "Media", "cannot be instanciated"
      );

      VideoWidget3::initProvider();
   }
   return e;
}

#undef QML_TYPE
#undef QML_SINGLETON

PhoneWindow* RingApplication::phoneWindow() const
{
   if (!m_pPhone)
      m_pPhone = new PhoneWindow(nullptr);

   return m_pPhone;
}

TimelineWindow* RingApplication::timelineWindow() const
{
   if (!m_pTimeline)
      m_pTimeline = new TimelineWindow();

   return m_pTimeline;
}

FancyMainWindow* RingApplication::mainWindow() const
{
   if (m_pPhone && !m_pTimeline)
      return m_pPhone;

   return RingApplication::timelineWindow();
}

///The daemon is not found
void RingApplication::daemonTimeout()
{
   if ((!CallModel::instance().isConnected()) || (!CallModel::instance().isValid())) {
      KMessageBox::error(mainWindow(), ErrorMessage::NO_DAEMON_ERROR);
      exit(1);
   }
}

void RingApplication::showWizard()
{
   RingApplication::engine()->rootContext()->setContextProperty(
      "wizardWelcomeOnly", QVariant(false)
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
      KMessageBox::error(mainWindow(),errorMessage);
      QTimer::singleShot(2500, this, &RingApplication::daemonTimeout);
   }
   catch (...) {
      qDebug() << ErrorMessage::GENERIC_ERROR;
      KMessageBox::error(mainWindow(),ErrorMessage::GENERIC_ERROR);
      QTimer::singleShot(2500, this, &RingApplication::daemonTimeout);
   }
   return false;
}

// kate: space-indent on; indent-width 3; replace-tabs on;
