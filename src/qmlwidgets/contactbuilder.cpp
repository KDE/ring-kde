/***************************************************************************
 *   Copyright (C) 2017 by Bluesystems                                     *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
#include "contactbuilder.h"

#include <contactmethod.h>
#include <person.h>
#include <personmodel.h>
#include <collectioninterface.h>
#include <numbercategorymodel.h>
#include <phonedirectorymodel.h>

class ContactBuilderPrivate
{
public:
    QMap<QString, QString> m_Payloads {};
};

ContactBuilder::ContactBuilder(QObject* parent) : QObject(parent),
    d_ptr(new ContactBuilderPrivate())
{}

ContactBuilder::~ContactBuilder()
{
    delete d_ptr;
}

bool ContactBuilder::from(ContactMethod* cm)
{
    if (cm->contact())
        return false;

    auto cols = PersonModel::instance().enabledCollections(
        CollectionInterface::SupportedFeatures::ADD |
        CollectionInterface::SupportedFeatures::EDIT|
        CollectionInterface::SupportedFeatures::SAVE
    );

    if (cols.isEmpty()) {
        qWarning() << "Failed to add a contact: no suitable backend found";
        return false;
    }

    const auto col = cols.first();

    auto p = new Person();
    p->setCollection(col); //TODO have a selection widget again
    cm->setPerson(p);
    p->setContactMethods({cm});

    return true;
}

bool ContactBuilder::addPhoneNumber(Person* p, const QString& number, int categoryIndex)
{
    auto catIndex = NumberCategoryModel::instance().index(categoryIndex, 0);

    if (!catIndex.isValid())
        return false;

    // Force a copy
    QVector<ContactMethod*> pn(p->phoneNumbers());

    auto newCM = PhoneDirectoryModel::instance().getNumber(
        number, p, nullptr, catIndex.data().toString()
    );

    const auto cpn = pn;
    for (auto cm : qAsConst(cpn)) {
        if (cm == newCM) {
            qWarning() << "Trying to add an already added phone number";
            return false;
        }
    }

    pn << newCM;

    p->setContactMethods(pn);

    return true;
}
