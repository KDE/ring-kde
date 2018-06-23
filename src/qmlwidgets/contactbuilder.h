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
#pragma once

#include <QtCore/QObject>

class ContactMethod;
class Person;
class Account;
class Individual;

class ContactBuilderPrivate;

class ContactBuilder : public QObject
{
    Q_OBJECT

public:
    explicit ContactBuilder(QObject* parent = nullptr);
    virtual ~ContactBuilder();

    Q_INVOKABLE Person* from(ContactMethod* cm);
    Q_INVOKABLE Person* from(Individual* cm, const QString& name = "");

    Q_INVOKABLE Person* fromScratch();

    Q_INVOKABLE ContactMethod* updatePhoneNumber(ContactMethod* cm, Individual* ind, Person* p, const QString& number, int categoryIndex, int accountIdx);

    Q_INVOKABLE void addEmptyPhoneNumber(Person* p);
    Q_INVOKABLE void acceptEmptyPhoneNumber(Person* p);
    Q_INVOKABLE void rejectEmptyPhoneNumber(Person* p);

private:
    ContactBuilderPrivate* d_ptr;
    Q_DECLARE_PRIVATE(ContactBuilder)
};
