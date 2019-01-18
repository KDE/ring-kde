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
#include "colorinterface.h"

// Qt
#include <QtGui/QIcon>
#include <QtGui/QGuiApplication>

// RingQt
#include <account.h>

ColorInterface::ColorInterface() : m_Pal(QGuiApplication::palette())
{
    m_Green = QColor(m_Pal.color(QPalette::Base));

    if (m_Green.green()+20 >= 255) {
        m_Green.setRed ( ((int)m_Green.red()  -20));
        m_Green.setBlue( ((int)m_Green.blue() -20));
    }
    else
        m_Green.setGreen(((int)m_Green.green()+20));

    m_Red = QColor(m_Pal.color(QPalette::Base));

    if (m_Red.red()+20 >= 255) {
        m_Red.setGreen(  ((int)m_Red.green()  -20));
        m_Red.setBlue(   ((int)m_Red.blue()   -20));
    }
    else
        m_Red.setRed(    ((int)m_Red.red()     +20));

    m_Yellow = QColor(m_Pal.color(QPalette::Base));

    if (m_Yellow.red()+20 >= 255 || m_Green.green()+20 >= 255) {
        m_Yellow.setBlue(((int)m_Yellow.blue() -20));
    }
    else {
        m_Yellow.setGreen(((int)m_Yellow.green()+20));
        m_Yellow.setRed( ((int)m_Yellow.red()   +20));
    }
}

QVariant ColorInterface::color(const Account* a)
{
    switch(a->registrationState()) {
        case Account::RegistrationState::READY:
            return m_Green;
        case Account::RegistrationState::UNREGISTERED:
            return m_Pal.color(QPalette::Base);
        case Account::RegistrationState::TRYING:
        case Account::RegistrationState::INITIALIZING:
            return m_Yellow;
        case Account::RegistrationState::ERROR:
            return m_Red;
        case Account::RegistrationState::COUNT__:
            break;
    };

    return QVariant();
}

QVariant ColorInterface::icon(const Account* a)
{
    if (a->editState() == Account::EditState::MODIFIED_COMPLETE)
        return QIcon::fromTheme(QStringLiteral("document-save"));

    if (a->editState() == Account::EditState::MODIFIED_INCOMPLETE)
        return QIcon::fromTheme(QStringLiteral("dialog-warning"));

    if (a->editState() == Account::EditState::OUTDATED)
        return QIcon::fromTheme(QStringLiteral("view-refresh"));

    return QVariant();
}
