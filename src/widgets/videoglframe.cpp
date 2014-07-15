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
#include "videoglframe.h"

//Qt
#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/qglfunctions.h>

//System
#include <math.h>
#include <GL/glu.h>

//SFLPhone
#include <lib/video/videorenderer.h>
#include <lib/video/videomodel.h>


#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

class ThreadedPainter2: public QObject, protected QGLFunctions {
   Q_OBJECT
public:
   friend class VideoGLFrame;
   ThreadedPainter2(VideoGLFrame* frm,QGLWidget* wdg);
   virtual ~ThreadedPainter2(){
//       QThread::currentThread()->quit();
   }

   //GL
   QPointF anchor;
   float scale;
   float rot_x, rot_y, rot_z;
   float tra_x, tra_y, tra_z;
   GLuint tile_list;
   bool isRendering;

   //Render
   VideoRenderer* m_pRenderer;

private:
   QGLWidget* m_pW;
   VideoGLFrame* m_pFrm;
   QMutex mutex;
   char* m_Data;
   QSize m_ActiveSize;

   //Methods
   void saveGLState();
   void restoreGLState();

public Q_SLOTS:
   void draw(QPainter* p);
   void copyFrame();
   void reset();
   void rendererStopped();
   void rendererStarted();
Q_SIGNALS:
   void changed();
};

ThreadedPainter2::ThreadedPainter2(VideoGLFrame* frm,QGLWidget* wdg) : QObject(), m_pRenderer(nullptr),
   m_pW(wdg), rot_x(0.0f),rot_y(0.0f),rot_z(0.0f),scale(0.8f),isRendering(false),m_pFrm(frm),
   m_Data(nullptr),tile_list(0),tra_x(0.0f), tra_y(0.0f), tra_z(0.0f)
{
}

void ThreadedPainter2::reset()
{
   m_pRenderer = nullptr;
}

void ThreadedPainter2::rendererStopped()
{
   QMutexLocker locker(&mutex);
   isRendering = false;
}

void ThreadedPainter2::rendererStarted()
{
   QMutexLocker locker(&mutex);
   m_pW->makeCurrent();
   isRendering = true;
}

void ThreadedPainter2::copyFrame()
{
   if (m_pRenderer) {
      //TODO change the method name
      //This is a left over from how 1.2.3 and 1.3.0 cached a copy of the
      //buffer to avoid a nasty race condition now fixed in the daemon
      emit changed();
   }
}

void ThreadedPainter2::draw(QPainter* p)
{
   Q_UNUSED(p)
   if (m_pRenderer && isRendering) {

      // save the GL state set for QPainter
      saveGLState();

      GLuint texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

      //To give a bit of history about why this code is weird, it used to have another copy buffer
      //but this was removed and replaced with a flip buffer in libQt, there is some leftover
      //code that need to be cleaned
      const QByteArray& data = m_pRenderer->currentFrame();
      QSize res = m_pRenderer->size();

      //Detect race conditions
      const int expectedSize = res.width() * res.height();
      if (expectedSize == data.size()/3) {
         qWarning() << "Invalid video size. Expected" << expectedSize << "got" << data.size()/3;
         return;
      }

      if (res.width() && res.height() && data.size()) {
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, res.width(), res.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, data.data());
      }

      // draw into the GL widget
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glFrustum(-1, 1, -1, 1, 10, 100);
      glTranslatef(0.0f, 0.0f, -15.0f);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glViewport(0, 0, m_pW->width(), m_pW->height());
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_MULTISAMPLE);
      glEnable(GL_CULL_FACE);
      glBindTexture(GL_TEXTURE_2D, texture);
      glScalef(-1.0f*scale, -1.0f*scale, 1.0f*scale);

      glRotatef(rot_x, 1.0f, 0.0f, 0.0f);
      glRotatef(rot_y, 0.0f, 1.0f, 0.0f);
      glRotatef(rot_z, 0.0f, 0.0f, 1.0f);

      //Handle the ratio
      float xRatio(1),yRatio(1);
      bool invert = (((int)rot_z)/90)%2;

      if (m_pFrm->keepAspectRatio()) {
         float rendererRatio = ((float)res.width())/((float)res.height());
         float windowRatio = ((float)p->device()->width())/((float)p->device()->height());

         if (windowRatio > rendererRatio)
            xRatio = (1.0f/windowRatio) * rendererRatio;
         else
            yRatio = 1.0f/((1.0f/windowRatio) * rendererRatio);
      }

      // draw background
      glPushMatrix();
      glScalef((invert?yRatio:xRatio)*1.7f, (invert?xRatio:yRatio)*1.7f, 1.7f);
      glTranslatef(tra_x, tra_y, tra_z);
      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      glCallList(tile_list);
      glPopMatrix();

      // restore the GL state that QPainter expects
      restoreGLState();
      glDeleteTextures(1, &texture);
   }
   else {
      //Try to cleanup
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   }
}


 VideoGLFrame::VideoGLFrame(QGLWidget *parent)
     : QObject(parent),m_pParent(parent),
     m_pPainter(new ThreadedPainter2(this,parent)),
     m_pRenderer(nullptr),m_KeepAspect(true)
 {
   connect(m_pPainter,SIGNAL(changed()),this,SLOT(slotEmitChanged()));

   m_pPainter->tile_list = glGenLists(1);
   glNewList(m_pPainter->tile_list, GL_COMPILE);
   glBegin(GL_QUADS);
   {
      //Define the video plane
      glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
      glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
      glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
   }
   glEnd();
   glEndList();
 }

VideoGLFrame::~VideoGLFrame()
{
   QMutexLocker locker(&m_pPainter->mutex);
   glDeleteLists(m_pPainter->tile_list, 1);
}

void VideoGLFrame::paintEvent(QPainter* p)
{
   m_pPainter->draw(p);
}

void ThreadedPainter2::saveGLState()
{
   glPushAttrib(GL_ALL_ATTRIB_BITS);
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
}

void ThreadedPainter2::restoreGLState()
{
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
   glPopAttrib();
}

///Set widget renderer
void VideoGLFrame::setRenderer(VideoRenderer* renderer)
{
   m_pRenderer = renderer;
//    if (m_pPainter->m_pRenderer && m_pPainter->m_pRenderer->isRendering())
//       m_pPainter->rendererStopped();
   QMutexLocker locker(&m_pPainter->mutex);
   if (m_pPainter->m_pRenderer) {
      disconnect(m_pPainter->m_pRenderer,SIGNAL(frameUpdated()),m_pPainter,SLOT(copyFrame()));
      disconnect(m_pPainter->m_pRenderer,SIGNAL(started()),m_pPainter,SLOT(rendererStarted()));
      disconnect(m_pPainter->m_pRenderer,SIGNAL(stopped()),m_pPainter,SLOT(rendererStopped()));
   }
   m_pPainter->m_pRenderer = renderer;
   if (m_pPainter->m_pRenderer) {
      connect(m_pPainter->m_pRenderer,SIGNAL(frameUpdated()),m_pPainter,SLOT(copyFrame()));
      connect(m_pPainter->m_pRenderer,SIGNAL(started()),m_pPainter,SLOT(rendererStarted()));
      connect(m_pPainter->m_pRenderer,SIGNAL(stopped()),m_pPainter,SLOT(rendererStopped()));
      connect(m_pPainter->m_pRenderer,SIGNAL(destroyed()),m_pPainter,SLOT(reset()));
//       setSizeIncrement(1,((float)m_pPainter->m_pRenderer->size().height()/(float)m_pPainter->m_pRenderer->size().width()));
      if (m_pPainter->thread()->isRunning())
         m_pPainter->isRendering = true;
   }
}

///Force widget aspect ratio
int VideoGLFrame::heightForWidth( int w ) const
{
   if (m_pPainter->m_pRenderer  )
   if (m_pPainter->m_pRenderer)
      return w*((float)m_pPainter->m_pRenderer->size().height()/(float)m_pPainter->m_pRenderer->size().width());
   return w*.75f;
}

QSize VideoGLFrame::sizeHint() const
{
   if (m_pPainter->m_pRenderer) {
      return m_pPainter->m_pRenderer->size();
   }
   return QSize(100,75);
}

void VideoGLFrame::slotEmitChanged()
{
   emit changed();
}

void VideoGLFrame::setRotZ(float rot)
{
   m_pPainter->rot_z = rot;
}

void VideoGLFrame::setRotY(float rot)
{
   m_pPainter->rot_y = rot;
}

void VideoGLFrame::setRotX(float rot)
{
   m_pPainter->rot_x = rot;
}

void VideoGLFrame::setTranslationZ(float tra)
{
   m_pPainter->tra_z = tra;
}

void VideoGLFrame::setTranslationY(float tra)
{
   m_pPainter->tra_y = tra;
}

void VideoGLFrame::setTranslationX(float tra)
{
   m_pPainter->tra_x = tra;
}


void VideoGLFrame::setScale(float scale)
{
   QMutexLocker locker(&m_pPainter->mutex);
   m_pPainter->scale = scale;
}

void VideoGLFrame::setAnchor(const QPointF& point)
{
   m_pPainter->anchor = point;
}

QPointF VideoGLFrame::anchor() const
{
   return m_pPainter->anchor;
}

float VideoGLFrame::rotZ() const
{
   return m_pPainter->rot_z;
}

float VideoGLFrame::rotY() const
{
   return m_pPainter->rot_y;
}

float VideoGLFrame::rotX() const
{
   return m_pPainter->rot_x;
}

float VideoGLFrame::scale() const
{
   QMutexLocker locker(&m_pPainter->mutex);
   return m_pPainter->scale;
}

void VideoGLFrame::setKeepAspectRatio(bool keep)
{
   m_KeepAspect = keep;
}

bool VideoGLFrame::keepAspectRatio() const
{
   return m_KeepAspect;
}

#include "videoglframe.moc"
#include "moc_videoglframe.cpp"
