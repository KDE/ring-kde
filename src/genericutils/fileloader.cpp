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
#include "fileloader.h"

// Qt
#include <QtCore/QFile>

class FileLoaderPrivate
{
public:
    QString m_Path;
    QString m_Content;
};

FileLoader::FileLoader(QObject* parent) :
    QObject(parent), d_ptr(new FileLoaderPrivate())
{}

FileLoader::~FileLoader()
{
    delete d_ptr;
}

QString FileLoader::path() const
{
    return d_ptr->m_Path;
}

void FileLoader::setPath(const QString& path)
{
    d_ptr->m_Path = path;

    QFile file(path);
    d_ptr->m_Content = file.open(QIODevice::ReadOnly) ? file.readAll() : "";

    emit contentChanged();
}

QString FileLoader::content() const
{
    return d_ptr->m_Content;
}
