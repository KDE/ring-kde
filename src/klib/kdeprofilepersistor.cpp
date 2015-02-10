/****************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                          *
 *   Author : Alexandre Lision <alexandre.lision@savoirfairelinux.com> *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#include "kdeprofilepersistor.h"

#include <person.h>
#include <profilemodel.h>
#include <QFile>
#include <KStandardDirs>

bool KDEProfilePersister::load()
{
   //TODO: Replace this with real directory and iterate over it to load all profiles
   QDir profilesDir = getProfilesDir();
   qDebug() << "Loading vcf from:" << profilesDir;

   QStringList extensions = QStringList();
   extensions << "*.vcf";

   QStringList entries = profilesDir.entryList(extensions, QDir::Files);

//   for (QString item : entries) {
//      QFile file(item);
//      if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//         return 1;

//      Person* profile = new Person(ProfileModel::instance());
//      while (!file.atEnd()) {
//         QByteArray line = file.readLine();
//         profile->setFormattedName("ALRIGHT");

//         line.split(';');
//      }
//   }

   return false;
}

bool KDEProfilePersister::save(const Person* c)
{
     QDir profilesDir = getProfilesDir();
     qDebug() << "Saving vcf in:" << profilesDir.absolutePath()+"/"+c->uid()+".vcf";
//   const QByteArray result = c->toVCard();

//   QFile file(profilesDir.absolutePath()+"/"+c->uid()+".vcf");
//   file.open(QIODevice::WriteOnly);
//   file.write(result);
//   file.close();

   return false;
}

QDir KDEProfilePersister::getProfilesDir()
{
   QDir(KStandardDirs::locateLocal("appdata","")).mkdir("profiles/");
   return QDir(KStandardDirs::locateLocal("appdata","")+"profiles/");
}
