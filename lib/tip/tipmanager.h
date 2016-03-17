/****************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
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
#ifndef TIPMANAGER_H
#define TIPMANAGER_H

//Qt
#include <QtCore/QObject>
#include <QtWidgets/QAbstractItemView>
#include <QtGui/QImage>
#include <QAbstractAnimation>

//Ring
#include "tip.h"
#include "tipanimationwrapper.h"
class TipManager;

///This class create a background brush for a QWidget with a tip window
class Q_DECL_EXPORT TipManager : public QObject
{
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
   friend class ResizeEventFilter;
public:
   //Constructor
   explicit TipManager(QAbstractItemView* parent);
   virtual ~TipManager();

   //Mutator
   bool hideCurrentTip(bool skipAnimation = false);

   //Getter
   QImage getImage();
   Tip* currentTip() {return m_pCurrentTip;}

   //Setters
   void setTopMargin(int margin);
   void setBottomMargin(int margin);
   void setCurrentTip(Tip* tip);

protected:
   virtual bool eventFilter(QObject *obj, QEvent *event) override;

private:
   //Methods
   void reload();

   //Attributes
   QAbstractItemView*      m_pParent         ;
   int                     m_TopMargin      {0};
   int                     m_BottomMargin   {0};
   QImage                  m_CurrentImage    ;
   Tip*                    m_pCurrentTip    {nullptr};
   TipAnimationWrapper     m_pAnim           ;
   FrameDescription        m_CurrentFrame    ;

private Q_SLOTS:
   void animationStep(FrameDescription desc);
   void slotTransitionStarted(QAbstractAnimation::Direction direction, QAbstractAnimation::State state);

public Q_SLOTS:
   void changeSize(bool ignoreAnim = false);

Q_SIGNALS:
   void sizeChanged(const QRect &newRect,bool ignoreAnim);
   void currentTipChanged(Tip*);
   void transitionStarted(QAbstractAnimation::Direction direction, QAbstractAnimation::State state);

};
Q_DECLARE_METATYPE(TipManager*)

#endif
