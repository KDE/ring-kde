/************************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                                  *
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
#ifndef MINIMALHISTORYBACKEND_H
#define MINIMALHISTORYBACKEND_H

#include "collectioninterface.h"
#include "collectioneditor.h"

class Call;

template<typename T> class CollectionMediator;

class LIB_EXPORT MinimalHistoryBackend : public CollectionInterface
{
public:
   template<typename T>
   explicit MinimalHistoryBackend(CollectionMediator<T>* mediator);
   virtual ~MinimalHistoryBackend();

   virtual bool load() override;
   virtual bool reload() override;
   virtual bool clear() override;
//    virtual bool save(const Call* call) override;
//    virtual bool append(const Call* item) override;

   virtual QString    name     () const override;
   virtual QString    category () const override;
   virtual QVariant   icon     () const override;
   virtual bool       isEnabled() const override;
   virtual QByteArray id       () const override;
//    virtual bool remove ( Call* c ) override;

   virtual SupportedFeatures  supportedFeatures() const override;

//    virtual QList<Call*> items() const override;

   ///Edit 'item', the implementation may be a GUI or somehting else
//    virtual bool edit( Call* call) override;
   ///Add a new item to the backend
//    virtual bool addNew( Call* call) override;

   ///Add a new phone number to an existing item
//    virtual bool addPhoneNumber( Call* call , PhoneNumber* number ) override;
private:
   CollectionMediator<Call>*  m_pMediator;
};

class MinimalHistoryEditor : public CollectionEditor<Call>
{
public:
   MinimalHistoryEditor(CollectionMediator<Call>* m) : CollectionEditor<Call>(m) {}
   virtual bool save       ( const Call* item ) override;
   virtual bool append     ( const Call* item ) override;
   virtual bool remove     ( Call*       item ) override;
   virtual bool edit       ( Call*       item ) override;
   virtual bool addNew     ( Call*       item ) override;

private:
   virtual QVector<Call*> items() const override;
};


template<typename T>
MinimalHistoryBackend::MinimalHistoryBackend(CollectionMediator<T>* mediator) :
CollectionInterface(new MinimalHistoryEditor(nullptr)),m_pMediator(mediator)
{
//    setObjectName("MinimalHistoryBackend");
}

#endif
