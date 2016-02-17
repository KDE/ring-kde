/****************************************************************************
 *   Copyright (C) 2015 by Emmanuel Lepage Vallee                           *
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
#include "roletransformationproxy.h"

typedef std::function<QVariant(const QModelIndex&)> Mutator;

class RoleTransformationProxyPrivate final
{
public:

   // Types
   enum class Mode {
      NONE,
      ROLE,
      STATIC,
      FUNCTION
   };

   ///Save some memory as only one type is currently supported, this may change
   typedef union Storage {
      int      role;
      QVariant var ;
      Mutator* func;
   } Storage; //TODO make sure to cleanup when mode change or enjoy SIGSEGV

   // Attributes

   //TODO make this lfexible beyons Qt::UserRole
   Mode m_Mode {Mode::NONE};
   Storage* m_Map[Qt::UserRole+1] {nullptr}; //TODO don't do this

   // Helpers
   inline void initRole(int destination);
};

void RoleTransformationProxyPrivate::initRole(int destination)
{
   if (!m_Map[destination])
      m_Map[destination] = (Storage*) malloc(sizeof(Storage));;
}

RoleTransformationProxy::RoleTransformationProxy(QObject* parent) : QIdentityProxyModel(parent),
d_ptr( new RoleTransformationProxyPrivate() )
{
}

RoleTransformationProxy::~RoleTransformationProxy()
{
   for(int i = 0; i < Qt::UserRole; i++) {
      if (d_ptr->m_Map[i]) {
         if (d_ptr->m_Mode == RoleTransformationProxyPrivate::Mode::FUNCTION)
            free(d_ptr->m_Map[i]->func);

         free(d_ptr->m_Map[i]);
      }
   }
   delete d_ptr;
}

void RoleTransformationProxy::setRole(int destination, int sourceRole)
{
   if (destination < 0 || destination > Qt::UserRole)
      return; //TODO remove limitation

   d_ptr->initRole(destination);

   d_ptr->m_Map[destination]->role = sourceRole;

   d_ptr->m_Mode = RoleTransformationProxyPrivate::Mode::ROLE;
}

void RoleTransformationProxy::setRole(int destination, const QVariant& var)
{
   if (destination < 0 || destination > Qt::UserRole)
      return; //TODO remove limitation

   d_ptr->initRole(destination);

   d_ptr->m_Map[destination]->var = var;

   d_ptr->m_Mode = RoleTransformationProxyPrivate::Mode::STATIC;
}

void RoleTransformationProxy::setRole(int destination, const std::function<QVariant(const QModelIndex&)>& f)
{
   if (destination < 0 || destination > Qt::UserRole)
      return; //TODO remove limitation

   d_ptr->initRole(destination);

   d_ptr->m_Map[destination]->func = new Mutator(f);

   d_ptr->m_Mode = RoleTransformationProxyPrivate::Mode::FUNCTION;
}

QVariant RoleTransformationProxy::data( const QModelIndex& index, int role ) const
{
   if (!index.isValid())
      return {};

   if (role >= 0 && role <= Qt::UserRole && d_ptr->m_Map[role]) { //TODO remove limitation
      switch (d_ptr->m_Mode) {
         case RoleTransformationProxyPrivate::Mode::NONE    :
            return QIdentityProxyModel::data(index, role);
         case RoleTransformationProxyPrivate::Mode::ROLE    :
            return QIdentityProxyModel::data(index, d_ptr->m_Map[role]->role);
         case RoleTransformationProxyPrivate::Mode::STATIC  :
            return d_ptr->m_Map[role]->var;
         case RoleTransformationProxyPrivate::Mode::FUNCTION:
            return (*d_ptr->m_Map[role]->func)(index);
      }
   }

   return QIdentityProxyModel::data(index, role);
}
