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
#ifndef MIME_H
#define MIME_H

namespace RingMimes {
   constexpr static const char* CALLID      = "text/ring.call.id"      ;
   constexpr static const char* CONTACT     = "text/ring.contact"      ;
   constexpr static const char* HISTORYID   = "text/ring.history.id"   ;
   constexpr static const char* PHONENUMBER = "text/ring.phone.number" ;
   constexpr static const char* PLAIN_TEXT  = "text/plain"             ;
}

#endif

