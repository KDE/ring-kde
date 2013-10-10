/***************************************************************************
 *   Copyright (C) 2008 Nokia Corporation                                  *
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
#ifndef VIDEOSCENE_H
#define VIDEOSCENE_H

#include <QtGui/QGraphicsScene>

//SFLPhone
class VideoGLFrame;
class VideoToolbar;

class VideoScene : public QGraphicsScene
{
   Q_OBJECT
public:
   //Constructor
   VideoScene();

   //Mutator
   void drawBackground(QPainter *painter, const QRectF &rect);

   //Setters
   void setToolbar( VideoToolbar* tb    );
   void addFrame  ( VideoGLFrame* frame );

protected:
   //Events
   virtual void mousePressEvent  ( QGraphicsSceneMouseEvent* event      );
   virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent* event      );
   virtual void mouseMoveEvent   ( QGraphicsSceneMouseEvent* event      );
   virtual void wheelEvent       ( QGraphicsSceneWheelEvent* wheelEvent );

private:
   //Atributes
   QList<VideoGLFrame*> m_lFrames        ;
   QColor               m_backgroundColor;
   VideoToolbar*        m_pToolbar       ;
//    QGraphicsRectItem *m_lightItem;


public Q_SLOTS:
   void setBackgroundColor();
   void frameChanged      ();
};

#endif
