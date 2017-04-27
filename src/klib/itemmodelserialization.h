/****************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
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
#ifndef ITEMMODELSTATESERIALIZATION_H
#define ITEMMODELSTATESERIALIZATION_H

#include <QtCore/QHash>

#include "typedefs.h"
#include <interfaces/itemmodelstateserializeri.h>
class Account;
class CollectionInterface;

class LIB_EXPORT ItemModelStateSerialization : public Interfaces::ItemModelStateSerializerI
{
public:
   virtual bool save() override;
   virtual bool load() override;
   virtual ~ItemModelStateSerialization();

   //Getter
   virtual bool isChecked(const CollectionInterface* backend) const override;
   virtual CollectionInterface* preferredCollection(
      CollectionManagerInterfaceBase* manager,
      FlagPack<CollectionInterface::SupportedFeatures> features,
      FlagPack<Interfaces::ItemModelStateSerializerI::Hints> hints
   ) override;

   //Setter
   virtual bool setChecked(const CollectionInterface* backend, bool enabled) override;

private:
   QHash<QByteArray,bool> m_hChecked;
   QHash<QByteArray,bool> m_hLoaded ;
   //Helper
//    void digg(const CollectionInterface* idx);
};

#endif

// kate: space-indent on; indent-width 3; replace-tabs on;
