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

#include <QtCore/QAbstractListModel>

class DockModelPrivate;

class DockModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit DockModel(QObject* parent = nullptr);
    virtual ~DockModel();

    enum Roles {
        ActiveCount = Qt::UserRole+1,
        Identifier,
        Action,
        Enabled
    };

    enum class Mode {
        TIMELINE,
        DIALVIEW,
        CONTACTREQUESTS,
        ADDRESSBOOK,
        HISTORY,
        BOOKMARKS,
    };

    virtual QVariant data( const QModelIndex& index, int role) const override;
    virtual int rowCount( const QModelIndex& parent) const override;
    virtual QHash<int,QByteArray> roleNames() const override;

private:
    DockModelPrivate* d_ptr;
    Q_DECLARE_PRIVATE(DockModel)
};
