/***************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                         *
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
#include "videowidget3.h"

//Qt
#include <QtCore/QDebug>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLFormat>
#include <QtGui/QResizeEvent>

//System
#include <math.h>
#include <GL/glu.h>

//SFLPhone
#include <lib/videomodel.h>
#include "videoscene.h"
#include "videotoolbar.h"
#include "actioncollection.h"
#include "extendedaction.h"


#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif


VideoWidget3::VideoWidget3(QWidget *parent) : QGraphicsView(parent)
{
   QSizePolicy sp = sizePolicy();
   sp.setVerticalPolicy  ( QSizePolicy::Preferred );
   sp.setHorizontalPolicy( QSizePolicy::Preferred );
   sp.setHeightForWidth  ( true                   );
   sp.setWidthForHeight  ( true                   );
   setSizePolicy(sp);

   setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   setVerticalScrollBarPolicy  (Qt::ScrollBarAlwaysOff);

   m_pWdg = new QGLWidget(QGLFormat(QGL::SampleBuffers/*|QGL::AlphaChannel*/),this);
   setViewport(m_pWdg);
   setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

   m_pScene = new VideoScene();
   setScene(m_pScene);

//    m_pScene->setToolbar(tb);
   m_pScene->setSceneRect(0,0,width(),height());
}

VideoWidget3::~VideoWidget3()
{
   
}

void VideoWidget3::addRenderer(VideoRenderer* renderer)
{
   m_pWdg->makeCurrent();
   if (m_hFrames[renderer]) {
      m_pScene->addFrame(m_hFrames[renderer]);
      return;
   }
   if (renderer) {
      qDebug() << "NEW";
      VideoGLFrame* frm = new VideoGLFrame(m_pWdg);
      frm->setRenderer(renderer);
      connect(frm,SIGNAL(changed()),m_pScene,SLOT(frameChanged()));
      m_pScene->addFrame(frm);
      m_hFrames[renderer] = frm;
   }
}

void VideoWidget3::removeRenderer(VideoRenderer* renderer)
{
   Q_UNUSED(renderer)
   m_pScene->removeFrame(m_hFrames[renderer]);
}

void VideoWidget3::resizeEvent(QResizeEvent* event)
{
   m_pScene->setSceneRect(0,0,event->size().width(),event->size().height());
}



void VideoWidget3::slotRotateLeft()
{
   m_pScene->slotRotateLeft();
}

void VideoWidget3::slotRotateRight()
{
   m_pScene->slotRotateRight();
}

void VideoWidget3::slotShowPreview(bool show)
{
   if (VideoModel::instance()->isPreviewing() && show) {
      qDebug() << "show";
      addRenderer(VideoModel::instance()->previewRenderer());
      VideoGLFrame* frm = m_hFrames[VideoModel::instance()->previewRenderer()];
      if (frm) {
         frm->setScale(0.3);
         frm->setTranslationX(1.8);
         frm->setTranslationY(1.8);
//          frm->setRotY(60);
      }
   }
   else {
      qDebug() << "hide";
      removeRenderer(VideoModel::instance()->previewRenderer());
   }
}

void VideoWidget3::slotMuteOutgoindVideo(bool mute)
{
   if (VideoModel::instance()->isPreviewing() && mute)
      VideoModel::instance()->stopPreview();
   else {
      VideoModel::instance()->startPreview();
      if (ActionCollection::instance()->videoPreviewAction()->isChecked())
         slotShowPreview(true);
   }
}

