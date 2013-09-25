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
#include <QtGui/QLabel>
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
}

void VideoScene::drawBackground(QPainter *painter, const QRectF& rect)
{
   Q_UNUSED(rect)
   if (painter->paintEngine()->type() != QPaintEngine::OpenGL && painter->paintEngine()->type() != QPaintEngine::OpenGL2) {
      qDebug() << "VideoScene: drawBackground needs a QGLWidget to be set as viewport on the graphics view" << painter->paintEngine()->type();
      return;
   }

   glClearColor(m_backgroundColor.redF(), m_backgroundColor.greenF(), m_backgroundColor.blueF(), 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   foreach(VideoGLFrame* frm, m_lFrames) {
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

   event->accept();
   update();
}

void VideoScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
   QGraphicsScene::wheelEvent(event);
   if (event->isAccepted())
      return;

   event->accept();
   foreach(VideoGLFrame* frm, m_lFrames) {
      frm->setScale(frm->scale() +(event->delta() > 0 ?1:-1)*frm->scale()*0.1f);
   }
   update();
}

void VideoScene::frameChanged()
{
   update();
}

