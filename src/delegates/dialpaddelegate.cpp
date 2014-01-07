/****************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#include "dialpaddelegate.h"
#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include "lib/call.h"
#include "lib/callmodel.h"

QTimer* DialpadDelegate::m_spTimer             = nullptr;
DialpadDelegate* DialpadDelegate::m_spInstance = nullptr;
int DialpadDelegate::m_sStep                   = 0;
QSet<QModelIndex> DialpadDelegate::m_slIndexes = QSet<QModelIndex>();

void DialpadDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index, uint rightPadding)
{
   int animStep(index.data(Call::Role::DTMFAnimState).toInt()),dialIdx(index.data(Call::Role::LastDTMFidx).toInt());
   if (animStep <= 0)
      return;
   painter->save();
   painter->setRenderHint(QPainter::Antialiasing, true );

   const QRect r(option.rect.x()+option.rect.width()-option.rect.height()-8-rightPadding,option.rect.y()+2,option.rect.height()-4,option.rect.height()-4);
   static QColor c1(Qt::black);
   c1.setAlpha(animStep*1.5);
   painter->setBrush(c1);
   painter->setPen(Qt::NoPen);
   int h = r.height();

   for (int i=0; i < 12;i++) {
      if (i == dialIdx) {
         painter->save();
         static QColor c("#dd0000");
         c.setAlpha(animStep*2);
         painter->setBrush(QBrush(c));
      }
      painter->drawRoundedRect(r.x()+(h/3)*(i%3),r.y()+(h/4)*(i/3)+2,h/3-2,h/4-2,2,2);
      if (i == dialIdx) {
         painter->restore();
      }
   }
   painter->restore();

   if (animStep == 50)
      m_sStep = 50;

   if (!m_spTimer) {
      m_spTimer = new QTimer();
      m_spTimer->setInterval(1000/60);
      m_spInstance = new DialpadDelegate();
      connect(m_spTimer,SIGNAL(timeout()),m_spInstance,SLOT(slotFade()));
   }
   if (!m_spTimer->isActive() && animStep > 0)
      m_spTimer->start();
   m_slIndexes << index;
}

void DialpadDelegate::slotFade()
{
   m_sStep--;

   foreach(const QModelIndex& idx,m_slIndexes) {
      if (!idx.isValid() || idx.data(Call::Role::DTMFAnimState).toInt() == 0) {
         m_slIndexes.remove(idx);
      }
      else {
         CallModel::instance()->setData(idx,idx.data(Call::Role::DTMFAnimState).toInt() -1,Call::Role::DTMFAnimState);
      }
   }
   if ((m_sStep<=0) ||!m_slIndexes.size()) {
      m_spTimer->stop();
   }
}
