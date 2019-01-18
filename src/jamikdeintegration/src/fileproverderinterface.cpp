/***************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                         *
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
#include "fileproverderinterface.h"

#include <QtWidgets/QFileDialog>
#include <QtCore/QDir>

#include <kcfg_settings.h>

QUrl FileProviderInterface::getAnyFile(const QStringList& extensions) const
{
    Q_UNUSED(extensions)
    return QFileDialog::getOpenFileName(
        nullptr,
        QStringLiteral("Open File"),
        QDir::currentPath(),
        QStringLiteral("Media Files (*.png *.jpg *.gif *.mp4 *.mkv *.webm *.txt *.avi *.mpg)")
    );
}

QList<QUrl> FileProviderInterface::recentFiles() const
{
    const QStringList files = ConfigurationSkeleton::recentStreamedFiles();
    QList<QUrl> ret;

    for (const auto& f : qAsConst(files))
        ret << f;

    return ret;
}

void FileProviderInterface::addRecentFile(const QUrl& path) const
{
    QStringList files = ConfigurationSkeleton::recentStreamedFiles();
    files << path.path();

    ConfigurationSkeleton::setRecentStreamedFiles(files);

}
