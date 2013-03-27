/****************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                               *
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
#include "bookmarkmodel.h"

//SFLPhone
#include "configurationskeleton.h"

BookmarkModel* BookmarkModel::m_pSelf = nullptr;

class NumberTreeBackend : public BookmarkTreeBackend, public QObject
{
   friend class BookmarkModel;
   public:
      NumberTreeBackend(QString name): BookmarkTreeBackend(BookmarkTreeBackend::Type::NUMBER),m_Name(name){}
      virtual QObject* getSelf() { return this; }

   private:
      QString m_Name;
};

BookmarkTreeBackend::Type BookmarkTreeBackend::type3() const
{
   return m_Type3;
}

void BookmarkModel::reloadCategories()
{
   beginResetModel();
   m_hCategories.clear();
   foreach(TopLevelItem* item, m_lCategoryCounter) {
      foreach (NumberTreeBackend* child, item->m_lChilds) {
         delete child;
      }
      delete item;
   }
   m_lCategoryCounter.clear();
   m_isContactDateInit = false;
   foreach(QString bookmark, ConfigurationSkeleton::bookmarkList()) {
      NumberTreeBackend* bm = new NumberTreeBackend(bookmark);
      QString val = category(bm);
      if (!m_hCategories[val]) {
         TopLevelItem* item = new TopLevelItem(val);
         m_hCategories[val] = item;
         m_lCategoryCounter << item;
      }
      TopLevelItem* item = m_hCategories[val];
      if (item) {
         item->m_lChilds << bm;
      }
      else
         qDebug() << "ERROR count";
   }
   endResetModel();
   emit layoutChanged();
   emit dataChanged(index(0,0),index(rowCount()-1,0));
}

bool BookmarkModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

QVariant BookmarkModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();

   BookmarkTreeBackend* modelItem = (BookmarkTreeBackend*)index.internalPointer();
   switch (modelItem->type3()) {
      case BookmarkTreeBackend::Type::TOP_LEVEL:
         switch (role) {
            case Qt::DisplayRole:
               return ((TopLevelItem*)modelItem)->m_Name;
         }
         break;
      case BookmarkTreeBackend::Type::NUMBER:
         if (role == Qt::DisplayRole)
            return commonCallInfo(m_lCategoryCounter[index.parent().row()]->m_lChilds[index.row()],role);
   };
   return QVariant();
}

QVariant BookmarkModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   Q_UNUSED(section)
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
      return QVariant("Contacts");
   return QVariant();
}

int BookmarkModel::rowCount( const QModelIndex& parent ) const
{
   if (!parent.isValid() || !parent.internalPointer())
      return m_lCategoryCounter.size();
   else
      return m_lCategoryCounter[parent.row()]->m_lChilds.size();
   return 0;
}

Qt::ItemFlags BookmarkModel::flags( const QModelIndex& index ) const
{
   if (!index.isValid())
      return 0;
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int BookmarkModel::columnCount ( const QModelIndex& parent) const
{
   Q_UNUSED(parent)
   return 1;
}

QModelIndex BookmarkModel::parent( const QModelIndex& index) const
{
   if (!index.isValid() || !index.internalPointer()) {
      return QModelIndex();
   }
   BookmarkTreeBackend* modelItem = static_cast<BookmarkTreeBackend*>(index.internalPointer());
   if (modelItem->type3() == BookmarkTreeBackend::Type::NUMBER) {
      QString val = category(((NumberTreeBackend*)(index.internalPointer())));
      if (m_hCategories[val])
         return BookmarkModel::index(m_lCategoryCounter.indexOf(m_hCategories[val]),0);
   }
   return QModelIndex();
}

QModelIndex BookmarkModel::index( int row, int column, const QModelIndex& parent) const
{
   if (parent.isValid())
      return createIndex(row,column,m_lCategoryCounter[parent.row()]->m_lChilds[row]);
   else {
      return createIndex(row,column,m_lCategoryCounter[row]);
   }
   return QModelIndex();
}

QVariant BookmarkModel::commonCallInfo(NumberTreeBackend* call, int role) const
{
   if (!call)
      return QVariant();
   QVariant cat = call->m_Name;
   switch (role) {
      case Qt::DisplayRole:
         cat = call->m_Name;
         break;
   }
   return cat;
}

QString BookmarkModel::category(NumberTreeBackend* number) const
{
   QString cat = commonCallInfo(number).toString();
   if (cat.size())
      cat = cat[0].toUpper();
   return cat;
}
