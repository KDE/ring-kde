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
#ifndef CONTACT_PROXY_MODEL_H
#define CONTACT_PROXY_MODEL_H

#include <QtCore/QHash>
#include <QtCore/QStringList>
#include <QtCore/QAbstractItemModel>

//SFLPhone
#include "../lib/typedefs.h"
#include "../lib/contact.h"
class AbstractContactBackend;

class LIB_EXPORT ContactProxyModel :  public QAbstractItemModel
{
   IGNORE_NULL(Q_OBJECT)
public:
   friend class AbstractContactBackend;
   explicit ContactProxyModel(AbstractContactBackend* parent,int role = Qt::DisplayRole, bool showAll = false);

   //Setters
   void setRole(int role);
   void setShowAll(bool showAll);

   //Model implementation
   virtual bool          setData     ( const QModelIndex& index, const QVariant &value, int role   );
   virtual QVariant      data        ( const QModelIndex& index, int role = Qt::DisplayRole        ) const;
   virtual int           rowCount    ( const QModelIndex& parent = QModelIndex()                   ) const;
   virtual Qt::ItemFlags flags       ( const QModelIndex& index                                    ) const;
   virtual int           columnCount ( const QModelIndex& parent = QModelIndex()                   ) const;
   virtual QModelIndex   parent      ( const QModelIndex& index                                    ) const;
   virtual QModelIndex   index       ( int row, int column, const QModelIndex& parent=QModelIndex()) const;
   virtual QVariant      headerData  ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
   virtual QStringList   mimeTypes   (                                                             ) const;
   virtual QMimeData*    mimeData    ( const QModelIndexList &indexes                              ) const;
   virtual bool dropMimeData         ( const QMimeData*, Qt::DropAction, int, int, const QModelIndex& );

private:
   class TopLevelItem : public ContactTreeBackend,public QObject {
   friend class ContactProxyModel;
   public:
      virtual QObject* self() {return this;}
   private:
      explicit TopLevelItem(QString name) : ContactTreeBackend(ContactTreeBackend::TOP_LEVEL),QObject(nullptr),m_Name(name) {}
      QList<Contact*> m_lChilds;
      QString m_Name;
   };

   virtual ~ContactProxyModel();

   QModelIndex getContactIndex(Contact* ct) const;

   //Helpers
   QString category(Contact* ct) const;
   QHash<Contact*, time_t> getContactListByTime() const;

   //Attributes
   QHash<Contact*, time_t>      m_hContactByDate       ;
   AbstractContactBackend*      m_pModel               ;
   QList<TopLevelItem*>         m_lCategoryCounter     ;
   QHash<QString,TopLevelItem*> m_hCategories          ;
   int                          m_Role                 ;
   bool                         m_ShowAll              ;
   const static char*           m_slHistoryConstStr[25];
   bool                         m_isContactDateInit    ;
   QStringList                  m_lMimes               ;

private Q_SLOTS:
   void reloadCategories();
};

#endif
