#include <QtGui/QImage>
#include "videowidget2.h"

#include <QtCore/QDebug>

#include <math.h>

#include <lib/videorenderer.h>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

 VideoWidget2::VideoWidget2(QWidget *parent)
     : QGLWidget(QGLFormat(QGL::SampleBuffers|QGL::AlphaChannel), parent),
     rot_x(0.0f),rot_y(0.0f),rot_z(0.0f),scale(0.1f),m_pRenderer(nullptr)
 {
   makeCurrent();
   if (QGLFramebufferObject::hasOpenGLFramebufferBlit()) {
      QGLFramebufferObjectFormat format;
      format.setSamples(4);
      format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);

      render_fbo = new QGLFramebufferObject(512, 512, format);
   } else {
      render_fbo = new QGLFramebufferObject(1024, 1024);
   }

   m_Image = new QImage("/home/etudiant/sflphone8.png");
//      m_Image = m_Image->convertToFormat(QImage::Format_ARGB32);

   tile_list = glGenLists(1);
   glNewList(tile_list, GL_COMPILE);
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

VideoWidget2::~VideoWidget2()
{
   glDeleteLists(tile_list, 1);
   delete render_fbo;
}

void VideoWidget2::paintEvent(QPaintEvent *)
{
   draw();
}

void VideoWidget2::draw()
{
   glClearColor(0,0,0,0);
   QPainter p(this);

   // save the GL state set for QPainter
   saveGLState();

   // render the 'bubbles.svg' file into our framebuffer object
   QPainter fbo_painter(render_fbo);
   fbo_painter.drawImage(0,0,*m_Image);
   fbo_painter.end();

   // draw into the GL widget
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(-1, 1, -1, 1, 10, 100);
   glTranslatef(0.0f, 0.0f, -15.0f);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glViewport(0, 0, width(), height());
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glEnable(GL_TEXTURE_2D);
   glEnable(GL_MULTISAMPLE);
   glEnable(GL_CULL_FACE);
   glScalef(1.0f*scale, 1.0f*scale, 1.0f*scale);
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
   p.setPen  (QColor(197 , 197    , 197    , 157));
   p.setBrush(QColor(197 , 197    , 197    , 127));
   p.drawRect(QRect (0   , 0      , width(), 50 ));
   p.setPen(Qt::black);
   p.setBrush(Qt::NoBrush);
   const QString str1(tr("A simple OpenGL framebuffer object example."));
   const QString str2(tr("Use the mouse wheel to zoom, press buttons and move mouse to rotate, double-click to flip."));
   QFontMetrics fm(p.font());
   p.drawText(width()/2 - fm.width(str1)/2, 20, str1);
   p.drawText(width()/2 - fm.width(str2)/2, 20 + fm.lineSpacing(), str2);
}

void VideoWidget2::mousePressEvent(QMouseEvent *e)
{
   anchor = e->pos();
}

void VideoWidget2::mouseMoveEvent(QMouseEvent *e)
{
   QPoint diff = e->pos() - anchor;
   if (e->buttons() & Qt::LeftButton) {
      rot_x += diff.y()/5.0f;
      rot_y += diff.x()/5.0f;
   } else if (e->buttons() & Qt::RightButton) {
      rot_z += diff.x()/5.0f;
   }

   anchor = e->pos();
   draw();
}

void VideoWidget2::wheelEvent(QWheelEvent *e)
{
   e->delta() > 0 ? scale += scale*0.1f : scale -= scale*0.1f;
   draw();
}

void VideoWidget2::saveGLState()
{
   glPushAttrib(GL_ALL_ATTRIB_BITS);
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
}

void VideoWidget2::restoreGLState()
{
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
   glPopAttrib();
}

///Called when a new frame is ready
void VideoWidget2::updateFrame()
{
   const QSize size(m_pRenderer->activeResolution().width(), m_pRenderer->activeResolution().height());
   if (size != minimumSize())
      setMinimumSize(size);
   if (m_Image)
      delete m_Image;
   //if (!m_Image && VideoModel::instance()->isRendering())
      m_Image = new QImage((uchar*)m_pRenderer->rawData() , size.width(), size.height(), QImage::Format_ARGB32 );
   //This is the right way to do it, but it does not work
//    if (!m_Image || (m_Image && m_Image->size() != size))
//       m_Image = new QImage((uchar*)VideoModel::instance()->rawData() , size.width(), size.height(), QImage::Format_ARGB32 );
//    if (!m_Image->loadFromData(VideoModel::instance()->getCurrentFrame())) {
//       qDebug() << "Loading image failed";
//    }
   repaint();
}

///Prevent the painter to try to paint an invalid framebuffer
void VideoWidget2::stop()
{
   if (m_Image) {
      delete m_Image;
      m_Image = nullptr;
   }
}


///Set widget renderer
void VideoWidget2::setRenderer(VideoRenderer* renderer)
{
   if (m_pRenderer)
      disconnect(m_pRenderer,SIGNAL(frameUpdated()),this,SLOT(updateFrame()));
   m_pRenderer = renderer;
   connect(m_pRenderer,SIGNAL(frameUpdated()),this,SLOT(updateFrame()));
}

///Repaint the widget
void VideoWidget2::update() {
   QPainter painter(this);
   if (m_Image && m_pRenderer->isRendering())
      painter.drawImage(QRect(0,0,width(),height()),*(m_Image));
   painter.end();
}
