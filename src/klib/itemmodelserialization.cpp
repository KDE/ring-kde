/****************************************************************************
 *   Copyright (C) 2014 by Savoir-Faire Linux                               *
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
#include "itemmodelserialization.h"

#include "../lib/abstractitembackend.h"
#include "kcfg_settings.h"

ItemModelStateSerialization::~ItemModelStateSerialization()
{
   
}

bool ItemModelStateSerialization::save()
{
   //FIXME this is too tied to the contact backend, it need to support roles for
   //other collection types (aka: bookmarks)
   QStringList ret;
   for (QHash<QByteArray,bool>::iterator i = m_hChecked.begin(); i != m_hChecked.end(); ++i) {
      if (i.value())
         ret << i.key();
   }
//    digg(QModelIndex());
   ConfigurationSkeleton::setDisabledCollectionList(ret);
   return true;
}

bool ItemModelStateSerialization::load()
{
   m_hChecked.clear();
   const QStringList disabled = ConfigurationSkeleton::disabledCollectionList();
   foreach(const QString& str, disabled) {
      m_hChecked[str.toAscii()] = true; //Disabled == true, enabled == false
   }
   return true;
}

bool ItemModelStateSerialization::isChecked(AbstractItemBackendBase* backend) const
{
   return !(m_hChecked[backend->id()]);
}

bool ItemModelStateSerialization::setChecked(AbstractItemBackendBase* backend, bool enabled)
{
   m_hChecked[backend->id()] = ! enabled;
   return true;
}
