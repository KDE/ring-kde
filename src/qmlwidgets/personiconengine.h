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

#include <QtGui/QIconEngine>

class Person;

class PersonIconEnginePrivate;

/**
 * Generate pixmap with photo and presence data.
 */
class PersonIconEngine : public QIconEngine
{
public:
    explicit PersonIconEngine(Person* person);
    virtual ~PersonIconEngine();

    virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override;
    virtual QIconEngine* clone() const override;
    virtual QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state) override;

private:
    PersonIconEnginePrivate* d_ptr;
};
