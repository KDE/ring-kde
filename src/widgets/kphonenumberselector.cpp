/***************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                         *
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
#include <QInputDialog>
#include <klocalizedstring.h>

//Ring
#include "contactmethod.h"
#include "numbercategory.h"
#include "phonedirectorymodel.h"
#include <ring.h>
#include <person.h>


void KPhoneNumberSelector::init()
{
}

ContactMethod* KPhoneNumberSelector::number(const Person* contact)
{
   if (contact) {
      if (contact->phoneNumbers().size()>1) {
         bool                   ok = false;
         QHash<QString,QString> map       ;
         QStringList            list      ;
         foreach (ContactMethod* number, contact->phoneNumbers()) {
            map[number->category()->name()+" ("+number->uri()+')'] = number->uri();
            list << number->category()->name()+" ("+number->uri()+')';
         }
         const QString result = QInputDialog::getItem (Ring::app(),i18n("Select phone number"), i18n("This contact has many phone numbers, please select the one you wish to call"), list, 0, false, &ok);
         return PhoneDirectoryModel::instance()->getNumber(result);//new ContactMethod(result,"");
      }
      else if (contact->phoneNumbers().size() == 1)
         return contact->phoneNumbers()[0];
   }
   return const_cast<ContactMethod*>(ContactMethod::BLANK());
}
