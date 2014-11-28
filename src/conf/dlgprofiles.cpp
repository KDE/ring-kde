/****************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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
#include <KIcon>
#include <QDebug>
#include <QDateTime>
#include <QFileDialog>

//Ring
#include "lib/profilemodel.h"
#include "lib/contactmodel.h"

DlgProfiles::DlgProfiles(QWidget *parent) : QWidget(parent)
{
   setupUi(this);
   qDebug() << "Constructing DlgProfiles";
}

DlgProfiles::~DlgProfiles()
{

}

bool DlgProfiles::checkValues()
{
   return !edit_name->text().isEmpty();
}

void DlgProfiles::accept()
{
   qDebug() << "accept";
   if(checkValues()) {

      Contact* profile = new Contact(ProfileModel::instance()->getBackEnd());
      profile->setUid(QString::number(QDateTime::currentDateTime().currentMSecsSinceEpoch()).toUtf8());
      profile->setFirstName(edit_name->text());
      profile->setFamilyName(edit_lname->text());
      profile->setPreferredEmail(edit_email->text());
      profile->setFormattedName(edit_name->text() + " " + edit_lname->text());
      if(photoView->pixmap()) {
         QPixmap photo = *photoView->pixmap();
         profile->setPhoto(QVariant::fromValue(photo));
      }

      Contact::Address* test = new Contact::Address();
      test->addressLine = edit_address_1->text();
      test->city = edit_city->text();
      test->country = edit_country->text();
      test->type = cb_address_type->currentText();
      test->postalCode = edit_postal_code->text();
      test->state = edit_state->text();

      profile->addAddress(test);

      //Get values of custom fields
      for (QString key : m_hCustomFields.keys()) {
         profile->addCustomField(key, m_hCustomFields.value(key)->text());
      }

      ProfileModel::instance()->addNewProfile(profile, nullptr);
   }
}

void DlgProfiles::cancel()
{
   qDebug() << "cancel";
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

void DlgProfiles::choosePhoto()
{
   QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"),  QDir::currentPath(), tr("Image Files (*.png *.jpg *.gif)"));
   qDebug() << "choosePhoto result: " << fileName;

   QPixmap pic(fileName);
   QFileInfo meta(fileName);

   // Limit size to 10Ko instead of 8, 96x96 images are too big otherwise
   if(meta.size() > 10000) {
      qDebug() << "Image is too big (max size is 10Ko): " << meta.size();
      return;
   }

   //TODO: Allow resize on the fly
   if(pic.width() != pic.height()) {
      qDebug() << "Itmage is not square";
      return;
   }

   if(pic.width() > 96) {
      qDebug() << "Maximum size is 96px";
      return;
   }

   if(pic.width() < 32) {
      qDebug() << "Minimum size is 32px";
      return;
   }

   photoView->setPixmap(pic);
}

void DlgProfiles::addCustomField()
{
   qDebug() << "addCustom";
   if(!edit_custom_field->text().isEmpty()) {
      QLineEdit* edit_custom = new QLineEdit(this);
      formLayout->insertRow(formLayout->rowCount() - 1, edit_custom_field->text(), edit_custom);
      m_hCustomFields.insert("X-" + edit_custom_field->text().toUpper(),edit_custom);
   }
}

