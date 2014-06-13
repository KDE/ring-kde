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

#ifndef ITEMMODELSTATESERIALIZATION_H
#define ITEMMODELSTATESERIALIZATION_H

#include <QtCore/QHash>

#include "../lib/typedefs.h"
#include "../lib/visitors/itemmodelstateserializationvisitor.h"
class Account;
class AbstractItemBackendBase;

///SFLPhonelib Qt does not link to QtGui, and does not need to, this allow to add runtime Gui support
class LIB_EXPORT ItemModelStateSerialization : public ItemModelStateSerializationVisitor {
public:
   virtual bool save();
   virtual bool load();
   virtual ~ItemModelStateSerialization();

   //Getter
   virtual bool isChecked(AbstractItemBackendBase* backend) const;

   //Setter
   virtual bool setChecked(AbstractItemBackendBase* backend, bool enabled);

private:
   QHash<QByteArray,bool> m_hChecked;
   QHash<QByteArray,bool> m_hLoaded ;
   //Helper
//    void digg(const AbstractItemBackendBase* idx);
};

#endif
