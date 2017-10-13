/***************************************************************************
 *   Copyright (C) 2017 by Emmanuel Lepage Vallee                          *
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

#include "fancymainwindow.h"

class RecentDock;
class ContactMethod;
class ViewContactDock;
class DockBase;
#include "timeline/viewcontactdock.h"

class ContactMethod;
class Person;

class TimelineWindow final : public FancyMainWindow
{
    Q_OBJECT

public:
    explicit TimelineWindow();
    virtual ~TimelineWindow();

    void setCurrentPage(ViewContactDock::Pages page);

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private:
    RecentDock* m_pPeersTimeline;
    ViewContactDock* m_pViewContact {nullptr};
    DockBase* m_pContactCD;

public Q_SLOTS:
    void viewContact(ContactMethod* cm);
    void viewPerson(Person* p);
};
