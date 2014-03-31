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

#include "videoscene.h"
#include <QtGui/QLabel>
#include <QtCore/QMutex>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <GL/glu.h>

#include "videoglframe.h"
#include "videotoolbar.h"
#include <lib/videorenderer.h>
#include <lib/videomodel.h>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

VideoScene::VideoScene()
   : m_backgroundColor(25, 25, 25)//,m_pToolbar(nullptr)
{

//    QPointF pos(10, 10);
//    foreach (QGraphicsItem *item, items()) {
//       item->setFlag(QGraphicsItem::ItemIsMovable);
//       item->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
// 
//       const QRectF rect = item->boundingRect();
//       item->setPos(pos.x() - rect.x(), pos.y() - rect.y());
//       pos += QPointF(0, 10 + rect.height());
//    }

//    QRadialGradient gradient(40, 40, 40, 40, 40);
//    gradient.setColorAt(0.2, Qt::yellow);
//    gradient.setColorAt(1, Qt::transparent);

//    m_lightItem = new QGraphicsRectItem(0, 0, 80, 80);
//    m_lightItem->setPen(Qt::NoPen);
//    m_lightItem->setBrush(gradient);
//    m_lightItem->setFlag(QGraphicsItem::ItemIsMovable);
//    m_lightItem->setPos(800, 200);
//    addItem(m_lightItem);
}

void VideoScene::drawBackground(QPainter *painter, const QRectF& rect)
{
   Q_UNUSED(rect)
   glClearColor(m_backgroundColor.redF(), m_backgroundColor.greenF(), m_backgroundColor.blueF(), 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   foreach(VideoGLFrame* frm, m_lFrames) {
         frm->paintEvent(painter);
   }
}

void VideoScene::setBackgroundColor()
{
//    const QColor color = QColorDialog::getColor(m_backgroundColor);
//    if (color.isValid()) {
//       m_backgroundColor = color;
//       update();
//    }
}

// void VideoScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
// {
//    QGraphicsScene::mouseMoveEvent(event);
//    if (event->isAccepted())
//       return;
//    if (event->buttons() & Qt::LeftButton) {
//       event->accept();
//       update();
//    }
//    foreach(VideoGLFrame* frm, m_lFrames) {
// //       const QPointF diff = event->pos() - frm->anchor();
//       if (event->buttons() & Qt::LeftButton) {
//          
//          frm->setRotZ(frm->rotZ()+1);
// //          frm->setRotY(frm->rotY()+1/5.0f);
// //       } else if (event->buttons() & Qt::RightButton) {
// //          frm->setRotZ(frm->rotZ()+diff.x()/5.0f);
//       }
// 
//       frm->setAnchor(event->pos());
//    }
// }

// void VideoScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
// {
//    QGraphicsScene::mousePressEvent(event);
//    if (event->isAccepted())
//       return;
// 
//    event->accept();
// 
//    foreach(VideoGLFrame* frm, m_lFrames) {
//       frm->setAnchor(event->pos());
//    }
// }

// void VideoScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
// {
//    QGraphicsScene::mouseReleaseEvent(event);
//    if (event->isAccepted())
//       return;
// 
//    event->accept();
//    update();
// }

void VideoScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
   QGraphicsScene::wheelEvent(event);
   if (event->isAccepted())
      return;

   event->accept();
   foreach(VideoGLFrame* frm, m_lFrames) {
      if (frm) {
         if (VideoModel::instance()->previewRenderer() != frm->renderer())
            frm->setScale(frm->scale() +(event->delta() > 0 ?1:-1)*frm->scale()*0.1f);
      }
   }
   update();
}

void VideoScene::frameChanged()
{
   update();
}

// void VideoScene::setToolbar(VideoToolbar* tb)
// {
//    m_pToolbar = tb;
//    addWidget(m_pToolbar);
// }

void VideoScene::addFrame(VideoGLFrame* frame)
{
   m_lFrames << frame;
//    m_pToolbar->resizeToolbar();
}

void VideoScene::removeFrame( VideoGLFrame* frame )
{
   m_lFrames.removeAll(frame);
}

void VideoScene::slotRotateLeft()
{
   foreach(VideoGLFrame* frm, m_lFrames) {
      if (VideoModel::instance()->previewRenderer() != frm->renderer())
         frm->setRotZ(frm->rotZ()-90);
   }
}

void VideoScene::slotRotateRight()
{
   foreach(VideoGLFrame* frm, m_lFrames) {
      if (VideoModel::instance()->previewRenderer() != frm->renderer())
         frm->setRotZ(frm->rotZ()+90);
   }
}

void VideoScene::slotShowPreview()
{
  
}

