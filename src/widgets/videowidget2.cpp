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
#include "videowidget2.h"

//Qt
#include <QtCore/QDebug>
#include <QtOpenGL/QGLFormat>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>

//System
#include <math.h>
#include <GL/glu.h>

//SFLPhone
#include <lib/video/videorenderer.h>
#include <lib/video/videomodel.h>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

class ThreadedPainter: public QObject {
   Q_OBJECT
public:
   friend class VideoWidget2;

   //Constructor
   ThreadedPainter(QGLWidget* wdg);
   virtual ~ThreadedPainter();

   //GL
   QPoint anchor             ;
   float  scale              ;
   float  rot_x, rot_y, rot_z;
   GLuint tile_list          ;
   bool   isRendering        ;

   //Render
   VideoRenderer* m_pRenderer;

private:
   //Attributes
   QGLWidget* m_pW ;
   QMutex     mutex;

   //Methods
   void saveGLState();
   void restoreGLState();

public Q_SLOTS:
   void draw           ();
   void reset          ();
   void rendererStopped();
   void rendererStarted();
};

///Constructor
ThreadedPainter::ThreadedPainter(QGLWidget* wdg) : QObject(), m_pRenderer(nullptr),
   m_pW(wdg), rot_x(0.0f),rot_y(0.0f),rot_z(0.0f),scale(0.8f),isRendering(false)
{
}

///Destructor
ThreadedPainter::~ThreadedPainter(){
//       QThread::currentThread()->quit();
}

///Reset the painter state
void ThreadedPainter::reset()
{
   m_pRenderer = nullptr;
}

///Notify the painter that the (SHM) renderer has stopped
void ThreadedPainter::rendererStopped()
{
   QMutexLocker locker(&mutex);
   isRendering = false;
}

///Notify the painter that the (SHM) renderer has started
void ThreadedPainter::rendererStarted()
{
   QMutexLocker locker(&mutex);
   m_pW->makeCurrent();
   isRendering = true;
}

///Draw the video stream on the GL canvas
void ThreadedPainter::draw()
{
   if (m_pRenderer && isRendering) {
//       QMutexLocker locker(&mutex);
      m_pRenderer->mutex()->lock();
      glClearColor(0,0,0,0);
      QPainter p(m_pW);

      // save the GL state set for QPainter
      saveGLState();

      GLuint texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_DECAL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_DECAL);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      const QSize size = m_pRenderer->size();
      if (size.width() && size.height() && m_pRenderer->rawData())
         gluBuild2DMipmaps(GL_TEXTURE_2D, 4, size.width(), size.height(), GL_BGRA, GL_UNSIGNED_BYTE, m_pRenderer->rawData());

      // draw into the GL widget
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
      glScalef(-1.0f*scale, -1.0f*scale, 1.0f*scale);
      glRotatef(rot_x, 1.0f, 0.0f, 0.0f);
      glRotatef(rot_y, 0.0f, 1.0f, 0.0f);
      glRotatef(rot_z, 0.0f, 0.0f, 1.0f);

      // draw background
      glPushMatrix();
      glScalef(1.7f, 1.7f, 1.7f);
      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      glCallList(tile_list);
      glPopMatrix();

      // restore the GL state that QPainter expects
      restoreGLState();

      // draw the overlayed text using QPainter
      p.setPen  (QColor(197 , 197 , 197 , 157));
      p.setBrush(QColor(197 , 197 , 197 , 127));
      p.drawRect(QRect (0   , 0   , m_pW->width(), 50 ));
      p.setPen(Qt::black);
      p.setBrush(Qt::NoBrush);
      const QString str1(tr("A simple OpenGL framebuffer object example."));
      const QString str2(tr("Use the mouse wheel to zoom, press buttons and move mouse to rotate, double-click to flip."));
      QFontMetrics fm(p.font());
      p.drawText(m_pW->width()/2 - fm.width(str1)/2, 20, str1);
      p.drawText(m_pW->width()/2 - fm.width(str2)/2, 20 + fm.lineSpacing(), str2);
      m_pRenderer->mutex()->unlock();
      p.setPen  (Qt::NoPen);
      p.setBrush(QColor(25 , 25    , 25    , 200));
      p.drawRect(QRect (0  , m_pW->height()-30 , m_pW->width(), 30 ));
   }
}

///Constructor
VideoWidget2::VideoWidget2(QWidget *parent)
   : QGLWidget(QGLFormat(QGL::SampleBuffers|QGL::AlphaChannel), parent),
   m_pPainter(new ThreadedPainter(this))
{
   makeCurrent();
//    Should work, does not
//    QThread* t = new QThread(this);
//    connect(t, SIGNAL(started()), m_pPainter, SLOT(rendererStarted()));
//    m_pPainter->moveToThread(t);
//    t->start();
   connect(this,SIGNAL(changed()),m_pPainter,SLOT(draw()));

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

   QSizePolicy sp = sizePolicy();
   sp.setVerticalPolicy(QSizePolicy::Preferred);
   sp.setHorizontalPolicy(QSizePolicy::Preferred);
   sp.setHeightForWidth(true);
   sp.setWidthForHeight(true);
   setSizePolicy(sp);
}

///Destructor
VideoWidget2::~VideoWidget2()
{
   QMutexLocker locker(&m_pPainter->mutex);
   glDeleteLists(m_pPainter->tile_list, 1);
}

void VideoWidget2::paintEvent(QPaintEvent *)
{
   //Handled in thread
}

void VideoWidget2::mousePressEvent(QMouseEvent *e)
{
   QMutexLocker locker(&m_pPainter->mutex);
   m_pPainter->anchor = e->pos();
}

void VideoWidget2::mouseMoveEvent(QMouseEvent *e)
{
   QMutexLocker locker(&m_pPainter->mutex);
   const QPoint diff = e->pos() - m_pPainter->anchor;
   if (e->buttons() & Qt::LeftButton) {
      m_pPainter->rot_x += diff.y()/5.0f;
      m_pPainter->rot_y += diff.x()/5.0f;
   } else if (e->buttons() & Qt::RightButton) {
      m_pPainter->rot_z += diff.x()/5.0f;
   }

   m_pPainter->anchor = e->pos();
   emit changed();
}

void VideoWidget2::wheelEvent(QWheelEvent *e)
{
   QMutexLocker locker(&m_pPainter->mutex);
   e->delta() > 0 ? m_pPainter->scale += m_pPainter->scale*0.1f : m_pPainter->scale -= m_pPainter->scale*0.1f;
   emit changed();
}

void ThreadedPainter::saveGLState()
{
   glPushAttrib(GL_ALL_ATTRIB_BITS);
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
}

void ThreadedPainter::restoreGLState()
{
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
   glPopAttrib();
}

///Set widget renderer
void VideoWidget2::setRenderer(VideoRenderer* renderer)
{
//    if (m_pPainter->m_pRenderer && m_pPainter->m_pRenderer->isRendering())
//       m_pPainter->rendererStopped();
   QMutexLocker locker(&m_pPainter->mutex);
   if (m_pPainter->m_pRenderer) {
      disconnect(m_pPainter->m_pRenderer,SIGNAL(frameUpdated()),m_pPainter,SLOT(draw()));
      disconnect(m_pPainter->m_pRenderer,SIGNAL(started()),m_pPainter,SLOT(rendererStarted()));
      disconnect(m_pPainter->m_pRenderer,SIGNAL(stopped()),m_pPainter,SLOT(rendererStopped()));
   }
   m_pPainter->m_pRenderer = renderer;
   if (m_pPainter->m_pRenderer) {
      connect(m_pPainter->m_pRenderer,SIGNAL(frameUpdated()),m_pPainter,SLOT(draw()));
      connect(m_pPainter->m_pRenderer,SIGNAL(started()),m_pPainter,SLOT(rendererStarted()));
      connect(m_pPainter->m_pRenderer,SIGNAL(stopped()),m_pPainter,SLOT(rendererStopped()));
      connect(m_pPainter->m_pRenderer,SIGNAL(destroyed()),m_pPainter,SLOT(reset()));
      setSizeIncrement(1,((float)m_pPainter->m_pRenderer->size().height()/(float)m_pPainter->m_pRenderer->size().width()));
      if (m_pPainter->thread()->isRunning())
         m_pPainter->isRendering = true;
   }
}

///Force widget aspect ratio
int VideoWidget2::heightForWidth( int w ) const
{
   if (m_pPainter->m_pRenderer  )
   if (m_pPainter->m_pRenderer)
      return w*((float)m_pPainter->m_pRenderer->size().height()/(float)m_pPainter->m_pRenderer->size().width());
   return w*.75f;
}

QSize VideoWidget2::sizeHint() const
{
   if (m_pPainter->m_pRenderer) {
      return m_pPainter->m_pRenderer->size();
   }
   return QSize(100,75);
}

#include "videowidget2.moc"
#include "moc_videowidget2.cpp"
