/***************************************************************************
 *   Copyright (C) 2011-2014 by Savoir-Faire Linux                         *
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
#include "videowidget.h"

//KDE
#include <KDebug>

//SFLPhone
#include "../lib/videorenderer.h"

///Constructor
VideoWidget::VideoWidget(QWidget* parent ,VideoRenderer* renderer) : QWidget(parent),m_Image(nullptr),m_pRenderer(renderer) {
   setMinimumSize(200,200);
   setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   connect(m_pRenderer,SIGNAL(frameUpdated()),this,SLOT(updateFrame()));
   connect(VideoModel::instance(),SIGNAL(videoStopped()),this,SLOT(stop()));
   connect(VideoModel::instance(),SIGNAL(videoCallInitiated(VideoRenderer*)),this,SLOT(setRenderer(VideoRenderer*)));
}

///Set widget renderer
void VideoWidget::setRenderer(VideoRenderer* renderer)
{
   disconnect(m_pRenderer,SIGNAL(frameUpdated()),this,SLOT(updateFrame()));
   m_pRenderer = renderer;
   connect(m_pRenderer,SIGNAL(frameUpdated()),this,SLOT(updateFrame()));
}

///Repaint the widget
void VideoWidget::update() {
   QPainter painter(this);
   if (m_Image && m_pRenderer->isRendering())
      painter.drawImage(QRect(0,0,width(),height()),*(m_Image));
   painter.end();
}

///Called when the widget need repainting
void VideoWidget::paintEvent(QPaintEvent* event)
{
   Q_UNUSED(event)
   update();
}

///Called when a new frame is ready
void VideoWidget::updateFrame()
{
   const QSize size(m_pRenderer->activeResolution().width(), m_pRenderer->activeResolution().height());
   if (size != minimumSize())
      setMinimumSize(size);
   if (m_Image)
      delete m_Image;
   m_Image = new QImage((uchar*)m_pRenderer->rawData() , size.width(), size.height(), QImage::Format_ARGB32 );
   repaint();
}

///Prevent the painter to try to paint an invalid framebuffer
void VideoWidget::stop()
{
   if (m_Image) {
      delete m_Image;
      m_Image = nullptr;
   }
}
