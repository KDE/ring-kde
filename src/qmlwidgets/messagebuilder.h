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

class MessageBuilderPrivate;

class MessageBuilder : public QObject
{
    Q_OBJECT

public:
    explicit MessageBuilder(QObject* parent = nullptr);
    virtual ~MessageBuilder();

    Q_INVOKABLE void addPayload(const QString& mime, const QString& content);

    Q_INVOKABLE void sendWidth(ContactMethod* cm);

private:
    MessageBuilderPrivate* d_ptr;
    Q_DECLARE_PRIVATE(MessageBuilder)
};
