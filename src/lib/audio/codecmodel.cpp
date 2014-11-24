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
#include "codecmodel.h"

//Qt
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

//SFLPhone
#include "account.h"
#include "dbus/configurationmanager.h"

///Constructor
Audio::CodecModel::CodecModel(Account* account) :
QAbstractListModel(account?(QObject*)account:(QObject*)QCoreApplication::instance()),m_pAccount(account)
{
   setObjectName("CodecModel: "+(account?account->id():"Unknown"));
   QHash<int, QByteArray> roles = roleNames();
   roles.insert(Audio::CodecModel::Role::ID        ,QByteArray("id"));
   roles.insert(Audio::CodecModel::Role::NAME      ,QByteArray("name"));
   roles.insert(Audio::CodecModel::Role::BITRATE   ,QByteArray("bitrate"));
   roles.insert(Audio::CodecModel::Role::SAMPLERATE,QByteArray("samplerate"));
   setRoleNames(roles);
}

Audio::CodecModel::~CodecModel()
{
   while (m_lAudioCodecs.size()) {
      AudioCodecData* c = m_lAudioCodecs[0];
      m_lAudioCodecs.removeAt(0);
      delete c;
   }
}

///Model data
QVariant Audio::CodecModel::data(const QModelIndex& idx, int role) const {
   if(idx.column() == 0      && role == Qt::DisplayRole                   ) {
      return QVariant(m_lAudioCodecs[idx.row()]->name);
   }
   else if(idx.column() == 0 && role == Qt::CheckStateRole                ) {
      return QVariant(m_lEnabledCodecs[m_lAudioCodecs[idx.row()]->id] ? Qt::Checked : Qt::Unchecked);
   }
   else if (idx.column() == 0 && role == Audio::CodecModel::Role::NAME       ) {
      return m_lAudioCodecs[idx.row()]->name;
   }
   else if (idx.column() == 0 && role == Audio::CodecModel::Role::BITRATE    ) {
      return m_lAudioCodecs[idx.row()]->bitrate;
   }
   else if (idx.column() == 0 && role == Audio::CodecModel::Role::SAMPLERATE ) {
      return m_lAudioCodecs[idx.row()]->samplerate;
   }
   else if (idx.column() == 0 && role == Audio::CodecModel::Role::ID         ) {
      return m_lAudioCodecs[idx.row()]->id;
   }
   return QVariant();
}

///Number of audio codecs
int Audio::CodecModel::rowCount(const QModelIndex& par) const {
   Q_UNUSED(par)
   return m_lAudioCodecs.size();
}

///Model flags
Qt::ItemFlags Audio::CodecModel::flags(const QModelIndex& idx) const {
   if (idx.column() == 0)
      return QAbstractItemModel::flags(idx) | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   return QAbstractItemModel::flags(idx);
}

///Set audio codec data
bool Audio::CodecModel::setData( const QModelIndex& idx, const QVariant &value, int role) {
   if (idx.column() == 0 && role == Audio::CodecModel::NAME) {
      m_lAudioCodecs[idx.row()]->name = value.toString();
      emit dataChanged(idx, idx);
      return true;
   }
   else if (idx.column() == 0 && role == Audio::CodecModel::BITRATE) {
      m_lAudioCodecs[idx.row()]->bitrate = value.toString();
      emit dataChanged(idx, idx);
      return true;
   }
   else if(idx.column() == 0 && role == Qt::CheckStateRole) {
      m_lEnabledCodecs[m_lAudioCodecs[idx.row()]->id] = value.toBool();
      emit dataChanged(idx, idx);
      return true;
   }
   else if (idx.column() == 0 && role == Audio::CodecModel::SAMPLERATE) {
      m_lAudioCodecs[idx.row()]->samplerate = value.toString();
      emit dataChanged(idx, idx);
      return true;
   }
   else if (idx.column() == 0 && role == Audio::CodecModel::ID) {
      m_lAudioCodecs[idx.row()]->id = value.toInt();
      emit dataChanged(idx, idx);
      return true;
   }
   return false;
}

///Add a new audio codec
QModelIndex Audio::CodecModel::add() {
   m_lAudioCodecs << new AudioCodecData;
   emit dataChanged(index(m_lAudioCodecs.size()-1,0), index(m_lAudioCodecs.size()-1,0));
   return index(m_lAudioCodecs.size()-1,0);
}

///Remove audio codec at 'idx'
void Audio::CodecModel::remove(const QModelIndex& idx) {
   if (idx.isValid()) {
      AudioCodecData* d = m_lAudioCodecs[idx.row()];
      m_lAudioCodecs.removeAt(idx.row());
      delete d;
      emit dataChanged(idx, index(m_lAudioCodecs.size()-1,0));
   }
   else {
      qDebug() << "Failed to remove an invalid audio codec";
   }
}

///Remove everything
void Audio::CodecModel::clear()
{
   while(m_lAudioCodecs.size()) {
      AudioCodecData* d = m_lAudioCodecs[0];
      m_lAudioCodecs.removeAt(0);
      delete d;
   }
   m_lAudioCodecs.clear  ();
   m_lEnabledCodecs.clear();
}

///Increase codec priority
bool Audio::CodecModel::moveUp(const QModelIndex& idx)
{
   if(idx.row() > 0 && idx.row() <= rowCount()) {
      AudioCodecData* data2 = m_lAudioCodecs[idx.row()];
      m_lAudioCodecs.removeAt(idx.row());
      m_lAudioCodecs.insert(idx.row() - 1, data2);
      emit dataChanged(index(idx.row() - 1, 0, QModelIndex()), index(idx.row(), 0, QModelIndex()));
      return true;
   }
   return false;
}

///Decrease codec priority
bool Audio::CodecModel::moveDown(const QModelIndex& idx)
{
   if(idx.row() >= 0 && idx.row() < rowCount()) {
      AudioCodecData* data2 = m_lAudioCodecs[idx.row()];
      m_lAudioCodecs.removeAt(idx.row());
      m_lAudioCodecs.insert(idx.row() + 1, data2);
      emit dataChanged(index(idx.row(), 0, QModelIndex()), index(idx.row() + 1, 0, QModelIndex()));
      return true;
   }
   return false;
}

///Reload the codeclist
void Audio::CodecModel::reload()
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   QVector<int> codecIdList = configurationManager.getAudioCodecList();
   if (!m_pAccount->isNew()) {
      QVector<int> activeCodecList = configurationManager.getActiveAudioCodecList(m_pAccount->id());
      QStringList tmpNameList;

      foreach (const int aCodec, activeCodecList) {
         if (!findCodec(aCodec)) {
            QStringList codec = configurationManager.getAudioCodecDetails(aCodec);
            QModelIndex idx = add();
            setData(idx,codec[0]     ,Audio::CodecModel::Role::NAME       );
            setData(idx,codec[1]     ,Audio::CodecModel::Role::SAMPLERATE );
            setData(idx,codec[2]     ,Audio::CodecModel::Role::BITRATE    );
            setData(idx,aCodec       ,Audio::CodecModel::Role::ID         );
            setData(idx, Qt::Checked ,Qt::CheckStateRole               );
            if (codecIdList.indexOf(aCodec)!=-1)
               codecIdList.remove(codecIdList.indexOf(aCodec));
         }
      }
   }

   foreach (const int aCodec, codecIdList) {
      if (!findCodec(aCodec)) {
         const QStringList codec = configurationManager.getAudioCodecDetails(aCodec);
         QModelIndex idx = add();
         setData(idx,codec[0],Audio::CodecModel::Role::NAME       );
         setData(idx,codec[1],Audio::CodecModel::Role::SAMPLERATE );
         setData(idx,codec[2],Audio::CodecModel::Role::BITRATE    );
         setData(idx,aCodec  ,Audio::CodecModel::Role::ID         );
         setData(idx, Qt::Unchecked ,Qt::CheckStateRole);
      }
   }
}

///Save details
void Audio::CodecModel::save()
{
   QStringList _codecList;
   for (int i=0; i < rowCount();i++) {
      QModelIndex idx = index(i,0);
      if (data(idx,Qt::CheckStateRole) == Qt::Checked) {
         _codecList << data(idx,Audio::CodecModel::Role::ID).toString();
      }
   }

   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   configurationManager.setActiveAudioCodecList(_codecList, m_pAccount->id());
}

///Check is a codec is already in the list
bool Audio::CodecModel::findCodec(int id)
{
   foreach(AudioCodecData* data, m_lAudioCodecs) {
      if (data->id == id)
         return true;
   }
   return false;
}
