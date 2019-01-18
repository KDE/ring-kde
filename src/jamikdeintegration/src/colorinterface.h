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
#pragma once

// Qt
#include <QtGui/QPalette>

// LibRingQt
#include <interfaces/accountlistcolorizeri.h>

class ColorInterface final : public QObject, public Interfaces::AccountListColorizerI
{
   Q_OBJECT
public:
    explicit ColorInterface();

    ColorInterface(const ColorInterface&) = delete;
    ColorInterface& operator=(const ColorInterface&) = delete;

    virtual Q_INVOKABLE QVariant color(const Account* a) override;

    virtual Q_INVOKABLE QVariant icon(const Account* a) override;
private:
    QPalette m_Pal;
    QColor   m_Green;
    QColor   m_Yellow;
    QColor   m_Red;
};
