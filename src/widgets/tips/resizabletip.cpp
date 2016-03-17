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
#include "resizabletip.h"

//Qt
#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QAbstractScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>

//Constants
static const int TOOLBAR_HEIGHT = 72;
static const int MARGINS        = 15;

class Handle : public QWidget
{
   Q_OBJECT
public:
   Handle(ResizableTip* parent = nullptr) : QWidget(parent), m_IsPressed(false),m_Dy(0),m_pParent(parent) {
      installEventFilter(this);
      setMinimumSize(0,8);
      setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
   }
   virtual ~Handle() {}

protected:

   virtual void paintEvent(QPaintEvent* event) override {
      Q_UNUSED(event)
      static const QColor dotCol = QApplication::palette().base().color();
      static const QColor hoverBg = QApplication::palette().highlight().color();
      QPainter p(this);
      p.setBrush(m_IsPressed? hoverBg : dotCol);
      p.setPen(Qt::transparent);
      p.setRenderHint(QPainter::Antialiasing,true);
      p.drawEllipse(width()/2-12,2,5,5);
      p.drawEllipse(width()/2-2,2,5,5);
      p.drawEllipse(width()/2+8,2,5,5);
   }

   virtual bool eventFilter(QObject *obj, QEvent *event) override {
      #pragma GCC diagnostic push
      #pragma GCC diagnostic ignored "-Wswitch-enum"
      switch(event->type()) {
         case QEvent::HoverMove:
         case QEvent::MouseMove:
            if (m_IsPressed) {
               const int y = static_cast<QMouseEvent*>(event)->y();
               int height  = m_pParent->height()+(m_Dy-y);
               if (height < 50)
                  height = 50;
               m_pParent->setMinimumSize(0,height);
               m_pParent->resize(m_pParent->width(),height);
               m_pParent->move(m_pParent->x(),m_pParent->y()-(m_Dy-y));
               m_Dy = y;
            }
            break;
         case QEvent::MouseButtonPress:
            m_Dy = static_cast<QMouseEvent*>(event)->y();
            m_IsPressed = true;
            grabMouse();
            break;
         case QEvent::MouseButtonRelease:
            m_IsPressed = false;
            releaseMouse();
            emit m_pParent->heightChanged(m_pParent->height());
            break;
         default:
            break;
      }
      #pragma GCC diagnostic pop
      return QObject::eventFilter(obj, event);
   }


private:
   bool m_IsPressed;
   int m_Dy;
   ResizableTip* m_pParent;
};

ResizableTip::ResizableTip(QWidget* parent) : QWidget(parent),m_pLayout(new QVBoxLayout(this))
{
   Handle* h = new Handle(this);
   m_pLayout->addWidget(h);
}

void ResizableTip::addWidget(QWidget* w)
{
   m_pLayout->addWidget(w);
}

bool ResizableTip::brightOrDarkBase()
{
   const QColor color = palette().base().color();
   return (color.red() > 128 && color.green() > 128 && color.blue() > 128);
}

void ResizableTip::paintEvent(QPaintEvent* event)
{
   QPainter customPainter(this);
   customPainter.setOpacity(0.1);
   customPainter.setPen(Qt::NoPen);
   customPainter.setRenderHint(QPainter::Antialiasing, true);
   customPainter.setBrush(QBrush(brightOrDarkBase()?Qt::black:Qt::white));
   customPainter.drawRoundedRect(0,0,width(),height(),10,10);
   QWidget::paintEvent(event);
}

bool ResizableTip::eventFilter(QObject *obj, QEvent *event)
{
   if (event->type() == QEvent::Resize) {
      const QWidget* p = parentWidget();

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
         resize(p->width()-wOffset-2*MARGINS,height());
         move(MARGINS,p->height()-TOOLBAR_HEIGHT-vOffset - MARGINS - height());
      }
   }
   // standard event processing
   return QObject::eventFilter(obj, event);
}

#include <resizabletip.moc>
