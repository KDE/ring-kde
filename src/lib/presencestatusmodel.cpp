/****************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                          *
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
#include "presencestatusmodel.h"

//Qt
#include <QtCore/QCoreApplication>

//SFLPhone
#include "accountmodel.h"
#include "dbus/presencemanager.h"
#include "visitors/presenceserializationvisitor.h"

//Static
PresenceStatusModel* PresenceStatusModel::m_spInstance = nullptr;

class PresenceStatusModelPrivate
{
public:
   PresenceStatusModelPrivate();
   //Attributes
   QVector<PresenceStatusModel::StatusData*> m_lStatuses;
   QString                          m_CustomMessage    ;
   bool                             m_UseCustomStatus  ;
   bool                             m_CustomStatus     ;
   PresenceStatusModel::StatusData* m_pCurrentStatus   ;
   PresenceStatusModel::StatusData* m_pDefaultStatus   ;
};

PresenceStatusModelPrivate::PresenceStatusModelPrivate() :
m_pCurrentStatus(nullptr),m_pDefaultStatus(nullptr),m_UseCustomStatus(false),m_CustomStatus(false)
{
   
}

///Constructor
PresenceStatusModel::PresenceStatusModel(QObject* parent) : QAbstractTableModel(parent?parent:QCoreApplication::instance()),
d_ptr(new PresenceStatusModelPrivate())
{
   setObjectName("PresenceStatusModel");
}

PresenceStatusModel::~PresenceStatusModel()
{
   foreach (StatusData* data, d_ptr->m_lStatuses) {
      delete data;
   }
}

///Get model data
QVariant PresenceStatusModel::data(const QModelIndex& index, int role ) const
{
   if (index.isValid()) {
      switch (static_cast<PresenceStatusModel::Columns>(index.column())) {
         case PresenceStatusModel::Columns::Name:
            switch (role) {
               case Qt::DisplayRole:
               case Qt::EditRole:
                  return d_ptr->m_lStatuses[index.row()]->name;
               case Qt::ToolTipRole:
                  return d_ptr->m_lStatuses[index.row()]->message;
            }
            break;
         case PresenceStatusModel::Columns::Message:
            switch (role) {
               case Qt::DisplayRole:
               case Qt::EditRole:
                  return d_ptr->m_lStatuses[index.row()]->message;
            }
            break;
         case PresenceStatusModel::Columns::Color:
            switch (role) {
               case Qt::BackgroundColorRole:
                  return d_ptr->m_lStatuses[index.row()]->color;
            }
            break;
         case PresenceStatusModel::Columns::Status:
            switch (role) {
               case Qt::CheckStateRole:
                  return d_ptr->m_lStatuses[index.row()]->status?Qt::Checked:Qt::Unchecked;
               case Qt::TextAlignmentRole:
                  return Qt::AlignCenter;
            }
            break;
         case PresenceStatusModel::Columns::Default:
            switch (role) {
               case Qt::CheckStateRole:
                  return d_ptr->m_lStatuses[index.row()]->defaultStatus?Qt::Checked:Qt::Unchecked;
               case Qt::TextAlignmentRole:
                  return Qt::AlignCenter;
            }
            break;
      }
   }
   return QVariant();
}

///Return the number of pre defined status
int PresenceStatusModel::rowCount(const QModelIndex& parent ) const
{
   if (parent.isValid()) return 0;
   return d_ptr->m_lStatuses.size();
}

///Return the number of column (static: {"Name","Message","Color","Present","Default"})
int PresenceStatusModel::columnCount(const QModelIndex& parent ) const
{
   if (parent.isValid()) return 0;
   return 5;
}

///All the items are enabled, selectable and editable
Qt::ItemFlags PresenceStatusModel::flags(const QModelIndex& index ) const
{
   const int col = index.column();
   return Qt::ItemIsEnabled
      | Qt::ItemIsSelectable
      | (col<2||col>=3?Qt::ItemIsEditable:Qt::NoItemFlags)
      | (col>=3?Qt::ItemIsUserCheckable:Qt::NoItemFlags);
}

///Set model data
bool PresenceStatusModel::setData(const QModelIndex& index, const QVariant &value, int role )
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   if (index.isValid()) {
      StatusData* dat = d_ptr->m_lStatuses[index.row()];
      switch(static_cast<PresenceStatusModel::Columns>(index.column())) {
         case PresenceStatusModel::Columns::Name:
            if (role == Qt::EditRole) {
               dat->name = value.toString();
               emit dataChanged(index,index);
               return true;
            }
            break;
         case PresenceStatusModel::Columns::Message:
            if (role == Qt::EditRole) {
               dat->message = value.toString();
               emit dataChanged(index,index);
               return true;
            }
            break;
         case PresenceStatusModel::Columns::Color:
            if (role == Qt::EditRole) {
               
            }
            break;
         case PresenceStatusModel::Columns::Status:
            if (role == Qt::CheckStateRole) {
               dat->status = value.toBool();
               emit dataChanged(index,index);
               return true;
            }
            break;
         case PresenceStatusModel::Columns::Default:
            if (role == Qt::CheckStateRole) {
               dat->defaultStatus = value.toBool();
               setDefaultStatus(index);
               return true;
            }
            break;
      };
   }
   return false;
}

///Return header data
QVariant PresenceStatusModel::headerData(int section, Qt::Orientation orientation, int role ) const
{
   Q_UNUSED(section)
   Q_UNUSED(orientation)
   static const QString rows[] = {tr("Name"),tr("Message"),tr("Color"),tr("Present"),tr("Default")};
   if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
      return QVariant(rows[section]);
   }
   return QVariant();
}

///Add a status to the model
void PresenceStatusModel::addStatus(StatusData* status)
{
   d_ptr->m_lStatuses << status;
   if (status->defaultStatus) {
      d_ptr->m_pDefaultStatus = status;
      if (!d_ptr->m_pCurrentStatus)
         setCurrentIndex(index(d_ptr->m_lStatuses.size()-1,0));
   }
}

///Add a new status
void PresenceStatusModel::addRow()
{
   StatusData* newRow = new StatusData();
   newRow->status = false;
   d_ptr->m_lStatuses << newRow;
   emit layoutChanged();
}

///Remove status[index]
void PresenceStatusModel::removeRow(const QModelIndex& index)
{
   StatusData* toDel = d_ptr->m_lStatuses[index.row()];
   d_ptr->m_lStatuses.remove(index.row());
   emit layoutChanged();
   delete toDel;
}

///Serialize model TODO a backend visitor need to be created
void PresenceStatusModel::save()
{
   PresenceSerializationVisitor::instance()->serialize();
}

///Singleton
PresenceStatusModel* PresenceStatusModel::instance()
{
   if (!m_spInstance) {
      m_spInstance = new PresenceStatusModel();
   }
   return m_spInstance;
}

///Move idx up
void PresenceStatusModel::moveUp(const QModelIndex& idx)
{
   const int row = idx.row();
   if (row > 0) {
      StatusData* tmp      = d_ptr->m_lStatuses[row-1];
      d_ptr->m_lStatuses[ row-1 ] = d_ptr->m_lStatuses[row  ];
      d_ptr->m_lStatuses[ row]    = tmp;
      emit dataChanged(index(row-1,0),index(row,0));
   }
}

///Move idx down
void PresenceStatusModel::moveDown(const QModelIndex& idx)
{
   const int row = idx.row();
   if (row-1 < d_ptr->m_lStatuses.size()) {
      StatusData* tmp      = d_ptr->m_lStatuses[row+1];
      d_ptr->m_lStatuses[ row+1 ] = d_ptr->m_lStatuses[row  ];
      d_ptr->m_lStatuses[ row   ] = tmp;
      emit dataChanged(index(row,0),index(row+1,0));
   }
}

///Return the (user defined) custom message;
QString PresenceStatusModel::customMessage() const
{
   return d_ptr->m_CustomMessage;
}

///Set the (user defined) custom message
void PresenceStatusModel::setCustomMessage(const QString& message)
{
   const bool hasChanged = d_ptr->m_CustomMessage != message;
   d_ptr->m_CustomMessage = message;
   if (hasChanged) {
      emit customMessageChanged(message);
      if (d_ptr->m_UseCustomStatus)
         emit currentMessageChanged(message);
   }
}

///Set the custom status
void PresenceStatusModel::setCustomStatus(bool status)
{
   const bool hasChanged = status != d_ptr->m_CustomStatus;
   d_ptr->m_CustomStatus = status;
   if (hasChanged) {
      emit customStatusChanged(status);
      if (d_ptr->m_UseCustomStatus)
         emit currentStatusChanged(status);
   }
}

///Switch between the pre-defined status list and custom ones
void PresenceStatusModel::setUseCustomStatus(bool useCustom)
{
   const bool changed = d_ptr->m_UseCustomStatus != useCustom;
   d_ptr->m_UseCustomStatus = useCustom;
   if (changed) {
      emit useCustomStatusChanged( useCustom                                                                                                     );
      emit currentIndexChanged   ( useCustom||!d_ptr->m_pCurrentStatus?index(-1,-1):index(d_ptr->m_lStatuses.indexOf(d_ptr->m_pCurrentStatus),0) );
      emit currentNameChanged    ( useCustom?tr("Custom"):(d_ptr->m_pCurrentStatus?d_ptr->m_pCurrentStatus->name:tr("N/A"))                      );
      emit currentStatusChanged  ( useCustom?d_ptr->m_CustomStatus:(d_ptr->m_pCurrentStatus?d_ptr->m_pCurrentStatus->status:false)               );
      emit currentMessageChanged ( useCustom?d_ptr->m_CustomMessage:(d_ptr->m_pCurrentStatus?d_ptr->m_pCurrentStatus->message:tr("N/A"))         );
   }
}

///Return if the presence status is from the predefined list or custom
bool PresenceStatusModel::useCustomStatus() const
{
   return d_ptr->m_UseCustomStatus;
}

///Return the custom status
bool PresenceStatusModel::customStatus() const
{
   return d_ptr->m_CustomStatus;
}

///Set the current status and publish it on the network
void PresenceStatusModel::setCurrentIndex  (const QModelIndex& index)
{
   if (!index.isValid()) return;
   d_ptr->m_pCurrentStatus = d_ptr->m_lStatuses[index.row()];
   emit currentIndexChanged(index);
   emit currentNameChanged(d_ptr->m_pCurrentStatus->name);
   emit currentMessageChanged(d_ptr->m_pCurrentStatus->message);
   emit currentStatusChanged(d_ptr->m_pCurrentStatus->status);
   for (int i=0; i < AccountModel::instance()->size(); i++) {
      DBus::PresenceManager::instance().publish(
         (*AccountModel::instance())[1]->id(), d_ptr->m_pCurrentStatus->status,d_ptr->m_pCurrentStatus->message
      );
   }
}

///Return the current status
bool PresenceStatusModel::currentStatus() const
{
   if (d_ptr->m_UseCustomStatus) return d_ptr->m_CustomStatus;
   if (!d_ptr->m_pCurrentStatus) return false;
   return d_ptr->m_UseCustomStatus?d_ptr->m_CustomStatus:d_ptr->m_pCurrentStatus->status;
}

///Return the current status message
QString PresenceStatusModel::currentMessage() const
{
   if (d_ptr->m_UseCustomStatus) return d_ptr->m_CustomMessage;
   if (!d_ptr->m_pCurrentStatus) return tr("N/A");
   return d_ptr->m_pCurrentStatus->message;
}

///Return current name
QString PresenceStatusModel::currentName() const
{
   return d_ptr->m_UseCustomStatus?tr("Custom"):d_ptr->m_pCurrentStatus?d_ptr->m_pCurrentStatus->name:tr("N/A");
}

///Return the default status index
QModelIndex PresenceStatusModel::defaultStatus() const
{
   if (!d_ptr->m_pDefaultStatus) return index(-1,-1);
   return index(d_ptr->m_lStatuses.indexOf(d_ptr->m_pDefaultStatus),0);
}

///Set the new default status
void PresenceStatusModel::setDefaultStatus( const QModelIndex& idx )
{
   if (!idx.isValid()) return;
   if (d_ptr->m_pDefaultStatus) {
      d_ptr->m_pDefaultStatus->defaultStatus = false;
      const QModelIndex& oldIdx = index(d_ptr->m_lStatuses.indexOf(d_ptr->m_pDefaultStatus),static_cast<int>(PresenceStatusModel::Columns::Default));
      emit dataChanged(oldIdx,oldIdx);
   }
   d_ptr->m_pDefaultStatus = d_ptr->m_lStatuses[idx.row()];
   d_ptr->m_pDefaultStatus->defaultStatus = true;
   emit defaultStatusChanged(idx);
   emit dataChanged(idx,idx);
}

bool PresenceStatusModel::isAutoTracked(AbstractItemBackendBase* backend) const
{
   return PresenceSerializationVisitor::instance()->isTracked(backend);
}

void PresenceStatusModel::setAutoTracked(AbstractItemBackendBase* backend, bool tracked) const
{
   PresenceSerializationVisitor::instance()->setTracked(backend,tracked);
}
