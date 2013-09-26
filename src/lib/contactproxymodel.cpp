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
#include "contactproxymodel.h"

//Qt
#include <QtCore/QDebug>
#include <QtCore/QDate>
#include <QtCore/QMimeData>
#include <QtCore/QCoreApplication>

//SFLPhone
#include "abstractcontactbackend.h"
#include "callmodel.h"
#include "historymodel.h"
#include "phonenumber.h"
#include "phonedirectorymodel.h"
#include "historytimecategorymodel.h"

const char* ContactProxyModel::m_slHistoryConstStr[25] = {
      "Today"                                                    ,//0
      "Yesterday"                                                ,//1
      QDate::currentDate().addDays(-2).toString("dddd").toAscii().constData(),//2
      QDate::currentDate().addDays(-3).toString("dddd").toAscii().constData(),//3
      QDate::currentDate().addDays(-4).toString("dddd").toAscii().constData(),//4
      QDate::currentDate().addDays(-5).toString("dddd").toAscii().constData(),//5
      QDate::currentDate().addDays(-6).toString("dddd").toAscii().constData(),//6
      "Last week"                                                ,//7
      "Two weeks ago"                                            ,//8
      "Three weeks ago"                                          ,//9
      "Last month"                                               ,//10
      "Two months ago"                                           ,//11
      "Three months ago"                                         ,//12
      "Four months ago"                                          ,//13
      "Five months ago"                                          ,//14
      "Six months ago"                                           ,//15
      "Seven months ago"                                         ,//16
      "Eight months ago"                                         ,//17
      "Nine months ago"                                          ,//18
      "Ten months ago"                                           ,//19
      "Eleven months ago"                                        ,//20
      "Twelve months ago"                                        ,//21
      "Last year"                                                ,//22
      "Very long time ago"                                       ,//23
      "Never"                                                     //24
};

QObject* ContactProxyModel::TopLevelItem::getSelf() 
{
   return this;
}

//
ContactProxyModel::ContactProxyModel(AbstractContactBackend* parent,int role, bool showAll) : QAbstractItemModel(QCoreApplication::instance()),
m_pModel(parent),m_Role(role),m_ShowAll(showAll)
{
   setObjectName("ContactProxyModel");
   m_lMimes << MIME_PLAIN_TEXT << MIME_PHONENUMBER;
   connect(m_pModel,SIGNAL(collectionChanged()),this,SLOT(reloadCategories()));
   QHash<int, QByteArray> roles = roleNames();
   roles.insert(AbstractContactBackend::Role::Organization      ,QByteArray("organization")     );
   roles.insert(AbstractContactBackend::Role::Group             ,QByteArray("group")            );
   roles.insert(AbstractContactBackend::Role::Department        ,QByteArray("department")       );
   roles.insert(AbstractContactBackend::Role::PreferredEmail    ,QByteArray("preferredEmail")   );
   roles.insert(AbstractContactBackend::Role::FormattedLastUsed ,QByteArray("formattedLastUsed"));
   roles.insert(AbstractContactBackend::Role::IndexedLastUsed   ,QByteArray("indexedLastUsed")  );
   roles.insert(AbstractContactBackend::Role::DatedLastUsed     ,QByteArray("datedLastUsed")    );
   roles.insert(AbstractContactBackend::Role::Filter            ,QByteArray("filter")           );
   roles.insert(AbstractContactBackend::Role::DropState         ,QByteArray("dropState")        );
   setRoleNames(roles);
}

ContactProxyModel::~ContactProxyModel()
{
   foreach(TopLevelItem* item,m_lCategoryCounter) {
      delete item;
   }
}

void ContactProxyModel::reloadCategories()
{
   beginResetModel();
   m_hCategories.clear();
   foreach(TopLevelItem* item,m_lCategoryCounter) {
      delete item;
   }
   m_lCategoryCounter.clear();
   foreach(Contact* cont, m_pModel->getContactList()) {
      QString val = category(cont);
      if (!m_hCategories[val]) {
         TopLevelItem* item = new TopLevelItem(val);
         m_hCategories[val] = item;
         m_lCategoryCounter << item;
         emit dataChanged(index(0,0),index(rowCount()-1,0));
      }
      TopLevelItem* item = m_hCategories[val];
      if (item) {
         QModelIndex parent = index(m_lCategoryCounter.indexOf(item),0);
         item->m_lChildren << cont;
      }
      else
         qDebug() << "ERROR count";
   }
   endResetModel();
   emit layoutChanged();
   emit dataChanged(index(0,0),index(rowCount()-1,0));
}

bool ContactProxyModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   if (index.isValid() && index.parent().isValid()) {
      CategorizedCompositeNode* modelItem = (CategorizedCompositeNode*)index.internalPointer();
      if (role == AbstractContactBackend::Role::DropState) {
         modelItem->setDropState(value.toInt());
         emit dataChanged(index, index);
      }
   }
   return false;
}

QVariant ContactProxyModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();

   CategorizedCompositeNode* modelItem = (CategorizedCompositeNode*)index.internalPointer();
   switch (modelItem->type()) {
      case CategorizedCompositeNode::Type::TOP_LEVEL: /*|| !index.parent().isValid()) {*/
      switch (role) {
         case Qt::DisplayRole:
            return static_cast<const TopLevelItem*>(modelItem)->m_Name;
         default:
            break;
      }
      break;
   case CategorizedCompositeNode::Type::CONTACT:{ /* && (role == Qt::DisplayRole)) {*/
      const Contact* c = m_lCategoryCounter[index.parent().row()]->m_lChildren[index.row()];
      switch (role) {
         case Qt::DisplayRole:
            return QVariant(c->formattedName());
         case AbstractContactBackend::Role::Organization:
            return QVariant(c->organization());
         case AbstractContactBackend::Role::Group:
            return QVariant(c->group());
         case AbstractContactBackend::Role::Department:
            return QVariant(c->department());
         case AbstractContactBackend::Role::PreferredEmail:
            return QVariant(c->preferredEmail());
         case AbstractContactBackend::Role::DropState:
            return QVariant(modelItem->dropState());
         case AbstractContactBackend::Role::FormattedLastUsed:
            return QVariant(HistoryTimeCategoryModel::timeToHistoryCategory(c->phoneNumbers().lastUsedTimeStamp()));
         case AbstractContactBackend::Role::IndexedLastUsed:
            return QVariant((int)HistoryTimeCategoryModel::timeToHistoryConst(c->phoneNumbers().lastUsedTimeStamp()));
         case AbstractContactBackend::Role::DatedLastUsed:
            return QVariant(QDateTime::fromTime_t( c->phoneNumbers().lastUsedTimeStamp()));
         case AbstractContactBackend::Role::Filter: {
            QString normStripppedC;
            foreach(QChar char2,QString(c->formattedName()+'\n'+c->organization()+'\n'+c->group()+'\n'+
               c->department()+'\n'+c->preferredEmail()).toLower().normalized(QString::NormalizationForm_KD) ) {
               if (!char2.combiningClass())
                  normStripppedC += char2;
            }
            return normStripppedC;
         }
         default:
            break;
      }
      break;
   }
   case CategorizedCompositeNode::Type::NUMBER: /* && (role == Qt::DisplayRole)) {*/
      switch (role) {
         case Qt::DisplayRole:
            return QVariant(m_lCategoryCounter[index.parent().parent().row()]->m_lChildren[index.parent().row()]->phoneNumbers()[index.row()]->uri());
      }
      break;
   default:
   case CategorizedCompositeNode::Type::CALL:
   case CategorizedCompositeNode::Type::BOOKMARK:
      break;
   };
   return QVariant();
}

QVariant ContactProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   Q_UNUSED(section)
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
      return QVariant("Contacts");
   return QVariant();
}

bool ContactProxyModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
   Q_UNUSED( data   )
   Q_UNUSED( row    )
   Q_UNUSED( column )
   Q_UNUSED( action )
   setData(parent,-1,Call::Role::DropState);
   return false;
}


int ContactProxyModel::rowCount( const QModelIndex& parent ) const
{
   if (!parent.isValid() || !parent.internalPointer())
      return m_lCategoryCounter.size();
   else if (!parent.parent().isValid()) {
      return m_lCategoryCounter[parent.row()]->m_lChildren.size();
   }
   else if (parent.parent().isValid() && !parent.parent().parent().isValid()) {
      const int size = m_lCategoryCounter[parent.parent().row()]->m_lChildren[parent.row()]->phoneNumbers().size();
      return size==1?0:size;
   }
   return 0;
}

Qt::ItemFlags ContactProxyModel::flags( const QModelIndex& index ) const
{
   if (!index.isValid())
      return Qt::NoItemFlags;
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable | (index.parent().isValid()?Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled:Qt::ItemIsEnabled);
}

int ContactProxyModel::columnCount ( const QModelIndex& parent) const
{
   Q_UNUSED(parent)
   return 1;
}

QModelIndex ContactProxyModel::parent( const QModelIndex& index) const
{
   if (!index.isValid() || !index.internalPointer()) {
      return QModelIndex();
   }
   CategorizedCompositeNode* modelItem = static_cast<CategorizedCompositeNode*>(index.internalPointer());
   if (modelItem && (long long)modelItem > 100 && modelItem->type() == CategorizedCompositeNode::Type::CONTACT) {
      Contact* ct = (Contact*)((CategorizedCompositeNode*)(index.internalPointer()))->getSelf();
      const QString val = category(ct);
      if (m_hCategories[val])
         return ContactProxyModel::index(m_lCategoryCounter.indexOf(m_hCategories[val]),0);
   }
   else if (modelItem && modelItem->type() == CategorizedCompositeNode::Type::NUMBER) {
      Contact* ct = static_cast<Contact*>(modelItem->getSelf());
      const QString val = category(ct);
      if (m_hCategories[val]) {
         return ContactProxyModel::index(
            (m_hCategories[val]->m_lChildren.indexOf(ct)),
            0,
            ContactProxyModel::index(m_lCategoryCounter.indexOf(m_hCategories[val]),0));
      }
   }
   else if (modelItem && modelItem->type() == CategorizedCompositeNode::Type::TOP_LEVEL) {
      return QModelIndex();
   }
   return QModelIndex();
}

QModelIndex ContactProxyModel::index( int row, int column, const QModelIndex& parent) const
{
   if (!parent.isValid()) {
      return createIndex(row,column,m_lCategoryCounter[row]);
   }
   else if (!parent.parent().isValid() && column < m_lCategoryCounter[parent.row()]->m_lChildren.size() ) {
      return createIndex(row,column,(void*)dynamic_cast<CategorizedCompositeNode*>(m_lCategoryCounter[parent.row()]->m_lChildren[row]));
   }
   else if (parent.parent().isValid() && m_lCategoryCounter.size() > parent.parent().row() && m_lCategoryCounter[parent.parent().row()]->m_lChildren.size() > parent.row()) {
      return createIndex(row,column,(void*)&m_lCategoryCounter[parent.parent().row()]->m_lChildren[parent.row()]->phoneNumbers());
   }
   return QModelIndex();
}

QStringList ContactProxyModel::mimeTypes() const
{
   return m_lMimes;
}

QMimeData* ContactProxyModel::mimeData(const QModelIndexList &indexes) const
{
   QMimeData *mimeData = new QMimeData();
   foreach (const QModelIndex &index, indexes) {
      if (index.isValid()) {
         if (index.parent().parent().isValid()) {
            //Phone number
            QString text = data(index, Qt::DisplayRole).toString();
            mimeData->setData(MIME_PLAIN_TEXT , text.toUtf8());
            mimeData->setData(MIME_PHONENUMBER, text.toUtf8());
            return mimeData;
         }
         else if (index.parent().isValid()) {
            //Contact
            Contact* ct = m_lCategoryCounter[index.parent().row()]->m_lChildren[index.row()];
            if (ct) {
               if (ct->phoneNumbers().size() == 1) {
                  mimeData->setData(MIME_PHONENUMBER , ct->phoneNumbers()[0]->uri().toUtf8());
               }
               mimeData->setData(MIME_CONTACT , ct->uid().toUtf8());
            }
            return mimeData;
         }
      }
   }
   return mimeData;
}

///Return valid payload types
int ContactProxyModel::acceptedPayloadTypes()
{
   return CallModel::DropPayloadType::CALL;
}



/*****************************************************************************
 *                                                                           *
 *                                  Helpers                                  *
 *                                                                           *
 ****************************************************************************/


QString ContactProxyModel::category(Contact* ct) const {
   QString cat;
   switch (m_Role) {
      case AbstractContactBackend::Role::Organization:
         cat = ct->organization();
         break;
      case AbstractContactBackend::Role::Group:
         cat = ct->group();
         break;
      case AbstractContactBackend::Role::Department:
         cat = ct->department();
         break;
      case AbstractContactBackend::Role::PreferredEmail:
         cat = ct->preferredEmail();
         break;
      case AbstractContactBackend::Role::FormattedLastUsed:
         cat = HistoryTimeCategoryModel::timeToHistoryCategory(ct->phoneNumbers().lastUsedTimeStamp());
         break;
      case AbstractContactBackend::Role::IndexedLastUsed:
         cat = QString::number((int)HistoryTimeCategoryModel::timeToHistoryConst(ct->phoneNumbers().lastUsedTimeStamp()));
         break;
      case AbstractContactBackend::Role::DatedLastUsed:
         cat = QDateTime::fromTime_t(ct->phoneNumbers().lastUsedTimeStamp()).toString();
         break;
      default:
         cat = ct->formattedName();
   }
   if (cat.size() && !m_ShowAll)
      cat = cat[0].toUpper();
   return cat;
}

///Return the list of contact from history (in order, most recently used first)
// QHash<Contact*, time_t> ContactProxyModel::getContactListByTime() const
// {
//    const CallMap& history= HistoryModel::getHistory();
//    QHash<Contact*, time_t> toReturn;
//    QSet<QString> alreadyUsed;
//    QMapIterator<uint, Call*> i(history);
//    i.toBack();
//    while (i.hasPrevious()) { //Iterate from the end up
//       i.previous();
//       (alreadyUsed.find(i.value()->peerPhoneNumber()) == alreadyUsed.constEnd()); //Don't ask, leave it there Elv13(2012)
//       if (alreadyUsed.find(i.value()->peerPhoneNumber()) == alreadyUsed.constEnd()) {
//          Contact* contact = i.value()->contact();
//          if (contact && toReturn.find(contact) == toReturn.end()) {
//             toReturn[contact] = i.value()->startTimeStamp();
//          }
//          alreadyUsed << i.value()->peerPhoneNumber();
//       }
//    }
//    return toReturn;
// } //getContactListByTime


void ContactProxyModel::setRole(int role)
{
   if (role != m_Role) {
      m_Role = role;
      reloadCategories();
   }
}

void ContactProxyModel::setShowAll(bool showAll)
{
   if (showAll != m_ShowAll) {
      m_ShowAll = showAll;
      reloadCategories();
   }
}
