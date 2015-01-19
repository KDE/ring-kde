/************************************************************************************
 *   Copyright (C) 2014 by Savoir-Faire Linux                                       *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>         *
 *                                                                                  *
 *   This library is free software; you can redistribute it and/or                  *
 *   modify it under the terms of the GNU Lesser General Public                     *
 *   License as published by the Free Software Foundation; either                   *
 *   version 2.1 of the License, or (at your option) any later version.             *
 *                                                                                  *
 *   This library is distributed in the hope that it will be useful,                *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                 *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU              *
 *   Lesser General Public License for more details.                                *
 *                                                                                  *
 *   You should have received a copy of the GNU Lesser General Public               *
 *   License along with this library; if not, write to the Free Software            *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA *
 ***********************************************************************************/
#ifndef BOOKMARKBACKEND_H
#define BOOKMARKBACKEND_H

#include <abstractitembackend.h>

class QTextStream;

class LIB_EXPORT BookmarkBackend : public AbstractBookmarkBackend
{
public:
   explicit BookmarkBackend(QObject* parent = nullptr);
   virtual ~BookmarkBackend();

   virtual bool load() override;
   virtual bool reload() override;
   virtual bool clear() override;
   virtual bool remove(PhoneNumber* number) override;
   virtual bool save(const PhoneNumber* number) override;
   virtual bool append(const PhoneNumber* number) override;

   virtual QString    name     () const override;
   virtual QVariant   icon     () const override;
   virtual bool       isEnabled() const override;
   virtual QByteArray id       () const override;

   virtual SupportedFeatures  supportedFeatures() const override;

   virtual QList<PhoneNumber*> items() const override;

   ///Edit 'item', the implementation may be a GUI or somehting else
   virtual bool edit( PhoneNumber* number) override;

   ///Add a new item to the backend
   virtual bool addNew( PhoneNumber* number) override;

//    ///Add a new phone number to an existing item
//    virtual bool addPhoneNumber( PhoneNumber* number , PhoneNumber* number );

private:
   //Attributes
   QList<PhoneNumber*> m_lNumbers;

   //Helpers
   void saveHelper(QTextStream& streamFileOut, const PhoneNumber* number);
};

#endif
