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

//
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
//      ContactPlaceHolder *c = new ContactPlaceHolder(edit_name->text().toUtf8());
//      c->setFormattedName(edit_name->text());
//      ContactModel::instance()->addContact(c);

      Contact* profile = ContactModel::instance()->getContactByUid(QString("12345678900").toUtf8());

      if(profile) {
         profile->setFormattedName(edit_name->text());
      }
   }
}

void DlgProfiles::cancel()
{
   qDebug() << "cancel";
}

