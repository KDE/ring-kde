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
#include "directoryview.h"

#include "ui_directoryview.h"

#include <QtCore/QSortFilterProxyModel>
#include <QtCore/QTimer>

#include <phonedirectorymodel.h>
#include <contactmethod.h>

DirectoryView::DirectoryView(QWidget* parent)
{
    Ui_DirectoryView ui;
    ui.setupUi(this);

    auto p = new QSortFilterProxyModel(this);
    p->setSourceModel(&PhoneDirectoryModel::instance());
    p->setFilterRole((int)ContactMethod::Role::Filter);
    p->setFilterCaseSensitivity( Qt::CaseInsensitive );
    p->setSortCaseSensitivity  ( Qt::CaseInsensitive );
    connect(ui.lineEdit ,SIGNAL(filterStringChanged(QString)), p, SLOT(setFilterRegExp(QString)));
    ui.tableView->setModel(p);
    exec();

    auto le = ui.lineEdit;
    auto tv = ui.tableView;

    QTimer::singleShot(0, [le, p, tv]() {
        le->setFocus(Qt::OtherFocusReason);
        for (int i = 0; i < p->columnCount(); i++)
            tv->horizontalHeader()->setSectionResizeMode(i,QHeaderView::ResizeToContents);
    });
}

DirectoryView::~DirectoryView()
{
}
