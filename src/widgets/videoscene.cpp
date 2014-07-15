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
#include <QGraphicsSceneMouseEvent>

#include "videoglframe.h"
#include "videotoolbar.h"
#include <lib/video/videorenderer.h>
#include <lib/video/videomodel.h>
#include "klib/kcfg_settings.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

VideoScene::VideoScene()
   : m_backgroundColor(25, 25, 25),m_pPreviewFrame(nullptr)//,m_pToolbar(nullptr)
{
}

void VideoScene::drawBackground(QPainter *painter, const QRectF& rect)
{
   Q_UNUSED(rect)
   glClearColor(m_backgroundColor.redF(), m_backgroundColor.greenF(), m_backgroundColor.blueF(), 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   foreach(VideoGLFrame* frm, m_lFrames) {
      frm->paintEvent(painter);
   }
   if (m_pPreviewFrame) {
      m_pPreviewFrame->paintEvent(painter);
   }
}

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

void VideoScene::addFrame(VideoGLFrame* frame)
{
   if (frame->renderer() == VideoModel::instance()->previewRenderer())
      m_pPreviewFrame = frame;
   else
      m_lFrames << frame;
//    m_pToolbar->resizeToolbar();
   invalidate();
}

void VideoScene::removeFrame( VideoGLFrame* frame )
{
   if (!frame) return;
   if (frame == m_pPreviewFrame)
      m_pPreviewFrame = nullptr;
   else
      m_lFrames.removeAll(frame);
   invalidate();
}

void VideoScene::slotRotateLeft()
{
   foreach(VideoGLFrame* frm, m_lFrames) {
      if (VideoModel::instance()->previewRenderer() != frm->renderer())
         frm->setRotZ(frm->rotZ()+90);
   }
}

void VideoScene::slotRotateRight()
{
   foreach(VideoGLFrame* frm, m_lFrames) {
      if (VideoModel::instance()->previewRenderer() != frm->renderer())
         frm->setRotZ(frm->rotZ()-90);
   }
}

void VideoScene::slotKeepAspectRatio(bool keep)
{
   ConfigurationSkeleton::setKeepVideoAspectRatio(keep);
   foreach(VideoGLFrame* frm, m_lFrames) {
      if (VideoModel::instance()->previewRenderer() != frm->renderer())
         frm->setKeepAspectRatio(keep);
   }
}

