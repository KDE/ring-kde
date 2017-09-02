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
#include "treehelper.h"

#include <QtCore/QDebug>

class TreeHelperPrivate
{
public:
    QMap<QString, QString> m_Payloads {};

    //FIXME it leaks
    static QHash<const QAbstractItemModel*, QHash<QString, int>> m_shRoleNameMapper;
};

QHash<const QAbstractItemModel*, QHash<QString, int>> TreeHelperPrivate::m_shRoleNameMapper;

TreeHelper::TreeHelper(QObject* parent) : QObject(parent),
    d_ptr(new TreeHelperPrivate())
{}

bool TreeHelper::setData(const QModelIndex& index, const QVariant& data, const QString& roleName)
{
    if (!index.isValid())
        return false;

    auto model = const_cast<QAbstractItemModel*>(index.model());

    auto ret = d_ptr->m_shRoleNameMapper.value(model);

    // Map the role names
    if (!d_ptr->m_shRoleNameMapper.contains(model)) {
        const auto rn = model->roleNames();
        for (auto i = rn.constBegin(); i != rn.constEnd(); i++)
            ret[i.value()] = i.key();

        d_ptr->m_shRoleNameMapper[model] = ret;
    }

    if (!ret.contains(roleName))
        return false;

    return model->setData(index, data, ret[roleName]);
}

TreeHelper::~TreeHelper()
{
    delete d_ptr;
}

QModelIndex TreeHelper::getIndex(int row, const QModelIndex& parent)
{
    if (!parent.isValid())
        return {};

//     Q_ASSERT(parent.parent().isValid() == true);
//     Q_ASSERT(!parent.parent().parent().isValid() == true);
    return parent.model()->index(row, 0, parent);
}
