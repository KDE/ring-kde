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
#include <individual.h>

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

Person* ContactBuilder::from(ContactMethod* cm)
{
    if (cm && cm->contact())
        return cm->contact();

    auto cols = PersonModel::instance().enabledCollections(
        CollectionInterface::SupportedFeatures::ADD |
        CollectionInterface::SupportedFeatures::EDIT|
        CollectionInterface::SupportedFeatures::SAVE
    );

    if (cols.isEmpty()) {
        qWarning() << "Failed to add a contact: no suitable backend found";
        return nullptr;
    }

    const auto col = cols.first();

    auto p = new Person();
    p->setCollection(col); //TODO have a selection widget again

    if (cm) {
        cm->setPerson(p);
        p->individual()->addPhoneNumber(cm);
    }

    col->editor<Person>()->addExisting(p);

    qDebug() << "New contact successfully added";

    return p;
}

ContactMethod* ContactBuilder::updatePhoneNumber(ContactMethod* cm, Person* p, const QString& number, int categoryIndex)
{
    auto catIndex = NumberCategoryModel::instance().index(categoryIndex, 0);

    if (!catIndex.isValid())
        return nullptr;

    ContactMethod* newCM = cm;

    // Create a person and add the CM to the phone numbers
    if ((!p) && !cm) {
        newCM = PhoneDirectoryModel::instance().getNumber(
            number, catIndex.data().toString()
        );
        from(newCM);

        return newCM;
    }

    p = (p || !newCM) ? p : newCM->contact();

    if (!p) {
        qWarning() << "Failed to create a contact";
        return nullptr;
    }

    if (!newCM) {
        newCM = PhoneDirectoryModel::instance().getNumber(
            number, p, nullptr, catIndex.data().toString()
        );
        p->individual()->addPhoneNumber(newCM);
    }
    else if (p && newCM->type() == ContactMethod::Type::TEMPORARY) {
        p->individual()->addPhoneNumber(newCM);

        const auto lastRow = p->individual()->index(
            p->individual()->rowCount()-1, 0
        );

        if (QVariant::fromValue(cm) == p->individual()->data(lastRow, (int)Ring::Role::Object))
            p->individual()->removeRows(lastRow.row(), 1, {});
    }
    else if (p) {
        auto newCM2 = PhoneDirectoryModel::instance().getNumber(
            number, p, newCM->account(), catIndex.data().toString()
        );
        p->individual()->replacePhoneNumber(newCM, newCM2);
        newCM = newCM2;
    }

    qDebug() << "New phone number added to" << p;

    return newCM;
}


void ContactBuilder::addEmptyPhoneNumber(Person* p)
{
    if (!p)
        return;

    const auto idx = p->individual()->index(p->individual()->rowCount(), 0);
    const bool ret = p->individual()->setData(idx, QString(), Qt::DisplayRole);
}

void ContactBuilder::acceptEmptyPhoneNumber(Person* p)
{
    if (!p)
        return;

}
void ContactBuilder::rejectEmptyPhoneNumber(Person* p)
{
    if (!p)
        return;

}
