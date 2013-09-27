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

class QLabel;

class VideoGLFrame;
class VideoToolbar;

class VideoScene : public QGraphicsScene
{
   Q_OBJECT

public:
   VideoScene();

   void drawBackground(QPainter *painter, const QRectF &rect);

   //Setters
   void setToolbar(VideoToolbar* tb);
   void addFrame(VideoGLFrame* frame);

public slots:
   void setBackgroundColor();
   void frameChanged();

protected:
   void mousePressEvent(QGraphicsSceneMouseEvent *event);
   void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
   void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
   void wheelEvent(QGraphicsSceneWheelEvent * wheelEvent);

private:
   QList<VideoGLFrame*> m_lFrames ;

   QColor m_backgroundColor;
//    QGraphicsRectItem *m_lightItem;

   VideoToolbar* m_pToolbar;
};

#endif
