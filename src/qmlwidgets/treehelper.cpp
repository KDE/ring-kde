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
#include <QtCore/QMimeData>

class TreeHelperPrivate
{
public:
    QMap<QString, QString> m_Payloads {};

    //FIXME it leaks
    static QHash<const QAbstractItemModel*, QHash<QString, int>> m_shRoleNameMapper;
    QAbstractItemModel* m_pModel {nullptr};

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


QAbstractItemModel* TreeHelper::model() const
{
    return d_ptr->m_pModel;
}

void TreeHelper::setModel(QAbstractItemModel* model)
{
    d_ptr->m_pModel = model;
}

QVariant TreeHelper::mimeData(const QModelIndex& parent, int row) const
{
    qDebug() << "\n\nIN WRAP MIME DATA";

    if (!d_ptr->m_pModel)
        return {};

    const auto idx = d_ptr->m_pModel->index(row, 0, parent);

    if (!idx.isValid())
        return {};

    const auto md = d_ptr->m_pModel->mimeData({idx});

    qDebug() << "GOT DATA" << md;

    QVariantMap ret;

    const auto formats = md->formats();

    for (const auto& mime : qAsConst(formats)) {
        ret[mime] = QString(md->data(mime));
    }

    return QVariant::fromValue(ret);
}

bool TreeHelper::dropMimeData(const QVariant& dragEvent, const QModelIndex& parent, int row)
{
    auto obj = qvariant_cast<QObject*>(dragEvent);

    if (!obj)
        return false;

    if (obj->metaObject()->className() != QLatin1String("QQuickDropEvent"))
        return false;

    const QStringList formats = obj->property("formats").toStringList();

    QMimeData* md = new QMimeData();

    for (const auto& mime : qAsConst(formats)) {
        //stupid unexported qmimedata
    }

    return false;
}

bool TreeHelper::dropMimeData2(const QVariant& dragEvent, const QModelIndex& parent, int row)
{
    qDebug() << dragEvent << dragEvent.canConvert<QVariantMap>();

    QMap<QString, QVariant> map = qvariant_cast<QVariantMap>(dragEvent);

    const auto idx = d_ptr->m_pModel->index(row, 0, parent);

    if (!idx.isValid())
        return false;

    QMimeData* md = new QMimeData();

    for(auto i = map.constBegin(); i != map.constEnd(); i++) {
        md->setData(i.key(), i.value().toByteArray());
    }

    return d_ptr->m_pModel->dropMimeData(md, {}, row, 0, parent);
}
