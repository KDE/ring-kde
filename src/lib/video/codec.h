/****************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                          *
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
#ifndef VIDEO_CODEC_H
#define VIDEO_CODEC_H

#include "../typedefs.h"
#include <QtCore/QObject>

class Account;

class VideoCodecPrivate;

namespace Video {


///Codec: Codecs used for video calls
class LIB_EXPORT Codec : public QObject {
   Q_OBJECT
   friend class CodecModel2;
   public:
      //Properties
      Q_PROPERTY(QString name       READ name                          )
      Q_PROPERTY(uint    bitrate    READ bitrate    WRITE setBitrate   )
      Q_PROPERTY(bool    enabled    READ isEnabled  WRITE setEnabled   )
      Q_PROPERTY(QString parameters READ parameters WRITE setParamaters)

      //Static setters
//       static void setActiveCodecList(Account* account, const QStringList& codecs);

      //Getters
      QString name      () const;
      uint    bitrate   () const;
      bool    isEnabled () const;
      QString parameters() const;
      QMap<QString,QString> toMap() const;

      //Setters
      void setBitrate   (const uint     bitrate );
      void setEnabled   (const bool     enabled );
      void setParamaters(const QString& params  );

   private:
      //Constructor
      Codec(const QString &codecName, uint bitRate, bool enabled);
      virtual ~Codec();

      VideoCodecPrivate* d_ptr;
};

}

#endif
