/***************************************************************************
 *   Copyright (C) 2011-2013 by Savoir-Faire Linux                         *
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
#include "delegatedropoverlay.h"

#include <QtGui/QPainter>
#include <KDebug>

#include <QtCore/QTimer>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QMimeData>
#include <QtGui/QApplication>
#include "lib/call.h"
#include <lib/abstractcontactbackend.h>

///Constructor
DelegateDropOverlay::DelegateDropOverlay(QObject* parent):QObject(parent),
m_pTimer(0),m_Init(false),m_Reverse(1),m_lpButtons(nullptr)
{
   const QColor color = QApplication::palette().base().color();
   const bool dark = (color.red() > 128 && color.green() > 128 && color.blue() > 128);
   if (dark)
      m_Pen.setColor(Qt::black);
   else
      m_Pen.setColor(Qt::white);
}

///Destructor
DelegateDropOverlay::~DelegateDropOverlay()
{
   if (m_pTimer) delete m_pTimer;
}

///How to paint
void DelegateDropOverlay::paintEvent(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index)
{
   if (!m_lpButtons)
      return;
   int step = index.data(AbstractContactBackend::Role::DropState).toInt();
   if ((step == 1 || step == -1) && m_lActiveIndexes.indexOf(index) == -1) {
      m_lActiveIndexes << index;
      //Create tge timer
      if (!m_pTimer) {
         m_pTimer = new QTimer(this);
         connect(m_pTimer, SIGNAL(timeout()), this, SLOT(changeVisibility()));
      }

      //Start it if it's nor already
      if (!m_pTimer->isActive()) {
         m_pTimer->start(10);
      }
   }
   int i =0;
   QMapIterator<QString, QImage*> it(*m_lpButtons);
   while (it.hasNext()) {
      it.next();
      if (step) {
         painter->save();
         painter->setRenderHint(QPainter::Antialiasing, true);
         painter->setPen(Qt::NoPen);
         const int tmpStep = (step>0)?step:15+step;
         painter->setBrush(QColor(125,125,125,(0.5*tmpStep*tmpStep)));
         const QRect buttonRect = QRect(option.rect.x()+(option.rect.width()/m_lpButtons->size())*i,option.rect.y(),option.rect.width()/m_lpButtons->size() - 10,option.rect.height());
         painter->drawRoundedRect(buttonRect, 10, 10);
         painter->setPen(m_Pen);

         if (it.value()) {
            painter->drawImage(QRect(buttonRect.x()+buttonRect.width()-(buttonRect.height()-10)-10/*padding*/,buttonRect.y()+5,(buttonRect.height()-10),(buttonRect.height()-10)),*it.value());
         }

         QFont font = painter->font();
         font.setBold(true);
         painter->setFont(font);
         painter->drawText (buttonRect, Qt::AlignVCenter|Qt::AlignHCenter, QString(it.key()).remove('&') );
         painter->restore();
         i++;
      }
   }
}//paintEvent

///Step by step animation
void DelegateDropOverlay::changeVisibility()
{
   foreach(const QModelIndex& idx, m_lActiveIndexes) {
      int step = idx.data(AbstractContactBackend::Role::DropState).toInt();
      //Remove items from the loop if there is no animation
      if (step >= 15 || step <= -15) {
         m_lActiveIndexes.removeAll(idx);
         if (step <= -15) //Hide the overlay
            ((QAbstractItemModel*)idx.model())->setData(idx,QVariant((int)0),AbstractContactBackend::Role::DropState);
      }
      else {
         //Update opacity
         if (step == 1)
            setHoverState(true);
         else if (step == -1)
            setHoverState(false);
         step+=(step>0)?1:-1;
         ((QAbstractItemModel*)idx.model())->setData(idx,QVariant((int)step),AbstractContactBackend::Role::DropState);
      }
   }
   //Stop loop if no animations are running
   if (!m_lActiveIndexes.size()) {
      m_pTimer->stop();
   }
}

///Set the state when the user hover the widget
///@note This is not called directly to avoid a Qt bug/limitation
void DelegateDropOverlay::setHoverState(bool hover)
{
   Q_UNUSED(hover)
//       if (hover)
//          m_Pen.setColor("black");
//       else
//          m_Pen.setColor("white");
}//setHoverState
