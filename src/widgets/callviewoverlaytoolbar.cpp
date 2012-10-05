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
#include <QtGui/QToolButton>
#include <QtGui/QPainter>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include <QtSvg/QSvgRenderer>
#include <QtGui/QHBoxLayout>
#include <QDebug> //TODO remove

//KDE
#include <KStandardDirs>
#include <KIcon>

//SFLPhone
#include <sflphone.h>

///Constructor
CallViewOverlayToolbar::CallViewOverlayToolbar(QWidget* parent) : QWidget(parent),m_pRightRender(0),m_pLeftRender(0)
{
   m_pRightRender = new QSvgRenderer( KStandardDirs::locate("data","sflphone-client-kde/overlay_right_corner.svg") );
   m_pLeftRender  = new QSvgRenderer( KStandardDirs::locate("data","sflphone-client-kde/overlay_left_corner.svg" ) );
   //setStyleSheet("background-color:red;");

   QHBoxLayout* layout = new QHBoxLayout(this);

   layout->addWidget(createButton("Hang Up" ,KIcon(KStandardDirs::locate("data","sflphone-client-kde/hangup_grayscale.png"))));
   layout->addWidget(createButton("Transfer",KIcon(KStandardDirs::locate("data","sflphone-client-kde/transfer_grayscale.png"))));
   layout->addWidget(createButton("Mute"    ,KIcon(KStandardDirs::locate("data","sflphone-client-kde/mutemic_grayscale.png"))));
   layout->addWidget(createButton("Record"  ,KIcon(KStandardDirs::locate("data","sflphone-client-kde/record_grayscale.png"))));
   layout->addWidget(createButton("Unhold"  ,KIcon(KStandardDirs::locate("data","sflphone-client-kde/unhold_grayscale.png"))));
   layout->addWidget(createButton("Refuse"  ,KIcon(KStandardDirs::locate("data","sflphone-client-kde/refuse_grayscale.png"))));

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
   QBrush b = customPainter.brush();
   QPen   p = customPainter.pen();

   b.setColor("black");
   customPainter.setOpacity(0.5);

   customPainter.setBrush (Qt::black);
   customPainter.setPen   (Qt::transparent);
   customPainter.drawRect (QRect(3,10,width()-6,height()-16)               );
   customPainter.drawPie  (QRect(width()-8-3,height()-10,8,8),270*16,90*16 );
   customPainter.drawPie  (QRect(3,height()-10,8,8),180*16,90*16           );
   customPainter.drawRect (QRect(5+2,height()-6,width()-8-6,3)             );

   m_pLeftRender->render (&customPainter,QRect( 3,0,10,10)                 );
   m_pRightRender->render(&customPainter,QRect( width()-13,0,10,10)        );
}

///Create a toolbar button
QToolButton* CallViewOverlayToolbar::createButton(const QString& text,const KIcon& icon)
{
   QToolButton* b = new QToolButton(this);
   b->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
   b->setIconSize(QSize(28,28));
   b->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
   b->setText(text);
   b->setIcon(icon);
   b->setShortcut(0);
   b->setStyleSheet("margin-bottom:0px;margin-top:7px;font-weight:bold;background-color:transparent;color:white;padding-top:5px;");
   return b;
}