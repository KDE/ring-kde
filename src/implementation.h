/***************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/
#ifndef IMPLEMENTATION_H
#define IMPLEMENTATION_H

#include <lib/visitors/accountlistcolorvisitor.h>
#include <lib/visitors/pixmapmanipulationvisitor.h>
#include <lib/visitors/presenceserializationvisitor.h>
#include <lib/call.h>
#include <lib/account.h>

#include <QtGui/QPalette>
#include <QtGui/QColor>

//Implement all client dependant libqtsflphone abstract interfaces

class ColorVisitor : public AccountListColorVisitor {
public:
   explicit ColorVisitor(QPalette pal);

   virtual QVariant getColor(const Account* a);

   virtual QVariant getIcon(const Account* a);
private:
   QPalette m_Pal;
   QColor   m_Green;
   QColor   m_Yellow;
   QColor   m_Red;
};

class KDEPixmapManipulation : public QObject ,public PixmapManipulationVisitor {
   Q_OBJECT
public:
   KDEPixmapManipulation();
   QVariant contactPhoto(Contact* c, const QSize& size, bool displayPresence = true);

   virtual QVariant callPhoto(const PhoneNumber* n, const QSize& size, bool displayPresence = true);

   virtual QVariant callPhoto(Call* c, const QSize& size, bool displayPresence = true);

   QVariant numberCategoryIcon(const QPixmap* p, const QSize& size, bool displayPresence = false, bool isPresent = false);

   virtual QVariant serurityIssueIcon(const QModelIndex& index);

   static const char* icnPath[2][2];
private:
   static const TypedStateMachine< const char* , Call::State > callStateIcons;


   //Helper
   QPixmap drawDefaultUserPixmap(const QSize& size, bool displayPresence, bool isPresent);

private Q_SLOTS:
   void clearCache();
};

class KDEPresenceSerializationVisitor : public PresenceSerializationVisitor {
public:
   KDEPresenceSerializationVisitor():m_isLoaded(false){}
   virtual void serialize() override;
   virtual void load() override;
   virtual ~KDEPresenceSerializationVisitor();
   virtual bool isTracked(AbstractItemBackendBase* backend) override;
   virtual void setTracked(AbstractItemBackendBase* backend, bool tracked) override;
private:
   QHash<QString,bool> m_hTracked;
   bool m_isLoaded;
};

#endif
