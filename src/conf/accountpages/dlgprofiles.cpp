/****************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                          *
 *   Author : Alexandre Lision <alexandre.lision@savoirfairelinux.com>      *
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
#include "dlgprofiles.h"

//KDE
#include <QDebug>
#include <QDateTime>
#include <QFileDialog>
#include <KLocalizedString>

//Qt
#include <QtQuick/QQuickItem>

//Ring
#include "profilemodel.h"
#include "personmodel.h"

DlgProfiles::DlgProfiles(QWidget *parent, QQmlEngine* e, const QString& name, const QString& uri) :
   QQuickWidget(e, parent), m_pCurrentPerson(nullptr)
{
   Q_UNUSED(name)
   Q_UNUSED(uri)

   setResizeMode(QQuickWidget::SizeRootObjectToView);
   setSource(QUrl(QStringLiteral("qrc:/ContactDialog.qml")));
   auto item = rootObject();

   if (!item) {
      qDebug() << errors();
      Q_ASSERT(false);
   }

   item->setProperty("showStat"     , false     );
   item->setProperty("showImage"    , true      );
   item->setProperty("forcedState"  , "profile" );

   connect(rootObject(), SIGNAL(changed()), this, SLOT(slotChanged()));
}

DlgProfiles::~DlgProfiles()
{

}

bool DlgProfiles::checkValues()
{
   return true; //!edit_name->text().isEmpty();
}

void DlgProfiles::loadPerson(Person* profile)
{
   auto item = rootObject();
   m_Lock = true;
   item->setProperty("currentPerson", QVariant::fromValue(profile));
   item->setProperty("showStat", false);
   item->setProperty("showImage", true);
   item->setProperty("showSave", false);
   m_Lock = false;
}

bool DlgProfiles::saveToPerson(Person* p)
{
   m_Lock = true;
   auto item = rootObject();

   if (p)
      item->setProperty("currentPerson", QVariant::fromValue(p));

   QMetaObject::invokeMethod(item, "save");
   m_Lock = false;
   return true;
}

Person* DlgProfiles::create(CollectionInterface* col)
{
   if(checkValues()) {

      Person* profile = new Person(col);

      saveToPerson(profile);

      return profile;
   }

   return nullptr;
}

void DlgProfiles::cancel()
{
   emit requestCancel();
}

void DlgProfiles::accept()
{
   emit requestSave();
}

//4.6 Image Restrictions
//The following rules apply to images:
//1. The image SHOULD use less than eight kilobytes (8k) of data; this restriction
//   is to be enforced by the publishing client.
//2. The image height and width SHOULD be between thirty-two (32) and ninety-six (96) pixels;
//   the recommended size is sixty-four (64) pixels high and sixty-four (64) pixels wide.
//3. The image SHOULD be square.
//4. The image content type SHOULD be image/gif, image/jpeg, or image/png;
//   support for the "image/png" content type is REQUIRED,
//   support for the "image/gif" and "image/jpeg" content types is RECOMMENDED,
//   and support for any other content type is OPTIONAL.

void DlgProfiles::slotChanged()
{
   if (m_Lock)
      return;
   emit changed();
}

// kate: space-indent on; indent-width 3; replace-tabs on;
