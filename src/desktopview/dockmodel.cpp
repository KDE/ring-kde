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
#include "dockmodel.h"

#include <QtWidgets/QAction>

#include <callmodel.h>
#include <session.h>

#include "../jamikdeintegration/src/actioncollection.h" //FIXME

class DockModelPrivate final : public QObject
{
    Q_OBJECT
public:
    QVector<QAction*> m_lEntries;

    DockModel* q_ptr;
public Q_SLOTS:
    void reload();
};

DockModel::DockModel(QObject* parent) : QAbstractListModel(parent), d_ptr(new DockModelPrivate)
{
    d_ptr->q_ptr = this;

    for (auto tuple : {
        std::make_tuple(ActionCollection::instance()->showBookmarkDockAction(), "/toolbar/bookmark.svg", "bookmark"),
        std::make_tuple(ActionCollection::instance()->showContactDockAction (), "/toolbar/contact.svg" , "contact" ),
        std::make_tuple(ActionCollection::instance()->showTimelineDockAction(), "/toolbar/timeline.svg", "timeline"),
        std::make_tuple(ActionCollection::instance()->showHistoryDockAction (), "/toolbar/history.svg" , "history" ),
        std::make_tuple(ActionCollection::instance()->showDialDockAction    (), "/toolbar/call.svg"    , "call"    ),
    }) {
        qDebug() << std::get<0>(tuple) << std::get<1>(tuple);
        Q_ASSERT(std::get<0>(tuple));
        connect(std::get<0>(tuple), &QAction::toggled, d_ptr, &DockModelPrivate::reload);
        std::get<0>(tuple)->setProperty("iconpath"  , std::get<1>(tuple));
        std::get<0>(tuple)->setProperty("identifier", std::get<2>(tuple));
    }

    connect(Session::instance()->callModel(), &CallModel::callStateChanged,
        d_ptr, &DockModelPrivate::reload);

    connect(Session::instance()->callModel(), &CallModel::layoutChanged,
        d_ptr, &DockModelPrivate::reload);

    d_ptr->reload();
}

DockModel::~DockModel()
{
    delete d_ptr;
}

QVariant DockModel::data( const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    auto a = d_ptr->m_lEntries[index.row()];

    switch(role) {
        case Qt::DisplayRole:
            return a->text();
        case Qt::DecorationRole:
            return a->property("iconpath");
        case Roles::Identifier:
            return a->property("identifier");
        case Roles::Action:
            return QVariant::fromValue(a);
        case Roles::ActiveCount:
            if (a == ActionCollection::instance()->showDialDockAction())
                return Session::instance()->callModel()->size()
                    - (Session::instance()->callModel()->hasDialingCall() ? 1 : 0);
            return 0;
    }

    return {};
}

int DockModel::rowCount( const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : d_ptr->m_lEntries.size();
}

QHash<int,QByteArray> DockModel::roleNames() const
{
    static QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();
    static std::atomic_flag initRoles = ATOMIC_FLAG_INIT;

    if (!initRoles.test_and_set()) {
        roles[Roles::ActiveCount] = QByteArray( "activeCount" );
        roles[Roles::Identifier ] = QByteArray( "identifier"  );
        roles[Roles::Action     ] = QByteArray( "action"      );
        roles[Roles::Enabled    ] = QByteArray( "enabled2"    );
    }

   return roles;
}

void DockModelPrivate::reload()
{
    QVector<QAction*> l;

    for (QAction* a : {
        ActionCollection::instance()->showBookmarkDockAction(),
        ActionCollection::instance()->showContactDockAction (),
        ActionCollection::instance()->showTimelineDockAction(),
        ActionCollection::instance()->showHistoryDockAction (),
        ActionCollection::instance()->showDialDockAction    (),
    }) {
        if (a->isChecked())
            l << a;
    }

    if (m_lEntries != l) {
        q_ptr->beginResetModel();
        m_lEntries = l;
        q_ptr->endResetModel();
    }
    else
        emit q_ptr->dataChanged(
            q_ptr->index(0, 0), q_ptr->index(m_lEntries.size()-1,0)
        );
}

#include <dockmodel.moc>
