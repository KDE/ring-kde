/****************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                          *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>          *
 *            Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
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

#ifndef HELPER_FUNCTIONS
#define HELPER_FUNCTIONS

//Qt
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtGui/QFont>

//Ring
#include "person.h"

//Typedef
typedef QHash<QString,QHash<QString,QVariant> > PersonHash;

///HelperFunctions: little visitor not belonging to libringclient
///Ramdom mix of dynamic property and transtypping
class LIB_EXPORT HelperFunctions {
public:
   static QString     normStrippped             (QString str              );
   static QString     escapeHtmlEntities        (QString str              );
   static void        displayNoAccountMessageBox(QWidget* parent = nullptr);
};
#endif

// kate: space-indent on; indent-width 3; replace-tabs on;
