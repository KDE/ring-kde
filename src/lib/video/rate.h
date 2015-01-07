/****************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                          *
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
#ifndef VIDEORATE_H
#define VIDEORATE_H

#include "../typedefs.h"

namespace Video {
   class Resolution;
   class Device;
}
// class VideoRatePrivate
// {
// public:
//    
// };

namespace Video {

///@typedef Rate The rate for a device
class LIB_EXPORT Rate
{
   //Can only be created by Video::Device
   friend class Video::Device;

public:
   virtual ~Rate() {}
   QString name() const;
   int relativeIndex();

private:
   Rate(const Video::Resolution* res,const QString& name) :
      m_Name(name),m_pResolution(res) {}
   QString m_Name;
   const Video::Resolution* m_pResolution;
};

}

#endif
