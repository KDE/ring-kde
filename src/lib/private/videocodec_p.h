/****************************************************************************
 *   Copyright (C) 2014 by Savoir-Faire Linux                               *
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
#ifndef VIDEOCODECPRIVATE_H
#define VIDEOCODECPRIVATE_H

typedef QHash<QString,Video::Codec*> CodecHash;

class VideoCodecPrivate
{
public:
   VideoCodecPrivate();

   //Consts
   class CodecFields {
   public:
      constexpr static const char* PARAMETERS = "parameters";
      constexpr static const char* ENABLED    = "enabled"   ;
      constexpr static const char* BITRATE    = "bitrate"   ;
      constexpr static const char* NAME       = "name"      ;
   };

   //Attributes
   static CodecHash m_slCodecs  ;
   QString          m_Name      ;
   uint             m_Bitrate   ;
   bool             m_Enabled   ;
   static bool      m_sInit     ;
   QString          m_Parameters;
};

#endif
