/***************************************************************************
 *   Copyright (C) 2015-2017 by Emmanuel Lepage Vallee                     *
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

#include <KXmlGuiWindow>

//Qt
class QToolButton;

class FancyMainWindow : public KXmlGuiWindow
{
    Q_OBJECT

public:
    explicit FancyMainWindow(QWidget* parent = nullptr);
    virtual ~FancyMainWindow();

    void setActive(bool a);

protected:
   virtual bool eventFilter(QObject *obj, QEvent *event) override;

private:
   QHash<QTabBar*, QToolButton*> m_hEventFilters;
   bool m_IsActive {true};

public Q_SLOTS:
    void updateTabIcons();
    void showPhone();

Q_SIGNALS:
    void unregisterWindow();
};
