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

#ifndef TIP_ANIMATION_WRAPPER_H
#define TIP_ANIMATION_WRAPPER_H
//Base
#include <QtCore/QObject>

//Qt
#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtCore/QAbstractAnimation>
#include <QtGui/QImage>
class QTimer;

//SFLPhone
#include "tip.h"

//Structs
struct FrameDescription {
   QPoint point;
   QRect  rect;
   float  opacity;
};

class LIB_EXPORT TipAnimationWrapper : public QObject
{
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:
   explicit TipAnimationWrapper(QObject* parent = nullptr);
   virtual ~TipAnimationWrapper();

   //Mutator
   void start(bool show = true);

   //Setter
   void setTip(Tip* tip);

   //Getter
   const QImage& currentImage();
   QSize tipSize();
   bool  isRunning();
   Tip*  tip() {return m_pTip;}

private:
   //Attributes
   QTimer*           m_pTimer;
   Tip*              m_pTip;
   int               m_Step;
   Tip::TipAnimation m_CurrentAnimation;
   bool              m_FadeDirection;

   QRect   m_ParentRect;
   QSize   m_TipSize;
   QImage  m_CurrentImage;
   Tip*    m_pCurrentTip;
   FrameDescription m_pCurrentDesc;

protected:
   int     m_MaxStep;

private Q_SLOTS:
   void step();
   void sizeChanged(const QRect &rect,bool ignoreAnim);
   void currentChanged(Tip* newCurrent);
   void tipChanged();

Q_SIGNALS:
   void animationStep(FrameDescription desc);
   void animationEnded();
   void transitionStarted(QAbstractAnimation::Direction direction, QAbstractAnimation::State state);
};
#endif
