/***************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
#include "videowidget.h"

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


VideoWidget::VideoWidget(QWidget *parent, const bool previewOnly) : QGraphicsView(parent),m_pBackDevice(nullptr),m_IsPreviewOnly(previewOnly)
{
   connect(&Video::PreviewManager::instance(),&Video::PreviewManager::previewStateChanged,this,&VideoWidget::slotPreviewEnabled);
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

   if (Video::PreviewManager::instance().isPreviewing()) {
      slotShowPreview(true);
   }

//    m_pScene->setToolbar(tb);
   m_pScene->setSceneRect(0,0,width(),height());
   setAcceptDrops(true);
}

VideoWidget::~VideoWidget()
{
   delete m_pScene;
}

void VideoWidget::dragLeaveEvent( QDragLeaveEvent* e )
{
   Q_UNUSED(e)
}

void VideoWidget::dragEnterEvent( QDragEnterEvent* e )
{
   e->accept();
}

void VideoWidget::dragMoveEvent( QDragMoveEvent* e )
{
   e->accept();
}

void VideoWidget::dropEvent( QDropEvent* e )
{
   if (m_pSourceModel && e->mimeData()->hasFormat(QStringLiteral("text/uri-list"))) {
      m_pSourceModel->setFile(QUrl(e->mimeData()->data(QStringLiteral("text/uri-list"))));
   }
   e->accept();
}

void VideoWidget::setSourceModel(Video::SourceModel* model)
{
   m_pSourceModel = model;
}

void VideoWidget::addRenderer(Video::Renderer* renderer)
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
      connect(renderer,&Video::Renderer::frameUpdated,m_pScene,&VideoScene::frameChanged);
      m_pScene->addFrame(frm);
      m_hFrames[renderer] = frm;
   }
}

void VideoWidget::removeRenderer(Video::Renderer* renderer)
{
   Q_UNUSED(renderer)
   m_pScene->removeFrame(m_hFrames[renderer]);
}

void VideoWidget::resizeEvent(QResizeEvent* event)
{
   m_pScene->setSceneRect(0,0,event->size().width(),event->size().height());
}



void VideoWidget::slotRotateLeft()
{
   m_pScene->slotRotateLeft();
}

void VideoWidget::slotRotateRight()
{
   m_pScene->slotRotateRight();
}

void VideoWidget::slotShowPreview(bool show)
{
   ConfigurationSkeleton::setDisplayVideoPreview(show);
   if (Video::PreviewManager::instance().isPreviewing() && show) {
      addRenderer(Video::PreviewManager::instance().previewRenderer());
      VideoGLFrame* frm = m_hFrames[Video::PreviewManager::instance().previewRenderer()];
      if (frm) {
         frm->setScale(0.3f);
         frm->setTranslationX(1.8f);
         frm->setTranslationY(1.8f);
//          frm->setRotY(60);
      }
   }
   else {
      removeRenderer(Video::PreviewManager::instance().previewRenderer());
   }
}

void VideoWidget::slotMuteOutgoindVideo(bool mute)
{
   if (!m_pSourceModel)
       return;

   if (mute) {
      m_pBackDevice = Video::DeviceModel::instance().activeDevice();
      m_pSourceModel->switchTo(Video::SourceModel::ExtendedDeviceList::NONE);
   }
   else if (m_pBackDevice) {
      m_pSourceModel->switchTo(m_pBackDevice);
   }
}

void VideoWidget::slotKeepAspectRatio(bool keep)
{
   m_pScene->slotKeepAspectRatio(keep);
}


void VideoWidget::slotPreviewEnabled(bool show)
{
   if (!m_IsPreviewOnly && show && ActionCollection::instance()->videoPreviewAction()->isChecked()) {
      slotShowPreview(true);
   }
}

void VideoWidget::setPreviewOnly(bool prev)
{
   m_IsPreviewOnly = prev;
}
