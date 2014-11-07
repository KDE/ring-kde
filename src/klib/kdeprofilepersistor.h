/****************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                          *
 *   Author : Alexandre Lision <alexandre.lision@savoirfairelinux.com> *
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

#ifndef KDEPROFILEPERSISTER_H
#define KDEPROFILEPERSISTER_H

//Ring
#include "../lib/visitors/profilepersistervisitor.h"

//Qt
#include <QDir>

class Contact;

class LIB_EXPORT KDEProfilePersister : public ProfilePersisterVisitor {
public:
   virtual bool load();
   virtual bool save(const Contact* c);
   virtual QDir getProfilesDir();

//private:
};

#endif // KDEPROFILEPERSISTER_H
