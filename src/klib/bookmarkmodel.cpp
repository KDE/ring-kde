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

//Qt
#include <QtCore/QMimeData>

//SFLPhone
#include "configurationskeleton.h"
#include "../lib/historymodel.h"

BookmarkModel* BookmarkModel::m_pSelf = nullptr;

//Model item/index
class NumberTreeBackend : public HistoryTreeBackend, public QObject
{
   friend class BookmarkModel;
   public:
      NumberTreeBackend(QString name): HistoryTreeBackend(HistoryTreeBackend::Type::BOOKMARK),m_Name(name),m_IsMostPopular(false){}
      virtual QObject* getSelf() { return this; }

   private:
      QString m_Name;
      bool m_IsMostPopular;
};

BookmarkModel::BookmarkModel(QObject* parent) : QAbstractItemModel(parent){
      setObjectName("BookmarkModel");
      reloadCategories();
      m_lMimes << MIME_PLAIN_TEXT << MIME_PHONENUMBER;
   }

BookmarkModel* BookmarkModel::getInstance()
{
   if (!m_pSelf)
      m_pSelf = new BookmarkModel(nullptr);
   return m_pSelf;
}

///Reload bookmark cateogries
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

   //Load most used contacts
   if (ConfigurationSkeleton::displayContactCallHistory()) {
      TopLevelItem* item = new TopLevelItem("Most popular");
      m_hCategories["mp"] = item;
      m_lCategoryCounter << item;
      QStringList cl = HistoryModel::getNumbersByPopularity();
      for (int i=0;i<((cl.size()>=10)?10:cl.size());i++) {
         NumberTreeBackend* bm = new NumberTreeBackend(cl[i]);
         bm->m_IsMostPopular = true;
         item->m_lChilds << bm;
      }
   }

   foreach(const QString& bookmark, ConfigurationSkeleton::bookmarkList()) {
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
} //reloadCategories

//Do nothing
bool BookmarkModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

///Get bookmark model data HistoryTreeBackend::Type and Call::Role
QVariant BookmarkModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();

   HistoryTreeBackend* modelItem = static_cast<HistoryTreeBackend*>(index.internalPointer());
   if (!modelItem)
      return QVariant();
   switch (modelItem->type3()) {
      case HistoryTreeBackend::Type::TOP_LEVEL:
         switch (role) {
            case Qt::DisplayRole:
               return ((TopLevelItem*)modelItem)->m_Name;
         }
         break;
      case HistoryTreeBackend::Type::CALL:
      case HistoryTreeBackend::Type::BOOKMARK:
         return commonCallInfo(m_lCategoryCounter[index.parent().row()]->m_lChilds[index.row()],role);
         break;
      case HistoryTreeBackend::Type::NUMBER:
         break;
   };
   return QVariant();
} //Data

///Get header data
QVariant BookmarkModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   Q_UNUSED(section)
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
      return QVariant("Contacts");
   return QVariant();
}


///Get the number of child of "parent"
int BookmarkModel::rowCount( const QModelIndex& parent ) const
{
   if (!parent.isValid() || !parent.internalPointer())
      return m_lCategoryCounter.size();
   else if (!parent.parent().isValid()) {
      return m_lCategoryCounter[parent.row()]->m_lChilds.size();
   }
   return 0;
}

Qt::ItemFlags BookmarkModel::flags( const QModelIndex& index ) const
{
   if (!index.isValid())
      return 0;
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable | (index.parent().isValid()?Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled:Qt::ItemIsEnabled);
}

///There is only 1 column
int BookmarkModel::columnCount ( const QModelIndex& parent) const
{
   Q_UNUSED(parent)
   return 1;
}

///Get the bookmark parent
QModelIndex BookmarkModel::parent( const QModelIndex& index) const
{
   if (!index.isValid() || !index.internalPointer()) {
      return QModelIndex();
   }
   const HistoryTreeBackend* modelItem = static_cast<HistoryTreeBackend*>(index.internalPointer());
   if (modelItem->type3() == HistoryTreeBackend::Type::BOOKMARK) {
      QString val = category(((NumberTreeBackend*)(index.internalPointer())));
      if (((NumberTreeBackend*)modelItem)->m_IsMostPopular)
         return BookmarkModel::index(0,0);
      else if (m_hCategories[val])
         return BookmarkModel::index(m_lCategoryCounter.indexOf(m_hCategories[val]),0);
      else BookmarkModel::index(0,0);
   }
   return QModelIndex();
} //parent

///Get the index
QModelIndex BookmarkModel::index(int row, int column, const QModelIndex& parent) const
{
   if (parent.isValid())
      return createIndex(row,column,m_lCategoryCounter[parent.row()]->m_lChilds[row]);
   else {
      return createIndex(row,column,m_lCategoryCounter[row]);
   }
}

///Get bookmarks mime types
QStringList BookmarkModel::mimeTypes() const
{
   return m_lMimes;
}

///Generate mime data
QMimeData* BookmarkModel::mimeData(const QModelIndexList &indexes) const
{
   QMimeData *mimeData = new QMimeData();
   foreach (const QModelIndex &index, indexes) {
      if (index.isValid()) {
         QString text = data(index, Call::Role::Number).toString();
         mimeData->setData(MIME_PLAIN_TEXT , text.toUtf8());
         mimeData->setData(MIME_PHONENUMBER, text.toUtf8());
         return mimeData;
      }
   }
   return mimeData;
} //mimeData

///Get call info TODO use Call:: one
QVariant BookmarkModel::commonCallInfo(NumberTreeBackend* number, int role) const
{
   if (!number)
      return QVariant();
   QVariant cat;
   switch (role) {
      case Qt::DisplayRole:
      case Call::Role::Name:
         cat = number->m_Name;
         break;
      case Call::Role::Number:
         cat = number->m_Name;//call->getPeerPhoneNumber();
         break;
      case Call::Role::Direction:
         cat = 4;//call->getHistoryState();
         break;
      case Call::Role::Date:
         cat = "N/A";//call->getStartTimeStamp();
         break;
      case Call::Role::Length:
         cat = "N/A";//call->getLength();
         break;
      case Call::Role::FormattedDate:
         cat = "N/A";//QDateTime::fromTime_t(call->getStartTimeStamp().toUInt()).toString();
         break;
      case Call::Role::HasRecording:
         cat = false;//call->hasRecording();
         break;
      case Call::Role::HistoryState:
         cat = history_state::NONE;//call->getHistoryState();
         break;
      case Call::Role::Filter:
         cat = number->m_Name;//call->getHistoryState()+'\n'+commonCallInfo(call,Name).toString()+'\n'+commonCallInfo(call,Number).toString();
         break;
      case Call::Role::FuzzyDate:
         cat = "N/A";//timeToHistoryCategory(QDateTime::fromTime_t(call->getStartTimeStamp().toUInt()).date());
         break;
      case Call::Role::IsBookmark:
         cat = true;
         break;
   }
   return cat;
} //commonCallInfo

///Get category
QString BookmarkModel::category(NumberTreeBackend* number) const
{
   QString cat = commonCallInfo(number).toString();
   if (cat.size())
      cat = cat[0].toUpper();
   return cat;
}
