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

#ifndef TIP_ANIMATION_WRAPPER_H
#define TIP_ANIMATION_WRAPPER_H
//Base
#include <QtCore/QObject>

//Qt
#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtGui/QImage>
class QTimer;

//SFLPhone
#include "tip.h"
class TipManager;

//Structs
struct FrameDescription {
   QPoint point;
   QRect  rect;
   float  opacity;
};

class TipAnimationWrapper : public QObject
{
   Q_OBJECT
public:
   TipAnimationWrapper(TipManager* parent);
   virtual ~TipAnimationWrapper();

   //Mutator
   void start(bool show = true);

   //Setter
   void setTip(Tip* tip) {m_pTip = tip;}

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

protected:
   int     m_MaxStep;

private slots:
   void step();
   void sizeChanged(QRect rect,bool ignoreAnim);

signals:
   void animationStep(FrameDescription desc);
   void animationEnded();
};
#endif