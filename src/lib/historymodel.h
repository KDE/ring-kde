/****************************************************************************
 *   Copyright (C) 2012-2014 by Savoir-Faire Linux                          *
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
#ifndef HISTORY_MODEL_H
#define HISTORY_MODEL_H
//Base
#include "typedefs.h"
#include <QtCore/QObject>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QStringList>

//Qt

//SFLPhone
#include "call.h"
#include "commonbackendmanagerinterface.h"

//Typedef
typedef QMap<uint, Call*>  CallMap;
typedef QList<Call*>       CallList;

class HistoryItemNode;
class AbstractHistoryBackend;
class HistoryModelPrivate;
//TODO split ASAP
///HistoryModel: History call manager
class LIB_EXPORT HistoryModel : public QAbstractItemModel, public CommonBackendManagerInterface<AbstractHistoryBackend> {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:
   friend class HistoryItemNode;
   friend class HistoryTopLevelItem;
   friend class TopLevelItem;

   //Properties
   Q_PROPERTY(bool hasBackends   READ hasBackends  )

   //Singleton
   static HistoryModel* instance();

   //Getters
   int  acceptedPayloadTypes       () const;
   bool isHistoryLimited           () const;
   int  historyLimit               () const;
   const CallMap getHistoryCalls   () const;
   virtual bool hasBackends        () const override;
   virtual bool hasEnabledBackends () const override;
   virtual const QVector<AbstractHistoryBackend*> backends() const override;
   virtual const QVector<AbstractHistoryBackend*> enabledBackends() const override;
   virtual CommonItemBackendModel* backendModel() const override;

   //Setters
   void setCategoryRole(Call::Role role);
   void setHistoryLimited(bool isLimited);
   void setHistoryLimit(int numberOfDays);

   //Mutator
   void addBackend(AbstractHistoryBackend* backend, LoadOptions options = LoadOptions::NONE);
   void clearAllBackends() const;
   virtual bool enableBackend(AbstractHistoryBackend* backend, bool enabled);

   //Model implementation
   virtual bool          setData     ( const QModelIndex& index, const QVariant &value, int role   ) override;
   virtual QVariant      data        ( const QModelIndex& index, int role = Qt::DisplayRole        ) const override;
   virtual int           rowCount    ( const QModelIndex& parent = QModelIndex()                   ) const override;
   virtual Qt::ItemFlags flags       ( const QModelIndex& index                                    ) const override;
   virtual int           columnCount ( const QModelIndex& parent = QModelIndex()                   ) const  override;
   virtual QModelIndex   parent      ( const QModelIndex& index                                    ) const override;
   virtual QModelIndex   index       ( int row, int column, const QModelIndex& parent=QModelIndex()) const override;
   virtual QVariant      headerData  ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;
   virtual QStringList   mimeTypes   (                                                             ) const override;
   virtual QMimeData*    mimeData    ( const QModelIndexList &indexes                              ) const override;
   virtual bool          dropMimeData( const QMimeData*, Qt::DropAction, int, int, const QModelIndex& ) override;
   virtual bool          insertRows  ( int row, int count, const QModelIndex & parent = QModelIndex() ) override;


private:
   //Constructor
   explicit HistoryModel();
   ~HistoryModel();
   QScopedPointer<HistoryModelPrivate> d_ptr;
   Q_DECLARE_PRIVATE(HistoryModel)

   //Static attributes
   static HistoryModel* m_spInstance;

public Q_SLOTS:
   void add(Call* call);

Q_SIGNALS:
   ///Emitted when the history change (new items, cleared)
   void historyChanged          (            );
   ///Emitted when a new item is added to prevent full reload
   void newHistoryCall          ( Call* call );
   void newBackendAdded(AbstractHistoryBackend* backend);
};

#endif
