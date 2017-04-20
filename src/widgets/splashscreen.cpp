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
#include "splashscreen.h"

// Qt
#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtSvg/QSvgRenderer>
#include <QtCore/QStandardPaths>

// KDE
#include <KAboutData>

class SplashScreenPrivate
{
public:

};

SplashScreen::SplashScreen(KAboutData* data, QWidget* parent) :
QSplashScreen(parent), d_ptr(new SplashScreenPrivate)
{
    Q_UNUSED(data)

    QPixmap pxm(640, 480);

    QPainter p(&pxm);
    p.fillRect(0,0,640,480, "#004d61");

    const int h = 640*0.366101694915;
    const int pad = (640-h)/4;

    QSvgRenderer r( QStandardPaths::locate(
        QStandardPaths::GenericDataLocation,
        QStringLiteral("ring-kde/beta_logo.svg")
    ));
    r.render(&p, QRect(0, pad, 640, h));

    setPixmap(pxm);
}

SplashScreen::~SplashScreen()
{
    delete d_ptr;
}
