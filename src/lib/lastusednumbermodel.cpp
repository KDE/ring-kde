/****************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                          *
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
#include "lastusednumbermodel.h"
#include "call.h"
#include "uri.h"
#include "phonenumber.h"

LastUsedNumberModel* LastUsedNumberModel::m_spInstance = nullptr;


struct ChainedPhoneNumber {
   ChainedPhoneNumber(PhoneNumber* n) : m_pPrevious(nullptr),m_pNext(nullptr),m_pSelf(n){}
   ChainedPhoneNumber* m_pPrevious;
   ChainedPhoneNumber* m_pNext;
   PhoneNumber*  m_pSelf;
};

class LastUsedNumberModelPrivate
{
public:
   LastUsedNumberModelPrivate();

   //Const
   constexpr static const int MAX_ITEM = 15;

   //Attributes
   ChainedPhoneNumber* m_pFirstNode;
   QHash<PhoneNumber*,ChainedPhoneNumber*> m_hNumbers;
   bool m_IsValid;
   ChainedPhoneNumber* m_lLastNumbers[MAX_ITEM];
};

LastUsedNumberModelPrivate::LastUsedNumberModelPrivate():m_pFirstNode(nullptr),m_IsValid(false)
{}

LastUsedNumberModel::LastUsedNumberModel() : QAbstractListModel(),d_ptr(new LastUsedNumberModelPrivate())
{
   for (int i=0;i<LastUsedNumberModelPrivate::MAX_ITEM;i++)
      d_ptr->m_lLastNumbers[i] = nullptr;
}

LastUsedNumberModel::~LastUsedNumberModel()
{
   delete d_ptr;
}

LastUsedNumberModel* LastUsedNumberModel::instance()
{
   if (!m_spInstance) {
      m_spInstance = new LastUsedNumberModel();
   }
   return m_spInstance;
}

///Push 'call' phoneNumber on the top of the stack
void LastUsedNumberModel::addCall(Call* call)
{
   PhoneNumber* number = call->peerPhoneNumber();
   ChainedPhoneNumber* node = d_ptr->m_hNumbers[number];
   if (!number || ( node && d_ptr->m_pFirstNode == node) ) {
      //TODO enable threaded numbers now
      return;
   }

   if (!node) {
      node = new ChainedPhoneNumber(number);
      d_ptr->m_hNumbers[number] = node;
   }
   else {
      if (node->m_pPrevious)
         node->m_pPrevious->m_pNext = node->m_pNext;
      if (node->m_pNext)
         node->m_pNext->m_pPrevious = node->m_pPrevious;
   }
   if (d_ptr->m_pFirstNode) {
      d_ptr->m_pFirstNode->m_pPrevious = node;
      node->m_pNext = d_ptr->m_pFirstNode;
   }
   d_ptr->m_pFirstNode = node;
   d_ptr->m_IsValid = false;
   emit layoutChanged();
}


QVariant LastUsedNumberModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();
   if (!d_ptr->m_IsValid) {
      ChainedPhoneNumber* current = d_ptr->m_pFirstNode;
      for (int i=0;i<LastUsedNumberModelPrivate::MAX_ITEM;i++) { //Can only grow, no need to clear
         d_ptr->m_lLastNumbers[i] = current;
         current = current->m_pNext;
         if (!current)
            break;
      }
      d_ptr->m_IsValid = true;
   }
   switch (role) {
      case Qt::DisplayRole: {
         return d_ptr->m_lLastNumbers[index.row()]->m_pSelf->uri();
      }
   };
   return QVariant();
}

int LastUsedNumberModel::rowCount( const QModelIndex& parent) const
{
   if (parent.isValid())
      return 0;
   return d_ptr->m_hNumbers.size() < LastUsedNumberModelPrivate::MAX_ITEM?d_ptr->m_hNumbers.size():LastUsedNumberModelPrivate::MAX_ITEM;
}

Qt::ItemFlags LastUsedNumberModel::flags( const QModelIndex& index) const
{
   Q_UNUSED(index)
   return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool LastUsedNumberModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}
