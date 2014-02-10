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
#include <lib/abstractitembackend.h>
#include <lib/contactmodel.h>

///Constructor
DelegateDropOverlay::DelegateDropOverlay(QObject* parent):QObject(parent),
m_pTimer(0),m_Init(false),m_Reverse(1),m_lpButtons(nullptr)
{
   const QColor color = QApplication::palette().base().color();
   const bool dark = (color.red() > 128 && color.green() > 128 && color.blue() > 128);
   if (dark)
      m_Pen.setColor("#f3f3f3");
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
   static bool initSignals = false;
   if (!initSignals) {
      connect(index.model(),SIGNAL(layoutChanged()),this,SLOT(slotLayoutChanged()));
      initSignals = true;
   }
   int step = index.data(ContactModel::Role::DropState).toInt();
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
   QMapIterator<QString, OverlayButton*> it(*m_lpButtons);
   const int dropPosition = index.data(Call::Role::DropPosition).toInt();
   while (it.hasNext()) {
      it.next();
      if (step) {
         const bool highlight = dropPosition == it.value()->role;
         const int tmpStep = (step>0)?step:15+step;
         painter->save();
         painter->setOpacity(1);
         painter->setRenderHint(QPainter::Antialiasing, true);
         QPen pen = highlight?QApplication::palette().color(QPalette::Highlight): QColor(235,235,235,235);
         pen.setWidth(1);
         painter->setPen(pen);
         QLinearGradient linearGrad(QPointF(0, 0), QPointF(0, 100));
         linearGrad.setColorAt(0, QColor(130,130,130,(0.7*tmpStep*tmpStep)));
         linearGrad.setColorAt(1, QColor(0,0,0,(0.75*tmpStep*tmpStep)));
         painter->setBrush(linearGrad);
         const QRect buttonRect = QRect(
            option.rect.x()+(option.rect.width()/m_lpButtons->size())*i+4,
            option.rect.y()+2,
            option.rect.width()/m_lpButtons->size() - 10,
            option.rect.height()-4);
         painter->drawRoundedRect(buttonRect, 10, 10);
         painter->setPen(highlight?QColor(Qt::white):m_Pen);

         if (it.value()->m_pImage) {
            painter->drawImage(QRect(
               buttonRect.x()+buttonRect.width()-(buttonRect.height()-10)-10/*padding*/,
               buttonRect.y()+5,
               (buttonRect.height()-10),
               (buttonRect.height()-10)),*it.value()->m_pImage);
         }

         QFont font = painter->font();
         font.setBold(true);
         painter->setFont(font);
         painter->drawText (buttonRect, Qt::AlignVCenter|Qt::AlignHCenter, QString(it.key()).remove('&') );
         painter->restore();
         i++;

         if (highlight) {
            QColor col = pen.color();
            for(int i=1;i<=4;i++) {
               painter->setBrush(Qt::NoBrush);
               pen.setWidth(i);
               col.setAlpha(205*(0.5/((float)i)));
               pen.setColor(col);
               painter->setPen(pen);
               painter->drawRoundedRect(buttonRect, 10, 10);
            }
         }
      }
   }
}//paintEvent

///Step by step animation
void DelegateDropOverlay::changeVisibility()
{
   foreach(const QModelIndex& idx, m_lActiveIndexes) {
      //There is a race condition when removing a conference participant
      if (idx.isValid() && !idx.model()->rowCount(idx)) {
         int step = idx.data(ContactModel::Role::DropState).toInt();
         //Remove items from the loop if there is no animation
         if (step >= 15 || step <= -15) {
            m_lActiveIndexes.removeAll(idx);
            if (step <= -15) //Hide the overlay
               ((QAbstractItemModel*)idx.model())->setData(idx,QVariant((int)0),ContactModel::Role::DropState);
         }
         else {
            //Update opacity
            step+=(step>0)?1:-1;
            ((QAbstractItemModel*)idx.model())->setData(idx,QVariant((int)step),ContactModel::Role::DropState);
         }
      }
   }
   //Stop loop if no animations are running
   if (!m_lActiveIndexes.size()) {
      m_pTimer->stop();
   }
}

///Prevent a race condition between the timer and model changes
void DelegateDropOverlay::slotLayoutChanged()
{
   m_lActiveIndexes.clear();
}
