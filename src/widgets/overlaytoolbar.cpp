/***************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Valle <emmanuel.lepage@savoirfairelinux.com >*
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
#include "overlaytoolbar.h"

//Qt
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include <QtCore/QTimer>
#include <QtWidgets/QScrollBar>
#include <QtSvg/QSvgRenderer>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QStyle>

//Ring
#include "mainwindow.h"
#include <tip/tipmanager.h>
#include <QStandardPaths>

///Constructor
OverlayToolbar::OverlayToolbar(QWidget* parent) : QWidget(parent),m_pRightRender(nullptr),
   m_pLeftRender(nullptr),m_pForcedParent(nullptr),m_IconSize(-1)
{
   QPalette pal = palette();
   pal.setColor(QPalette::Background,Qt::transparent);
   pal.setColor(QPalette::Button    ,Qt::transparent);
   setPalette(pal);
   setAutoFillBackground(false);
   m_pRightRender = new QSvgRenderer( QStandardPaths::locate(QStandardPaths::GenericDataLocation, "ring-kde/overlay_right_corner.svg") );
   m_pLeftRender  = new QSvgRenderer( QStandardPaths::locate(QStandardPaths::GenericDataLocation, "ring-kde/overlay_left_corner.svg" ) );

   setMinimumSize(100,56);
   if (parent)
      parent->installEventFilter(this);
   setVisible(false);
   hideEvent(nullptr);

   m_pLayout = new QHBoxLayout(this);

   //All size signals before the show() call seem unreliable, as the application
   //is sure to be visible in the next loop cycle, wait for that and repaint
#if QT_VERSION >= 0x050400
   QTimer::singleShot(0,[this]() {
      resizeToolbar();
   });
#endif

} //OverlayToolbar

OverlayToolbar::~OverlayToolbar()
{
   delete m_pRightRender;
   delete m_pLeftRender;
}

///Repaint event
void OverlayToolbar::paintEvent(QPaintEvent* event)
{
   Q_UNUSED(event);
   QPainter customPainter(this);
   customPainter.setRenderHint(QPainter::Antialiasing);
   QBrush b = customPainter.brush();

   b.setColor(Qt::black);
   //Use the current style pixel metrics to do as well as possible to guess the right shape
   int margin = style()->pixelMetric(QStyle::PM_FocusFrameHMargin);
   customPainter.setOpacity( 0.5                                                             );
   customPainter.setBrush  ( Qt::black                                                       );
   customPainter.setPen    ( Qt::NoPen                                                       );
   customPainter.drawRect  ( QRect(margin,10,width()-2*margin,height()-10-2*margin - margin) );
   customPainter.drawPie   ( QRect(width()-8-margin,height()-10,8,8),270*16,90*16            );
   customPainter.drawPie   ( QRect(margin,height()-10,8,8),180*16,90*16                      );
   customPainter.drawRect  ( QRect(4+margin,height()-4-margin,width()-8-2*margin,4)          );

   m_pLeftRender->render   (&customPainter,QRect( margin,0,10,10)                            );
   m_pRightRender->render  (&customPainter,QRect( width()-10-margin,0,10,10)                 );
} //paintEvent

///Hide or show the toolbar and select visible actions
void OverlayToolbar::updateState()
{

} //updateState

void OverlayToolbar::hideEvent(QHideEvent *)
{
   const QWidget* p = m_pForcedParent?m_pForcedParent:parentWidget();
   if (p && p->property("tipManager").isValid()) {
      TipManager* manager = qvariant_cast<TipManager*>(p->property("tipManager"));
      if (manager)
         manager->setBottomMargin(0);
   }
   emit visibilityChanged(false);
}

void OverlayToolbar::showEvent(QShowEvent *)
{
   const QWidget* p = m_pForcedParent?m_pForcedParent:parentWidget();
   if (p && p->property("tipManager").isValid()) {
      TipManager* manager = qvariant_cast<TipManager*>(p->property("tipManager"));
      if (manager)
         manager->setBottomMargin(53);
   }
   emit visibilityChanged(true);
}

void OverlayToolbar::resizeToolbar()
{
   const QWidget* p = m_pForcedParent?m_pForcedParent:parentWidget();

   if (p) {
      int vOffset(0),wOffset(0);
      //If the parent has scrollbar, take this into account
      if (p->inherits("QAbstractScrollArea")) {
         const QAbstractScrollArea* scrl = static_cast< const QAbstractScrollArea*>(p);
         if (scrl && scrl->horizontalScrollBar()->isVisible())
            vOffset += scrl->horizontalScrollBar()->height();
         if (scrl && scrl->verticalScrollBar()->isVisible())
            wOffset += scrl->verticalScrollBar()->width();
      }
      resize(p->width()-wOffset,72);
      move(0,p->height()-72-vOffset);
      emit resized(p->width()-wOffset);
   }
}

bool OverlayToolbar::eventFilter(QObject *obj, QEvent *event)
{
   if (event->type() == QEvent::Resize) {
      resizeToolbar();
   }
   // standard event processing
   return QObject::eventFilter(obj, event);
}

void OverlayToolbar::setForcedParent(QWidget* parent)
{
   m_pForcedParent = parent;
   if (m_pForcedParent) {
      m_pForcedParent->installEventFilter(this);
      resizeToolbar();
   }
}

void OverlayToolbar::addWidget(QWidget* w)
{
   m_pLayout->addWidget(w);
}
