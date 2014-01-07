/***************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
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
#include "kphonenumberselector.h"

//KDE
#include <KInputDialog>
#include <KLocale>

//SFLPhone
#include "../lib/phonenumber.h"
#include "../lib/numbercategory.h"
#include "../lib/phonedirectorymodel.h"
#include "../sflphone.h"


void KPhoneNumberSelector::init()
{
   static KPhoneNumberSelector* v = new KPhoneNumberSelector();
   setDefaultVisitor(v);
}

PhoneNumber* KPhoneNumberSelector::getNumber(const Contact* contact)
{
   if (contact) {
      if (contact->phoneNumbers().size()>1) {
         bool                   ok = false;
         QHash<QString,QString> map       ;
         QStringList            list      ;
         foreach (PhoneNumber* number, contact->phoneNumbers()) {
            map[number->category()->name()+" ("+number->uri()+')'] = number->uri();
            list << number->category()->name()+" ("+number->uri()+')';
         }
         const QString result = KInputDialog::getItem (i18n("Select phone number"), i18n("This contact has many phone numbers, please select the one you wish to call"), list, 0, false, &ok,SFLPhone::app());
         return PhoneDirectoryModel::instance()->getNumber(result);//new PhoneNumber(result,"");
      }
      else if (contact->phoneNumbers().size() == 1)
         return contact->phoneNumbers()[0];
   }
   return const_cast<PhoneNumber*>(PhoneNumber::BLANK());
}
