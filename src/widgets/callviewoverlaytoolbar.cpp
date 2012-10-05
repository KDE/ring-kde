/***************************************************************************
 *   Copyright (C) 2009-2012 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Valle <emmanuel.lepage@savoirfairelinux.com >*
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
#include "callviewoverlaytoolbar.h"

//Qt
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include <QtSvg/QSvgRenderer>
#include <QDebug> //TODO remove

//KDE
#include <KStandardDirs>

///Constructor
CallViewOverlayToolbar::CallViewOverlayToolbar(QWidget* parent) : QWidget(parent),m_pRightRender(0),m_pLeftRender(0)
{
   m_pRightRender = new QSvgRenderer( KStandardDirs::locate("data","sflphone-client-kde/overlay_right_corner.svg") );
   m_pLeftRender  = new QSvgRenderer( KStandardDirs::locate("data","sflphone-client-kde/overlay_left_corner.svg" ) );
   //setStyleSheet("background-color:red;");
   setMinimumSize(100,56);
}

///Resize event
void CallViewOverlayToolbar::resizeEvent(QResizeEvent* event)
{
   QWidget::resizeEvent(event);
}

///Repaint event
void CallViewOverlayToolbar::paintEvent(QPaintEvent* event)
{
   Q_UNUSED(event);
   QPainter customPainter(this);
   customPainter.setRenderHint(QPainter::Antialiasing);
   QBrush   b = customPainter.brush();
   QPen     p = customPainter.pen();

   b.setColor("black");

   customPainter.setBrush(Qt::black);
   customPainter.setPen(Qt::transparent);
   customPainter.drawRect(QRect(5,10,width()-8,height()-25));
   customPainter.drawRoundedRect(QRect(5,height()-17,width()-8,15),5,5);

   m_pLeftRender->render (&customPainter,QRect(5,0,10,10));
   m_pRightRender->render(&customPainter,QRect(width()-13,0,10,10));
}