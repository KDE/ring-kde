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

#include <QQuickPaintedItem>

class ContactMethod;
class Person;

class ContactPhotoPrivate;

/**
 * This widget paints the contact with various built-in attributes:
 *
 *  * The photo (if any)
 *  * The presence status
 */
class ContactPhoto : public QQuickPaintedItem
{
    Q_OBJECT
public:
    Q_PROPERTY(ContactMethod* contactMethod READ contactMethod WRITE setContactMethod)
    Q_PROPERTY(Person* person READ person WRITE setPerson)
    Q_PROPERTY(bool hasPhoto READ hasPhoto NOTIFY hasPhotoChanged)
    Q_PROPERTY(bool displayEmpty READ displayEmpty WRITE setDisplayEmpty)
    Q_PROPERTY(bool drawEmptyOutline READ drawEmptyOutline WRITE setDrawEmptyOutline)

    explicit ContactPhoto(QQuickItem* parent = nullptr);
    virtual ~ContactPhoto();

    virtual void paint(QPainter *painter) override;

    ContactMethod* contactMethod() const;
    void setContactMethod(ContactMethod* cm);

    Person* person() const;
    void setPerson(Person* p);

    bool hasPhoto() const;

    bool displayEmpty() const;
    void setDisplayEmpty(bool val);

    bool drawEmptyOutline() const;
    void setDrawEmptyOutline(bool val);

Q_SIGNALS:
    void hasPhotoChanged();

private:
    ContactPhotoPrivate* d_ptr;
};
