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
#include "recentfilemodel.h"

#include <QtCore/QCoreApplication>
#include <QtWidgets/QFileDialog>
#include "../klib/kcfg_settings.h"

class RecentFileModelPrivate
{
public:
    QStringList m_lFiles;
};

RecentFileModel::RecentFileModel() :
    QStringListModel(QCoreApplication::instance()), d_ptr(new RecentFileModelPrivate)
{
    d_ptr->m_lFiles = ConfigurationSkeleton::recentStreamedFiles();
    setStringList(d_ptr->m_lFiles);
}

RecentFileModel::~RecentFileModel()
{
    delete d_ptr;
}

RecentFileModel& RecentFileModel::instance()
{
    static auto i = new RecentFileModel();
    return *i;
}

QString RecentFileModel::addFile()
{
    const auto fileName = QFileDialog::getOpenFileName(
        nullptr,
        QStringLiteral("Open File"),
        QDir::currentPath(),
        QStringLiteral("Media Files (*.png *.jpg *.gif *.mp4 *.mkv *.webm *.txt *.avi *.mpg)")
    );

    d_ptr->m_lFiles << fileName;
    ConfigurationSkeleton::setRecentStreamedFiles(d_ptr->m_lFiles);

    setStringList(d_ptr->m_lFiles);

    return QStringLiteral("file://")+fileName;
}
