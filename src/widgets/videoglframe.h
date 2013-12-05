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
#ifndef VIDEOGLFRAME_H
#define VIDEOGLFRAME_H

class VideoRenderer;

class ThreadedPainter2;
class QGLWidget;
class QPainter;

class VideoGLFrame : public QObject
{
   Q_OBJECT

public:
   explicit VideoGLFrame(QGLWidget *parent = nullptr);
   ~VideoGLFrame();

   void paintEvent     (QPainter* painter);
//    void mousePressEvent( QMouseEvent *);
//    void mouseMoveEvent ( QMouseEvent *);
//    void wheelEvent     ( QWheelEvent *);

   virtual int    heightForWidth( int w ) const;
   virtual QSize  sizeHint      (       ) const;

   //Setters
   void setRotZ(float rot);
   void setRotY(float rot);
   void setRotX(float rot);
   void setScale(float scale);
   void setAnchor(const QPointF& point);

   //Getter
   QPointF anchor() const;
   float rotZ() const;
   float rotY() const;
   float rotX() const;
   float scale() const;

private:
   //Attributes
   ThreadedPainter2* m_pPainter;
   QGLWidget* m_pParent;

public Q_SLOTS:
   void setRenderer(VideoRenderer* renderer = nullptr);

private Q_SLOTS:
   void slotEmitChanged();

Q_SIGNALS:
   void changed();
};

#endif
