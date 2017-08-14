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
#include <QtCore/QRect>
#include <QtGui/QImage>

class Person;

class PhotoSelectorPrivate;

class PhotoSelector : public QObject
{
    Q_OBJECT

public:
    Q_PROPERTY(QRect sourceRect READ sourceRect CONSTANT)
    Q_PROPERTY(QRect selectedSquare READ selectedSquare WRITE setSelectedSquare)
    Q_PROPERTY(QString path READ path NOTIFY pathChanged)
    Q_PROPERTY(QImage image READ image WRITE setImage)

    explicit PhotoSelector(QObject* parent = nullptr);
    virtual ~PhotoSelector();

    QRect sourceRect() const;
    QRect selectedSquare() const;
    void setSelectedSquare(QRect r);

    QImage image() const;
    void setImage(const QImage& image);

    Q_INVOKABLE QString selectFile(const QString& defaultPath = {});

    Q_INVOKABLE void setToPerson(Person* p);

    QString path() const;

Q_SIGNALS:
    void pathChanged(const QString& path);

private:
    PhotoSelectorPrivate* d_ptr;
    Q_DECLARE_PRIVATE(PhotoSelector)
};
