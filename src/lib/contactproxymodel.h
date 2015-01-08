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
#ifndef CONTACT_PROXY_MODEL_H
#define CONTACT_PROXY_MODEL_H

#include <QtCore/QHash>
#include <QtCore/QStringList>
#include <QtCore/QAbstractItemModel>

//Ring
#include "../lib/typedefs.h"
#include "../lib/contact.h"
class ContactModel;
class ContactTreeNode;
class TopLevelItem;
class ContactTreeBinder;
class ContactProxyModelPrivate;

class LIB_EXPORT ContactProxyModel :  public QAbstractItemModel
{
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:
   friend class ContactModel;
   friend class ContactTreeNode;
   friend class ContactTreeBinder;
   explicit ContactProxyModel(int role = Qt::DisplayRole, bool showAll = false);
   virtual ~ContactProxyModel();

   //Setters
   void setRole(int role);
   void setShowAll(bool showAll);

   //Model implementation
   virtual bool          setData     ( const QModelIndex& index, const QVariant &value, int role   ) override;
   virtual QVariant      data        ( const QModelIndex& index, int role = Qt::DisplayRole        ) const override;
   virtual int           rowCount    ( const QModelIndex& parent = QModelIndex()                   ) const override;
   virtual Qt::ItemFlags flags       ( const QModelIndex& index                                    ) const override;
   virtual int           columnCount ( const QModelIndex& parent = QModelIndex()                   ) const override;
   virtual QModelIndex   parent      ( const QModelIndex& index                                    ) const override;
   virtual QModelIndex   index       ( int row, int column, const QModelIndex& parent=QModelIndex()) const override;
   virtual QVariant      headerData  ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;
   virtual QStringList   mimeTypes   (                                                             ) const override;
   virtual QMimeData*    mimeData    ( const QModelIndexList &indexes                              ) const override;
   virtual bool dropMimeData         ( const QMimeData*, Qt::DropAction, int, int, const QModelIndex& ) override;

   //Getter
   static int acceptedPayloadTypes();

private:
   QScopedPointer<ContactProxyModelPrivate> d_ptr;
   Q_DECLARE_PRIVATE(ContactProxyModel)
};

#endif
