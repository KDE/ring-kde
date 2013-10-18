/****************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                               *
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

#include "../typedefs.h"

//Qt
#include <QtCore/QVariant>

//SFLPhone
class Contact    ;
class PhoneNumber;
class Call       ;

/**
 * Different clients can have multiple way of displaying images. Some may
 * add borders, other add corner radius (see Ubuntu-SDK HIG). This
 * abstract class define many operations that can be defined by each clients.
 *
 * Most methods return QVariants as this library doesn't link against QtGui
 * 
 * This interface is not frozen, more methods may be added later. To implement,
 * just create an object somewhere, be sure to call PixmapManipulationVisitor()
 */
class LIB_EXPORT PixmapManipulationVisitor {
public:
   PixmapManipulationVisitor();
   virtual ~PixmapManipulationVisitor() {}
   virtual QVariant contactPhoto(Contact* c, QSize size, bool displayPresence = true);
   virtual QVariant callPhoto(Call* c, QSize size, bool displayPresence = true);
   virtual QVariant callPhoto(const PhoneNumber* n, QSize size, bool displayPresence = true);
   virtual QVariant numberCategoryIcon(PhoneNumber* n, QSize size, bool displayPresence = false);

   //Singleton
   static PixmapManipulationVisitor* instance();
protected:
   static PixmapManipulationVisitor* m_spInstance;
};
