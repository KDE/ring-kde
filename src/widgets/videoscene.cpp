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

#include <QtGui>
#include <QtOpenGL>
#include <GL/glu.h>

#include "videoglframe.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

QDialog *VideoScene::createDialog(const QString &windowTitle) const
{
   QDialog *dialog = new QDialog(0, Qt::CustomizeWindowHint | Qt::WindowTitleHint);

   dialog->setWindowOpacity(0.8);
   dialog->setWindowTitle(windowTitle);
   dialog->setLayout(new QVBoxLayout);

   return dialog;
}

VideoScene::VideoScene()
   : m_backgroundColor(0, 170, 255)
   , m_lastTime(0)
   , m_distance(1.4f)
   , m_angularMomentum(0, 40, 0)
{
   QWidget *controls = createDialog(tr("Controls"));

   QCheckBox *wireframe = new QCheckBox(tr("Render as wireframe"));
   connect(wireframe, SIGNAL(toggled(bool)), this, SLOT(enableWireframe(bool)));
   controls->layout()->addWidget(wireframe);

   QCheckBox *normals = new QCheckBox(tr("Display normals vectors"));
   connect(normals, SIGNAL(toggled(bool)), this, SLOT(enableNormals(bool)));
   controls->layout()->addWidget(normals);

   QPushButton *colorButton = new QPushButton(tr("Choose model color"));
   controls->layout()->addWidget(colorButton);

   QPushButton *backgroundButton = new QPushButton(tr("Choose background color"));
   connect(backgroundButton, SIGNAL(clicked()), this, SLOT(setBackgroundColor()));
   controls->layout()->addWidget(backgroundButton);

   QWidget *statistics = createDialog(tr("Model info"));
   statistics->layout()->setMargin(20);

   for (int i = 0; i < 4; ++i) {
      m_labels[i] = new QLabel;
      statistics->layout()->addWidget(m_labels[i]);
   }

   QWidget *instructions = createDialog(tr("Instructions"));
   instructions->layout()->addWidget(new QLabel(tr("Use mouse wheel to zoom model, and click and drag to rotate model")));
   instructions->layout()->addWidget(new QLabel(tr("Move the sun around to change the light position")));

   addWidget(instructions);
   addWidget(controls);
   addWidget(statistics);

   QPointF pos(10, 10);
   foreach (QGraphicsItem *item, items()) {
      item->setFlag(QGraphicsItem::ItemIsMovable);
      item->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

      const QRectF rect = item->boundingRect();
      item->setPos(pos.x() - rect.x(), pos.y() - rect.y());
      pos += QPointF(0, 10 + rect.height());
   }

   QRadialGradient gradient(40, 40, 40, 40, 40);
   gradient.setColorAt(0.2, Qt::yellow);
   gradient.setColorAt(1, Qt::transparent);

   m_lightItem = new QGraphicsRectItem(0, 0, 80, 80);
   m_lightItem->setPen(Qt::NoPen);
   m_lightItem->setBrush(gradient);
   m_lightItem->setFlag(QGraphicsItem::ItemIsMovable);
   m_lightItem->setPos(800, 200);
   addItem(m_lightItem);
   m_time.start();
}

void VideoScene::drawBackground(QPainter *painter, const QRectF &)
{
   if (painter->paintEngine()->type() != QPaintEngine::OpenGL && painter->paintEngine()->type() != QPaintEngine::OpenGL2) {
      qDebug() << "VideoScene: drawBackground needs a QGLWidget to be set as viewport on the graphics view" << painter->paintEngine()->type();
      return;
   }
/*
   glClearColor(m_backgroundColor.redF(), m_backgroundColor.greenF(), m_backgroundColor.blueF(), 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluPerspective(70, width() / height(), 0.01, 1000);

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
#pragma GCC diagnostic ignored "-pedantic"
//    char m_Data[];
   const float pos[] = { m_lightItem->x() - width() / 2, height() / 2 - m_lightItem->y(), 512, 0 };
   glLightfv(GL_LIGHT0, GL_POSITION, pos);
   glColor4f(m_backgroundColor.redF(), m_backgroundColor.greenF(), m_backgroundColor.blueF(), 1.0f);

   const int delta = m_time.elapsed() - m_lastTime;
   m_rotation += m_angularMomentum * (delta / 1000.0);
   m_lastTime += delta;
#pragma GCC diagnostic pop

   glTranslatef(0, 0, -m_distance);
   glRotatef(m_rotation.x, 1, 0, 0);
   glRotatef(m_rotation.y, 0, 1, 0);
   glRotatef(m_rotation.z, 0, 0, 1);

   glEnable(GL_MULTISAMPLE);
   glDisable(GL_MULTISAMPLE);

   glPopMatrix();

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();*/

   QTimer::singleShot(20, this, SLOT(update()));
   foreach(VideoGLFrame* frm, m_lFrames) {
      qDebug() << "Drawing frame";
      frm->paintEvent(painter);
   }
}

void VideoScene::setBackgroundColor()
{
   const QColor color = QColorDialog::getColor(m_backgroundColor);
   if (color.isValid()) {
      m_backgroundColor = color;
      update();
   }
}

void VideoScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
   QGraphicsScene::mouseMoveEvent(event);
   if (event->isAccepted())
      return;
   if (event->buttons() & Qt::LeftButton) {
      const QPointF delta = event->scenePos() - event->lastScenePos();
      const Point3d angularImpulse = Point3d(delta.y(), delta.x(), 0) * 0.1;

      m_rotation += angularImpulse;
      m_accumulatedMomentum += angularImpulse;

      event->accept();
      update();
   }
   foreach(VideoGLFrame* frm, m_lFrames) {
      const QPointF diff = event->pos() - frm->anchor();
      if (event->buttons() & Qt::LeftButton) {
         frm->setRotX(frm->rotX()+diff.y()/5.0f);
         frm->setRotY(frm->rotY()+diff.x()/5.0f);
      } else if (event->buttons() & Qt::RightButton) {
         frm->setRotZ(frm->rotZ()+diff.x()/5.0f);
      }

      frm->setAnchor(event->pos());
   }
}

void VideoScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
   QGraphicsScene::mousePressEvent(event);
   if (event->isAccepted())
      return;

   m_mouseEventTime = m_time.elapsed();
   m_angularMomentum = m_accumulatedMomentum = Point3d();
   event->accept();

   foreach(VideoGLFrame* frm, m_lFrames) {
      frm->setAnchor(event->pos());
   }
}

void VideoScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
   QGraphicsScene::mouseReleaseEvent(event);
   if (event->isAccepted())
      return;

   const int delta = m_time.elapsed() - m_mouseEventTime;
   m_angularMomentum = m_accumulatedMomentum * (1000.0 / qMax(1, delta));
   event->accept();
   update();
}

void VideoScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
   QGraphicsScene::wheelEvent(event);
   if (event->isAccepted())
      return;

   m_distance *= qPow(1.2, -event->delta() / 120);
   event->accept();
   foreach(VideoGLFrame* frm, m_lFrames) {
      frm->setScale(frm->scale() +(event->delta() > 0 ?1:-1)*frm->scale()*0.1f);
   }
   update();
}
