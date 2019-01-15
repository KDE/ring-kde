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
#include <individualdirectory.h>
#include <individual.h>
#include <session.h>
#include <accountmodel.h>

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

Person* ContactBuilder::from(Individual* ind, const QString& name)
{
    if (!ind)
        return nullptr;

    if (ind->phoneNumbers().isEmpty())
        return nullptr;

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

    auto p = ind->buildPerson();
    p->setCollection(col);
    col->editor<Person>()->addExisting(p);

    if (!name.isEmpty())
        p->setFormattedName(name);

    Q_ASSERT(ind->person() == p);

    return p;
}

Person* ContactBuilder::from(ContactMethod* cm)
{
    if (cm && cm->contact())
        return cm->contact();

    auto p = fromScratch();

    if (cm) {
        cm->setPerson(p);
        p->individual()->addPhoneNumber(cm);
    }

    qDebug() << "New contact successfully added";

    return p;
}

Person* ContactBuilder::fromScratch()
{
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
    col->editor<Person>()->addExisting(p);

    return p;
}

ContactMethod* ContactBuilder::updatePhoneNumber(ContactMethod* cm, Individual* ind, Person* p, const QString& number, int categoryIndex, int accountIdx)
{
    auto catIndex = Session::instance()->numberCategoryModel()->index(categoryIndex, 0);

    if (!catIndex.isValid())
        return nullptr;

    const bool wasTemporary = cm && cm->type() == ContactMethod::Type::TEMPORARY;

    if (wasTemporary && !number.isEmpty()) {
        qobject_cast<TemporaryContactMethod*>(cm)->setUri(number);
    }

    ContactMethod* newCM = Session::instance()->individualDirectory()->fromTemporary(cm);

    Account* a = accountIdx == -1 ? nullptr : Session::instance()->accountModel()->getAccountByModelIndex(
        Session::instance()->accountModel()->index(accountIdx, 0)
    );

    // Create a person and add the CM to the phone numbers
    if ((!cm) && ind) {
        ind->addPhoneNumber(number, a, catIndex.data().toString());
    }
    else if (!cm) {
        newCM = Session::instance()->individualDirectory()->getNumber(
            number, a, catIndex.data().toString()
        );

        from(newCM);

        return newCM;
    }

    p = (p || !newCM) ? p : newCM->contact();

    if (newCM && a)
        newCM->setAccount(a);

    if (!p) {
        if (Q_UNLIKELY(!newCM)) {
            qWarning() << "Failed to create a contact";
            return nullptr;
        }
        else {
            qWarning() << "Creating a contact for" << newCM;
            from(newCM);
            return newCM;
        }
    }

    if (!newCM) {
        newCM = Session::instance()->individualDirectory()->getNumber(
            number, p, a, catIndex.data().toString()
        );
        p->individual()->addPhoneNumber(newCM);
    }
    else if (wasTemporary)
        p->individual()->addPhoneNumber(newCM);
    else {
        auto newCM2 = Session::instance()->individualDirectory()->getNumber(
            number, p, newCM->account(), catIndex.data().toString()
        );
        p->individual()->replacePhoneNumber(newCM, newCM2);
        newCM = newCM2;
    }

    if (wasTemporary)
        p->individual()->setEditRow(false);

    qDebug() << "New phone number added to" << p;

    return newCM;
}


void ContactBuilder::addEmptyPhoneNumber(Person* p)
{
    if (!p)
        return;

    const auto idx = p->individual()->index(p->individual()->rowCount(), 0);
    p->individual()->setData(idx, QString(), Qt::DisplayRole);
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
