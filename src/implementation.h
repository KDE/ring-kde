/***************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                         *
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

#include <delegates/accountlistcolordelegate.h>
#include <call.h>
#include <account.h>

#include <QtGui/QPalette>
#include <QtGui/QColor>
#include <QtCore/QMutex>
#include <delegates/presenceserializationdelegate.h>

//Implement all client dependant libringclient abstract interfaces

class ColorDelegate : public AccountListColorDelegate {
public:
   explicit ColorDelegate(QPalette pal);

   virtual QVariant getColor(const Account* a);

   virtual QVariant getIcon(const Account* a);
private:
   QPalette m_Pal;
   QColor   m_Green;
   QColor   m_Yellow;
   QColor   m_Red;
};

class KDEPresenceSerializationDelegate : public PresenceSerializationDelegate {
public:
   KDEPresenceSerializationDelegate():m_isLoaded(false){}
   virtual void serialize() override;
   virtual void load() override;
   virtual ~KDEPresenceSerializationDelegate();
   virtual bool isTracked(CollectionInterface* backend) override;
   virtual void setTracked(CollectionInterface* backend, bool tracked) override;
private:
   QHash<QString,bool> m_hTracked;
   bool m_isLoaded;
};

#endif
