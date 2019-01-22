/***************************************************************************
 *   Copyright (C) 2017 by bluesystems                                     *
 *   Copyright (C) 2016 by Marat Moustafine                                *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
 *   Author : Marat Moustafine <moustafine@tuta.io>                        *
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

#include "welcome.h"

#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QtGui/QGuiApplication>
#include <QQuickWindow>

WelcomeDialog::WelcomeDialog(QObject* parent) : QObject(parent)
{

}

WelcomeDialog::~WelcomeDialog()
{

}

QString WelcomeDialog::defaultUserName() const
{
    return qgetenv("USER");
}

// kate: space-indent on; indent-width 4; replace-tabs on;
