/****************************************************************************
 *   Copyright (C) 2012-2013 by Savoir-Faire Linux                          *
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
#include "videocodecmodel.h"
#include "call.h"
#include "account.h"
#include "video_interface_singleton.h"

///Get data from the model
QVariant VideoCodecModel::data( const QModelIndex& idx, int role) const
{
   if(idx.column() == 0 && role == Qt::DisplayRole)
      return QVariant(m_lCodecs[idx.row()]->getName());
   else if(idx.column() == 0 && role == Qt::CheckStateRole) {
      return QVariant(m_lCodecs[idx.row()]->getEnabled()?Qt::Checked:Qt::Unchecked);
   }
   else if (idx.column() == 0 && role == VideoCodecModel::BITRATE_ROLE)
      return QVariant(m_lCodecs[idx.row()]->getBitrate());
   return QVariant();
}

///The number of codec
int VideoCodecModel::rowCount( const QModelIndex& par ) const
{
   Q_UNUSED(par)
   return m_lCodecs.size();
}

///Items flag
Qt::ItemFlags VideoCodecModel::flags( const QModelIndex& idx ) const
{
   if (idx.column() == 0)
      return QAbstractItemModel::flags(idx) | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   return QAbstractItemModel::flags(idx);
}

///Set the codec data (codecs can't be added or removed that way)
bool VideoCodecModel::setData(const QModelIndex& idx, const QVariant &value, int role)
{

   if (idx.column() == 0 && role == Qt::CheckStateRole) {
      bool changed = m_lCodecs[idx.row()]->getEnabled() != (value == Qt::Checked);
      m_lCodecs[idx.row()]->setEnabled(value == Qt::Checked);
      if (changed)
         emit dataChanged(idx, idx);
      return true;
   }
   else if (idx.column() == 0 && role == VideoCodecModel::BITRATE_ROLE) {
      bool changed = m_lCodecs[idx.row()]->getBitrate() != value.toUInt();
      m_lCodecs[idx.row()]->setBitrate(value.toInt());
      if (changed)
         emit dataChanged(idx, idx);
      return true;
   }
   return false;
}

///Constructor
VideoCodecModel::VideoCodecModel(Account* account) : QAbstractListModel(),m_pAccount(account)
{
   reload();
}

///Force a model reload from dbus
void VideoCodecModel::reload()
{
   m_lCodecs.clear();
   VideoInterface& interface = VideoInterfaceSingleton::getInstance();
   const VectorMapStringString codecs =  interface.getCodecs(m_pAccount->getAccountId());
   foreach(const MapStringString& h,codecs) {
      VideoCodec* c = new VideoCodec(h["name"],h["bitrate"].toInt(),h["enabled"]=="true");
      m_lCodecs << c;
   }
   emit dataChanged(index(0,0), index(m_lCodecs.size()-1,0));
}

///Save the current model over dbus
void VideoCodecModel::save()
{
   VideoInterface& interface = VideoInterfaceSingleton::getInstance();
   VectorMapStringString toSave;
   foreach(VideoCodec* vc,m_lCodecs) {
      MapStringString details;
      details[ "name"    ] = vc->getName   ();
      details[ "bitrate" ] = QString::number(vc->getBitrate());
      details[ "enabled" ] = vc->getEnabled()?"true":"false";
      toSave << details;
   }
   interface.setCodecs(m_pAccount->getAccountId(),toSave);
}

///Increase codec priority
bool VideoCodecModel::moveUp(QModelIndex idx)
{
   if(idx.row() > 0 && idx.row() <= rowCount()) {
      VideoCodec* data2 = m_lCodecs[idx.row()];
      m_lCodecs.removeAt(idx.row());
      m_lCodecs.insert(idx.row() - 1, data2);
      emit dataChanged(index(idx.row() - 1, 0, QModelIndex()), index(idx.row(), 0, QModelIndex()));
      return true;
   }
   return false;
}

///Decrease codec priority
bool VideoCodecModel::moveDown(QModelIndex idx)
{
   if(idx.row() >= 0 && idx.row() < rowCount()) {
      VideoCodec* data2 = m_lCodecs[idx.row()];
      m_lCodecs.removeAt(idx.row());
      m_lCodecs.insert(idx.row() + 1, data2);
      emit dataChanged(index(idx.row(), 0, QModelIndex()), index(idx.row() + 1, 0, QModelIndex()));
      return true;
   }
   return false;
}


QHash<QString,VideoCodec*> VideoCodec::m_slCodecs;
bool VideoCodec::m_sInit = false;

///Private constructor
VideoCodec::VideoCodec(QString codecName, uint bitRate, bool enabled) :
m_Name(codecName),m_Bitrate(bitRate),m_Enabled(enabled)
{

}

///Get the current codec name
QString VideoCodec::getName() const
{
   return m_Name;
}

///Get the current codec id
uint VideoCodec::getBitrate() const
{
   return m_Bitrate;
}

///Get the current codec id
bool VideoCodec::getEnabled() const
{
   return m_Enabled;
}

///Set the codec bitrate
void VideoCodec::setBitrate(const uint bitrate)
{
   m_Bitrate = bitrate;
}

///Set if the codec is enabled
void VideoCodec::setEnabled(const bool enabled)
{
   m_Enabled = enabled;
}
