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

#include <QtWidgets/QDockWidget>

class ViewContactDockPrivate;
class ContactMethod;
class Person;

class ViewContactDock final : public QWidget
{
    Q_OBJECT
public:
    enum class Pages {
        INFORMATION,
        TIMELINE,
        CALL_HISTORY,
        RECORDINGS,
        SEARCH,
        MEDIA,
    };

    explicit ViewContactDock(QWidget* parent = nullptr);
    virtual ~ViewContactDock();

    void setCurrentPage(Pages page);

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event) override;

public Q_SLOTS:
    void setContactMethod(ContactMethod* cm);
    void setPerson(Person* p);

private:
    ViewContactDockPrivate* d_ptr;
    Q_DECLARE_PRIVATE(ViewContactDock)
};

Q_DECLARE_METATYPE(ViewContactDock*)
