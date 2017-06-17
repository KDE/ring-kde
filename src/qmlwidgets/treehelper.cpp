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
};

TreeHelper::TreeHelper(QObject* parent) : QObject(parent),
    d_ptr(new TreeHelperPrivate())
{}

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
