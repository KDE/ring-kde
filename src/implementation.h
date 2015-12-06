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

#include <interfaces/accountlistcolorizeri.h>
#include <call.h>
#include <account.h>

#include <QtGui/QPalette>
#include <QtGui/QColor>
#include <QtCore/QMutex>
#include <interfaces/presenceserializeri.h>
#include <interfaces/shortcutcreatori.h>
#include <interfaces/actionextenderi.h>

class Macro;

//Implement all client dependant libringclient abstract interfaces

class ColorDelegate : public Interfaces::AccountListColorizerI
{
public:
   explicit ColorDelegate(QPalette pal);

   virtual QVariant color(const Account* a) override;

   virtual QVariant icon(const Account* a) override;
private:
   QPalette m_Pal;
   QColor   m_Green;
   QColor   m_Yellow;
   QColor   m_Red;
};

class KDEPresenceSerializationDelegate : public Interfaces::PresenceSerializerI
{
public:
   KDEPresenceSerializationDelegate():m_isLoaded(false){}
   virtual void save() override;
   virtual void load() override;
   virtual ~KDEPresenceSerializationDelegate();
   virtual bool isTracked(CollectionInterface* backend) const override;
   virtual void setTracked(CollectionInterface* backend, bool tracked) override;
private:
   mutable QHash<QString,bool> m_hTracked;
   mutable bool m_isLoaded;
};

class KDEShortcutDelegate : public Interfaces::ShortcutCreatorI
{
public:
   virtual ~KDEShortcutDelegate(){}
   virtual QVariant createAction(Macro* macro) override;
};

class LIB_EXPORT KDEActionExtender final : public Interfaces::ActionExtenderI
{
public:

   virtual void editPerson(Person* p) override;
   virtual void viewChatHistory(ContactMethod* cm) override;
   virtual void copyInformation(QMimeData* data) override;
   virtual bool warnDeletePerson(Person* p) override;
   virtual bool warnDeleteCall(Call* c) override;
   virtual Person* selectPerson(FlagPack<SelectPersonHint> hints) const override;
   virtual ContactMethod* selectContactMethod(FlagPack<ActionExtenderI::SelectContactMethodHint>) const override;
};

#endif
