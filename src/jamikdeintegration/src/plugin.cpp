/************************************************************************************
 *   Copyright (C) 2018 by BlueSystems GmbH                                         *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                            *
 *                                                                                  *
 *   This library is free software; you can redistribute it and/or                  *
 *   modify it under the terms of the GNU Lesser General Public                     *
 *   License as published by the Free Software Foundation; either                   *
 *   version 2.1 of the License, or (at your option) any later version.             *
 *                                                                                  *
 *   This library is distributed in the hope that it will be useful,                *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                 *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU              *
 *   Lesser General Public License for more details.                                *
 *                                                                                  *
 *   You should have received a copy of the GNU Lesser General Public               *
 *   License along with this library; if not, write to the Free Software            *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA *
 ***********************************************************************************/
#include "plugin.h"

// Qt
#include <QQmlEngine>
#include <QQmlContext>
#include <QtGui/QIcon>
#include <QtCore/QFile>
#include <QtCore/QStandardPaths>

// KDE
#include <KLocalizedString>

// LinRingQt
#include <globalinstances.h>
#include <interfaces/itemmodelstateserializeri.h>
#include <session.h>
#include <numbercategorymodel.h>
#include <callmodel.h>
#include <accountmodel.h>
#include <profilemodel.h>
#include <certificatemodel.h>
#include <bookmarkmodel.h>
#include <persondirectory.h>
#include <libcard/historyimporter.h>

// Collections
#include <foldercertificatecollection.h>
#include <fallbackpersoncollection.h>
#include <peerprofilecollection2.h>
#include <localhistorycollection.h>
#include <localbookmarkcollection.h>
#include <localrecordingcollection.h>
#include <localprofilecollection.h>
#include <localtextrecordingcollection.h>

// Ring-KDE
#include "notification.h"
#include "actioncollection.h"
#include "actioninterface.h"
#include "colorinterface.h"
#include "pixmapinterface.h"
#include "serializationinterface.h"
#include "windowevent.h"
#include "systray.h"
#include "kcfg_settings.h"

#include "sharedassets/qrc_assets.cpp"

static QQmlEngine* e = nullptr;

static void loadNumberCategories()
{
   auto model = Session::instance()->numberCategoryModel();
   static const QString pathTemplate = QStringLiteral(":/mini/icons/miniicons/%1.png");
#define ICN(name) QPixmap(QString(pathTemplate).arg(QStringLiteral(name)))
   model->addCategory(i18n("Home")     ,ICN("home")     , 1 /*KABC::PhoneNumber::Home */);
   model->addCategory(i18n("Work")     ,ICN("work")     , 2 /*KABC::PhoneNumber::Work */);
   model->addCategory(i18n("Msg")      ,ICN("mail")     , 3 /*KABC::PhoneNumber::Msg  */);
   model->addCategory(i18n("Pref")     ,ICN("call")     , 4 /*KABC::PhoneNumber::Pref */);
   model->addCategory(i18n("Voice")    ,ICN("video")    , 5 /*KABC::PhoneNumber::Voice*/);
   model->addCategory(i18n("Fax")      ,ICN("call")     , 6 /*KABC::PhoneNumber::Fax  */);
   model->addCategory(i18n("Cell")     ,ICN("mobile")   , 7 /*KABC::PhoneNumber::Cell */);
   model->addCategory(i18n("Video")    ,ICN("call")     , 8 /*KABC::PhoneNumber::Video*/);
   model->addCategory(i18n("Bbs")      ,ICN("call")     , 9 /*KABC::PhoneNumber::Bbs  */);
   model->addCategory(i18n("Modem")    ,ICN("call")     , 10/*KABC::PhoneNumber::Modem*/);
   model->addCategory(i18n("Car")      ,ICN("car")      , 11/*KABC::PhoneNumber::Car  */);
   model->addCategory(i18n("Isdn")     ,ICN("call")     , 12/*KABC::PhoneNumber::Isdn */);
   model->addCategory(i18n("Pcs")      ,ICN("call")     , 13/*KABC::PhoneNumber::Pcs  */);
   model->addCategory(i18n("Pager")    ,ICN("pager")    , 14/*KABC::PhoneNumber::Pager*/);
   model->addCategory(i18n("Preferred"),ICN("preferred"), 10000                         );
#undef ICN
#undef IS_ENABLED
}

static void initCollections()
{
   loadNumberCategories();

   Session::instance()->callModel()->setAudoCleanDelay(5000);

   // Load the old phone call history and port it to the newer calendar events format.
   if (QFile::exists(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QLatin1Char('/') +"history.ini")) {
      auto histo = Session::instance()->historyModel()->addCollection<LocalHistoryCollection>(LoadOptions::FORCE_ENABLED);
      HistoryImporter::importHistory(histo);
      histo->clear();
   }

   //HACK load the Calendar now to speedup everything else
   const int accountCount = Session::instance()->accountModel()->size();
   for (int i=0; i < accountCount; i++)
      (*Session::instance()->accountModel())[i]->calendar();

   Session::instance()->profileModel()->addCollection<LocalProfileCollection>(LoadOptions::FORCE_ENABLED);

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

   Session::instance()->bookmarkModel()->addCollection<LocalBookmarkCollection>();
   Session::instance()->bookmarkModel()->setDisplayPopular(
      ConfigurationSkeleton::displayPopularAsBookmark()
   );

   Session::instance()->personDirectory()->addCollection<FallbackPersonCollection>(LoadOptions::FORCE_ENABLED);
   auto ppc = Session::instance()->personDirectory()->addCollection<PeerProfileCollection2>(LoadOptions::FORCE_ENABLED);

   const auto m = static_cast<PeerProfileCollection2::DefaultMode>(ConfigurationSkeleton::defaultPeerProfileMode());
   ppc->setDefaultMode(m);

#ifdef ENABLE_AKONADI
   AkonadiBackend::initCollections();
#endif

   Session::instance()->profileModel();

}

void JamiKDEIntegration::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QByteArray("org.kde.ringkde.jamikdeintegration"));

    GlobalInstances::setInterface<ActionInterface>();

    GlobalInstances::setInterface<ColorInterface>();

    GlobalInstances::setInterface<PixmapInterface>();

    GlobalInstances::setInterface<SerializationInterface>();
    GlobalInstances::itemModelStateSerializer().load();

    qmlRegisterType<WindowEvent>(uri, 1, 0, "WindowEvent");
}

void JamiKDEIntegration::initializeEngine(QQmlEngine *engine, const char *uri)
{
    e = engine;
    Notification::instance();
    ActionCollection::instance()->setupAction();

    initCollections();

    engine->rootContext()->setContextProperty(
        QStringLiteral("ActionCollection"), ActionCollection::instance()
    );

    new SysTray(QIcon(QStringLiteral(":appicon/icons/64-apps-ring-kde.png")));
}

QQmlEngine* JamiKDEIntegration::engine()
{
    return e;
}
