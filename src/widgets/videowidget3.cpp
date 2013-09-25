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
#include "videowidget3.h"

#include <QtCore/QDebug>
#include <QtOpenGL>

#include <math.h>

#include <lib/videorenderer.h>
#include <lib/videomodel.h>
#include "videoscene.h"
#include "videoglframe.h"

#include <GL/glu.h>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif


VideoWidget3::VideoWidget3(QWidget *parent) : QGraphicsView(parent)
{
   QSizePolicy sp = sizePolicy();
   sp.setVerticalPolicy(QSizePolicy::Preferred);
   sp.setHorizontalPolicy(QSizePolicy::Preferred);
   sp.setHeightForWidth(true);
   sp.setWidthForHeight(true);
   setSizePolicy(sp);

   m_pWdg = new QGLWidget(QGLFormat(QGL::SampleBuffers/*|QGL::AlphaChannel*/),this);
   setViewport(m_pWdg);
   setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
   m_pScene = new VideoScene();
   setScene(m_pScene);
}

VideoWidget3::~VideoWidget3()
{
   
}

//    virtual int    VideoWidget3::heightForWidth( int w ) const;
//    virtual QSize  VideoWidget3::sizeHint      (       ) const;

void VideoWidget3::addRenderer(VideoRenderer* renderer)
{
   if (renderer) {
      VideoGLFrame* frm = new VideoGLFrame(m_pWdg);
      frm->setRenderer(renderer);
      m_pScene->m_lFrames << frm;
   }
}
