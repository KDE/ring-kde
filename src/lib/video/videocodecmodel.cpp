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
#include "videocodecmodel.h"
#include <call.h>
#include <account.h>
#include <video/videocodec.h>
#include "../private/videocodec_p.h"
#include "../dbus/videomanager.h"

#include <QtCore/QCoreApplication>

class VideoCodecModelPrivate
{
public:
   VideoCodecModelPrivate();

   //Attrbutes
   QList<VideoCodec*> m_lCodecs;
   Account*           m_pAccount;
};

VideoCodecModelPrivate::VideoCodecModelPrivate() : m_pAccount(nullptr)
{}

///Constructor
VideoCodecModel::VideoCodecModel(Account* account) : QAbstractListModel(QCoreApplication::instance()),d_ptr(new VideoCodecModelPrivate())
{
   d_ptr->m_pAccount = account;
   reload();
}

///Get data from the model
QVariant VideoCodecModel::data( const QModelIndex& idx, int role) const
{
   if(idx.column() == 0 && role == Qt::DisplayRole)
      return QVariant(d_ptr->m_lCodecs[idx.row()]->name());
   else if(idx.column() == 0 && role == Qt::CheckStateRole) {
      return QVariant(d_ptr->m_lCodecs[idx.row()]->isEnabled()?Qt::Checked:Qt::Unchecked);
   }
   else if (idx.column() == 0 && role == VideoCodecModel::BITRATE_ROLE)
      return QVariant(d_ptr->m_lCodecs[idx.row()]->bitrate());
   return QVariant();
}

///The number of codec
int VideoCodecModel::rowCount( const QModelIndex& par ) const
{
   Q_UNUSED(par)
   return d_ptr->m_lCodecs.size();
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
      bool changed = d_ptr->m_lCodecs[idx.row()]->isEnabled() != (value == Qt::Checked);
      d_ptr->m_lCodecs[idx.row()]->setEnabled(value == Qt::Checked);
      if (changed)
         emit dataChanged(idx, idx);
      return true;
   }
   else if (idx.column() == 0 && role == VideoCodecModel::BITRATE_ROLE) {
      bool changed = d_ptr->m_lCodecs[idx.row()]->bitrate() != value.toUInt();
      d_ptr->m_lCodecs[idx.row()]->setBitrate(value.toInt());
      if (changed)
         emit dataChanged(idx, idx);
      return true;
   }
   return false;
}

///Destructor
VideoCodecModel::~VideoCodecModel()
{
   while (d_ptr->m_lCodecs.size()) {
      VideoCodec* c = d_ptr->m_lCodecs[0];
      d_ptr->m_lCodecs.removeAt(0);
      delete c;
   }
   delete d_ptr;
}

///Force a model reload from dbus
void VideoCodecModel::reload()
{
   while (d_ptr->m_lCodecs.size()) {
      VideoCodec* c = d_ptr->m_lCodecs[0];
      d_ptr->m_lCodecs.removeAt(0);
      delete c;
   }
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   const VectorMapStringString codecs =  interface.getCodecs(d_ptr->m_pAccount->id());
   foreach(const MapStringString& h,codecs) {
      VideoCodec* c = new VideoCodec(h[VideoCodecPrivate::CodecFields::NAME],
                                     h[VideoCodecPrivate::CodecFields::BITRATE].toInt(),
                                     h[VideoCodecPrivate::CodecFields::ENABLED]=="true");
      c->setParamaters(h[VideoCodecPrivate::CodecFields::PARAMETERS]);
      d_ptr->m_lCodecs << c;
   }
   emit dataChanged(index(0,0), index(d_ptr->m_lCodecs.size()-1,0));
}

///Save the current model over dbus
void VideoCodecModel::save()
{
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   VectorMapStringString toSave;
   foreach(VideoCodec* vc, d_ptr->m_lCodecs) {
      toSave << vc->toMap();
   }
   interface.setCodecs(d_ptr->m_pAccount->id(),toSave);
}

///Increase codec priority
bool VideoCodecModel::moveUp(QModelIndex idx)
{
   if(idx.row() > 0 && idx.row() <= rowCount()) {
      VideoCodec* data2 = d_ptr->m_lCodecs[idx.row()];
      d_ptr->m_lCodecs.removeAt(idx.row());
      d_ptr->m_lCodecs.insert(idx.row() - 1, data2);
      emit dataChanged(index(idx.row() - 1, 0, QModelIndex()), index(idx.row(), 0, QModelIndex()));
      return true;
   }
   return false;
}

///Decrease codec priority
bool VideoCodecModel::moveDown(QModelIndex idx)
{
   if(idx.row() >= 0 && idx.row() < rowCount()) {
      VideoCodec* data2 = d_ptr->m_lCodecs[idx.row()];
      d_ptr->m_lCodecs.removeAt(idx.row());
      d_ptr->m_lCodecs.insert(idx.row() + 1, data2);
      emit dataChanged(index(idx.row(), 0, QModelIndex()), index(idx.row() + 1, 0, QModelIndex()));
      return true;
   }
   return false;
}
