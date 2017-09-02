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
#include <QtCore/QModelIndex>

class ContactMethod;

class TreeHelperPrivate;

class TreeHelper : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit TreeHelper(QObject* parent = nullptr);
    virtual ~TreeHelper();

    Q_INVOKABLE QModelIndex getIndex(int row, const QModelIndex& parent);
    Q_INVOKABLE bool setData(const QModelIndex& index, const QVariant& data, const QString& roleName);

private:
    TreeHelperPrivate* d_ptr;
    Q_DECLARE_PRIVATE(TreeHelper)
};

Q_DECLARE_METATYPE(TreeHelper*)
