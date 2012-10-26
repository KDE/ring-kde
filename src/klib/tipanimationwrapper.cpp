/* Copyright 2012 by Savoir-Faire Linux                                         *
 * Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>       *
 *                                                                              *
 * This library is free software; you can redistribute it and/or                *
 * modify it under the terms of the GNU Lesser General Public                   *
 * License as published by the Free Software Foundation; either                 *
 * version 2.1 of the License, or (at your option) any later version.           *
 *                                                                              *
 * This library is distributed in the hope that it will be useful,              *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU            *
 * Lesser General Public License for more details.                              *
 *                                                                              *
 * You should have received a copy of the GNU Lesser General Public             *
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.*/
#include "tipanimationwrapper.h"

//Qt
#include <QtCore/QTimer>

//KDE
#include <KDebug>

//SFLPhone
#include "tip.h"
#include "svgtiploader.h"

TipAnimationWrapper::TipAnimationWrapper(Tip* aTip, SvgTipLoader* parent) : QObject(parent),m_MaxStep(15),m_Step(0),m_pTimer(nullptr),m_pTip(aTip),m_TipSize(QSize(0,0))
{
   connect(parent,SIGNAL(sizeChanged(QRect)),this,SLOT(sizeChanged(QRect)));
}

TipAnimationWrapper::~TipAnimationWrapper()
{
   
}

void TipAnimationWrapper::sizeChanged(QRect rect)
{
   if (m_pTip) {
      m_TipSize = m_pTip->reload(QRect(0,0,rect.width(),rect.height()));
      m_CurrentImage = m_pTip->m_CurrentImage;
   }
   m_ParentRect = rect;
}

const QImage& TipAnimationWrapper::currentImage()
{
   return m_CurrentImage;
}

QSize TipAnimationWrapper::tipSize()
{
   return m_TipSize;
}

void TipAnimationWrapper::start()
{
   if (!m_pTimer) {
      m_pTimer = new QTimer(this);
      connect(m_pTimer,SIGNAL(timeout()),this,SLOT(step()));
   }
   m_pTimer->start(33);
}

void TipAnimationWrapper::step()
{
   m_Step++;
   if (m_Step > m_MaxStep && m_pTimer) {
      m_Step = 0;
      m_pTimer->stop();
   }
   else {
      int wy = 0;

      int wx(10+((m_ParentRect.width())-tipSize().width())/2);

      switch (m_pTip->m_Position) {
         case Tip::TipPosition::Middle:
            wy = ((m_ParentRect.height()-tipSize().height())/2) + m_ParentRect.y();
            break;
         case Tip::TipPosition::Top:
            wy = (5 + m_ParentRect.y());
            break;
         case Tip::TipPosition::Bottom:
            wy = m_ParentRect.height() - tipSize().height() - 40 + m_ParentRect.y();
            break;
         default:
            wy = ((m_ParentRect.height()-tipSize().height())/2 + m_ParentRect.y());
            break;
      }

      emit animationStep({QPoint(wx,wy),QRect(0,0,0,0),(float)m_Step/m_MaxStep});
   }
}