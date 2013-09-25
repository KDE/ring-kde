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

#ifndef OPENGLSCENE_H
#define OPENGLSCENE_H

#include "point3d.h"

#include <QGraphicsScene>
#include <QLabel>
#include <QTime>

#ifndef QT_NO_CONCURRENT
#include <QFutureWatcher>
#endif

class VideoGLFrame;

class VideoScene : public QGraphicsScene
{
   Q_OBJECT

public:
   VideoScene();

   void drawBackground(QPainter *painter, const QRectF &rect);
   QList<VideoGLFrame*> m_lFrames ;

public slots:
   void setBackgroundColor();

protected:
   void mousePressEvent(QGraphicsSceneMouseEvent *event);
   void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
   void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
   void wheelEvent(QGraphicsSceneWheelEvent * wheelEvent);

private:
   QDialog *createDialog(const QString &windowTitle) const;

   QColor m_backgroundColor;

   QTime m_time;
   int m_lastTime;
   int m_mouseEventTime;

   float m_distance;
   Point3d m_rotation;
   Point3d m_angularMomentum;
   Point3d m_accumulatedMomentum;

   QLabel *m_labels[4];

   QGraphicsRectItem *m_lightItem;
};

#endif
