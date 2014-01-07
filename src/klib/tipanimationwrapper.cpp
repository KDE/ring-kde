/* Copyright 2012-2014 by Savoir-Faire Linux                                    *
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
#include "tipmanager.h"

TipAnimationWrapper::TipAnimationWrapper(QObject* parent) : QObject(parent),m_MaxStep(15),m_Step(0),m_pTimer(nullptr),m_TipSize(QSize(0,0)),
m_pTip(nullptr),m_pCurrentTip(nullptr),m_CurrentAnimation(Tip::TipAnimation::None),m_FadeDirection(true)
{
   connect(parent, SIGNAL(sizeChanged(QRect,bool)) , this , SLOT(sizeChanged(QRect,bool)) );
   connect(parent, SIGNAL(currentTipChanged(Tip*)) , this , SLOT(currentChanged(Tip*))    );
}

TipAnimationWrapper::~TipAnimationWrapper()
{
   
}

void TipAnimationWrapper::sizeChanged(const QRect &rect, bool ignoreAnim)
{
   if (m_pTip) {
      m_TipSize = m_pTip->reload(QRect(0,0,rect.width(),rect.height()));
      m_CurrentImage = m_pTip->m_CurrentImage;
   }
   m_ParentRect = rect;
   if (!ignoreAnim) {
      Tip::TipAnimation anim = m_CurrentAnimation;
      m_CurrentAnimation = Tip::TipAnimation::None;
      m_Step = 0;
      step();
      m_CurrentAnimation = anim;
   }
}

const QImage& TipAnimationWrapper::currentImage()
{
   return (m_pTip)?m_pTip->m_CurrentImage:m_CurrentImage;
}

QSize TipAnimationWrapper::tipSize()
{
   return m_TipSize;
}

/**
 * Start the animation (if any)
 * @param show start the 'in' or 'out' animation
 */
void TipAnimationWrapper::start(bool show)
{
   if (m_pTip) {
      if (!m_pTimer) {
         m_pTimer = new QTimer(this);
         connect(m_pTimer,SIGNAL(timeout()),this,SLOT(step()));
      }

      m_Step = 0;
      m_CurrentAnimation = show?m_pTip->m_AnimationIn:m_pTip->m_AnimationOut;
      m_FadeDirection    = show;
      if (m_CurrentAnimation != Tip::TipAnimation::None) {
         emit transitionStarted(show?QAbstractAnimation::Forward:QAbstractAnimation::Backward,QAbstractAnimation::Running);
         m_pTimer->start(33);
      }
      else {
         step();
         emit transitionStarted(show?QAbstractAnimation::Forward:QAbstractAnimation::Backward,QAbstractAnimation::Stopped);
         emit animationEnded();
      }
      m_pTip->setVisible(show);
   }
}

void TipAnimationWrapper::step()
{
   if (!m_pTip) return;
   m_Step++;
   if (m_Step > m_MaxStep) {
      m_Step = 0;
      if (m_pTimer)
         m_pTimer->stop();
      emit transitionStarted(m_FadeDirection?QAbstractAnimation::Forward:QAbstractAnimation::Backward,QAbstractAnimation::Stopped);
      emit animationEnded();
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

      //Set opacity
      float opacity = ((float)((m_FadeDirection)?0.0f:1.0f)-((float)m_Step/m_MaxStep))*m_pTip->opacity();
      opacity = (opacity<0)?-opacity:opacity;

      //In animations
      if (m_FadeDirection) {
         switch (m_CurrentAnimation) {
            case Tip::TipAnimation::Fade:
               break;
            case Tip::TipAnimation::TranslationTop:
               wy += -m_MaxStep+m_Step;
               break;
            case Tip::TipAnimation::TranslationBottom:
               wy += m_MaxStep-m_Step;
               break;
            case Tip::TipAnimation::TranslationLeft:
               wx += -m_MaxStep+m_Step;
               break;
            case Tip::TipAnimation::TranslationRight:
               wx += m_MaxStep-m_Step;
               break;
            case Tip::TipAnimation::None:
               opacity = 1;
               m_Step  = 0;
               break;
         }
      }
      //Out animations
      else {switch (m_CurrentAnimation) {
            case Tip::TipAnimation::Fade:
               break;
            case Tip::TipAnimation::TranslationTop:
               wy += m_Step;
               break;
            case Tip::TipAnimation::TranslationBottom:
               wy += -m_Step;
               break;
            case Tip::TipAnimation::TranslationLeft:
               wx += m_Step;
               break;
            case Tip::TipAnimation::TranslationRight:
               wx += -m_Step;
               break;
            case Tip::TipAnimation::None:
               opacity = 0;
               m_Step  = 0;
               break;
         }
      }

      m_pCurrentDesc = {QPoint(wx,wy),QRect(0,0,0,0),opacity};
      emit animationStep(m_pCurrentDesc);
   }
}

bool TipAnimationWrapper::isRunning()
{
   return m_Step!=0;
}

void TipAnimationWrapper::currentChanged(Tip* newCurrent)
{
   if (m_pCurrentTip)
      disconnect(m_pCurrentTip,SIGNAL(changed()),this,SLOT(tipChanged()));
   m_pCurrentTip = newCurrent;
   if (newCurrent)
      connect(m_pCurrentTip,SIGNAL(changed()),this,SLOT(tipChanged()));
}

void TipAnimationWrapper::tipChanged()
{
   //Only request repaint now if none is scheduled for the next frame
   if (m_pTimer && !m_pTimer->isActive())
      emit animationStep(m_pCurrentDesc);
}

void TipAnimationWrapper::setTip(Tip* tip)
{
   m_CurrentImage= QImage();
   m_pTip = tip;
   emit tipChanged();
}
