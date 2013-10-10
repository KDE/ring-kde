/***************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                              *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
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
#ifndef VIDEOWIDGET2_H
#define VIDEOWIDGET2_H

#include <QtOpenGL>

//SFLPhone
class VideoRenderer  ;
class ThreadedPainter;

class VideoWidget2 : public QGLWidget
{
   Q_OBJECT

public:
   //Constructor
   explicit VideoWidget2(QWidget *parent);
   ~VideoWidget2();

   //Events
   virtual void paintEvent     ( QPaintEvent* e );
   virtual void mousePressEvent( QMouseEvent* e );
   virtual void mouseMoveEvent ( QMouseEvent* e );
   virtual void wheelEvent     ( QWheelEvent* e );

   virtual int    heightForWidth( int w ) const;
   virtual QSize  sizeHint      (       ) const;

private:
   //Attributes
   ThreadedPainter* m_pPainter;

public Q_SLOTS:
   void setRenderer(VideoRenderer* renderer = nullptr);

Q_SIGNALS:
   void changed();
};

#endif
