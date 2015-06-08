/***************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                         *
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
#include <QtGui/QResizeEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>
#include <QtCore/QMimeData>
#include <QtOpenGL/QGLWidget>
#include <QtWidgets/QAction>

//System
#include <math.h>

#ifdef Q_OS_LINUX
 #include <GL/glu.h>
#else
 #include <OpenGL/glu.h>
#endif

//Ring
#include <video/sourcemodel.h>
#include "videoscene.h"
#include "videotoolbar.h"
#include "actioncollection.h"
#include <video/devicemodel.h>
#include <video/previewmanager.h>
#include "klib/kcfg_settings.h"


#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif


VideoWidget3::VideoWidget3(QWidget *parent, const bool previewOnly) : QGraphicsView(parent),m_pBackDevice(nullptr),m_IsPreviewOnly(previewOnly)
{
   connect(Video::PreviewManager::instance(),SIGNAL(previewStateChanged(bool)),this,SLOT(slotPreviewEnabled(bool)));
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

   if (Video::PreviewManager::instance()->isPreviewing()) {
      slotShowPreview(true);
   }

//    m_pScene->setToolbar(tb);
   m_pScene->setSceneRect(0,0,width(),height());
   setAcceptDrops(true);
}

VideoWidget3::~VideoWidget3()
{
   
}

void VideoWidget3::dragLeaveEvent( QDragLeaveEvent* e )
{
   Q_UNUSED(e)
}

void VideoWidget3::dragEnterEvent( QDragEnterEvent* e )
{
   e->accept();
}

void VideoWidget3::dragMoveEvent( QDragMoveEvent* e )
{
   e->accept();
}

void VideoWidget3::dropEvent( QDropEvent* e )
{
   if (e->mimeData()->hasFormat("text/uri-list")) {
      Video::SourceModel::instance()->setFile(QUrl(e->mimeData()->data("text/uri-list")));
   }
   e->accept();
}

void VideoWidget3::addRenderer(Video::Renderer* renderer)
{
   m_pWdg->makeCurrent();
   if (m_hFrames[renderer]) {
      m_pScene->addFrame(m_hFrames[renderer]);
      return;
   }
   if (renderer) {
      VideoGLFrame* frm = new VideoGLFrame(m_pWdg);
      frm->setKeepAspectRatio(ConfigurationSkeleton::keepVideoAspectRatio());
      frm->setRenderer(renderer);
      connect(frm,SIGNAL(changed()),m_pScene,SLOT(frameChanged()));
      m_pScene->addFrame(frm);
      m_hFrames[renderer] = frm;
   }
}

void VideoWidget3::removeRenderer(Video::Renderer* renderer)
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
   ConfigurationSkeleton::setDisplayVideoPreview(show);
   if (Video::PreviewManager::instance()->isPreviewing() && show) {
      addRenderer(Video::PreviewManager::instance()->previewRenderer());
      VideoGLFrame* frm = m_hFrames[Video::PreviewManager::instance()->previewRenderer()];
      if (frm) {
         frm->setScale(0.3);
         frm->setTranslationX(1.8);
         frm->setTranslationY(1.8);
//          frm->setRotY(60);
      }
   }
   else {
      removeRenderer(Video::PreviewManager::instance()->previewRenderer());
   }
}

void VideoWidget3::slotMuteOutgoindVideo(bool mute)
{
   if (mute) {
      m_pBackDevice = Video::DeviceModel::instance()->activeDevice();
      Video::SourceModel::instance()->switchTo(Video::SourceModel::ExtendedDeviceList::NONE);
   }
   else if (m_pBackDevice) {
      Video::SourceModel::instance()->switchTo(m_pBackDevice);
   }
}

void VideoWidget3::slotKeepAspectRatio(bool keep)
{
   m_pScene->slotKeepAspectRatio(keep);
}


void VideoWidget3::slotPreviewEnabled(bool show)
{
   if (!m_IsPreviewOnly && show && ActionCollection::instance()->videoPreviewAction()->isChecked()) {
      slotShowPreview(true);
   }
}

void VideoWidget3::setPreviewOnly(bool prev)
{
   m_IsPreviewOnly = prev;
}
