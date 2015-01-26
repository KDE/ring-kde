/****************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                          *
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
#ifndef KDEPIXMAPMANIPULATION_H
#define KDEPIXMAPMANIPULATION_H

#include <visitors/pixmapmanipulationvisitor.h>
#include <call.h>

class Contact;


class KDEPixmapManipulation : public QObject ,public PixmapManipulationVisitor {
   Q_OBJECT
public:
   KDEPixmapManipulation();
   QVariant contactPhoto(Contact* c, const QSize& size, bool displayPresence = true);

   virtual QVariant callPhoto(const PhoneNumber* n, const QSize& size, bool displayPresence = true);

   virtual QVariant callPhoto(Call* c, const QSize& size, bool displayPresence = true);

   virtual QByteArray toByteArray(const QVariant& pxm);

   virtual QVariant profilePhoto(const QByteArray& data);

   QVariant numberCategoryIcon(const QVariant& p, const QSize& size, bool displayPresence = false, bool isPresent = false);

   virtual QVariant serurityIssueIcon(const QModelIndex& index);

   static const char* icnPath[2][2];
private:
   static const TypedStateMachine< const char* , Call::State > callStateIcons;


   //Helper
   QPixmap drawDefaultUserPixmap(const QSize& size, bool displayPresence, bool isPresent);

private Q_SLOTS:
   void clearCache();
};

#endif // KDEPIXMAPMANIPULATION_H
