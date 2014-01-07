/****************************************************************************
 *   Copyright (C) 2012-2014 by Savoir-Faire Linux                          *
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
#include "tipmanager.h"

//Qt
#include <QtGui/QPainter>
#include <QtCore/QTimer>
#include <QtCore/QEvent>
#include <QtCore/QDebug>
#include <QtGui/QApplication>

//KDE
#include <KDebug>
#include <KStandardDirs>

bool TipManager::eventFilter(QObject *obj, QEvent *event)
{
   Q_UNUSED(obj);
   if (event->type() == QEvent::Resize) {
      changeSize();
   }
   return false;
}

///Constructor
TipManager::TipManager(QAbstractItemView* parent):QObject(parent),m_pParent(parent),
   m_BottomMargin(0),m_TopMargin(0),m_pAnim(this),m_pCurrentTip(nullptr)
{
   m_CurrentFrame = {QPoint(0,0),QRect(),0};
   parent->installEventFilter(this);
   parent->setProperty("tipManager",QVariant::fromValue(qobject_cast<TipManager*>(this)));
   changeSize();
   reload();

   connect(&m_pAnim,SIGNAL(animationStep(FrameDescription)),this,SLOT(animationStep(FrameDescription)));
   connect(&m_pAnim,SIGNAL(transitionStarted(QAbstractAnimation::Direction,QAbstractAnimation::State)),this,
         SLOT(slotTransitionStarted(QAbstractAnimation::Direction,QAbstractAnimation::State)));
}

TipManager::~TipManager()
{
}

///Get the current image
QImage TipManager::getImage()
{
   return m_CurrentImage;
}

///Reload the SVG with new size
void TipManager::reload()
{
   int width(m_pParent->width()),height(m_pParent->height());
   int effectiveHeight = height-m_BottomMargin-m_TopMargin;
   m_CurrentImage = QImage(QSize(width,height),QImage::Format_RGB888);
   m_CurrentImage.fill( QApplication::palette().base().color() );
   QPainter p(&m_CurrentImage);

   if (effectiveHeight >= m_pAnim.tipSize().height() /*&& m_pCurrentTip*/) {
      p.setOpacity(0.1*m_CurrentFrame.opacity);
      p.drawImage(m_CurrentFrame.point.x(),m_CurrentFrame.point.y(),m_pAnim.currentImage());
   }

   QPalette p2 = m_pParent->viewport()->palette();
   p2.setBrush(QPalette::Base, QBrush(m_CurrentImage));
   m_pParent->viewport()->setPalette(p2);
   m_pParent->setPalette(p2);
}

///Set the top margin
void TipManager::setTopMargin(int margin)
{
   const bool changed = !(m_TopMargin == margin);
   m_TopMargin = margin;
   if (changed) {
      changeSize(true);
      reload();
   }
}

///Set the bottom margin
void TipManager::setBottomMargin(int margin)
{
   bool changed = !(m_BottomMargin == margin);
   m_BottomMargin = margin;
   if (changed) {
      changeSize(false);
   }
   reload();
}

///Set the current tip, hide the previous one, if any
void TipManager::setCurrentTip(Tip* tip)
{
   m_pCurrentTip = tip;
   m_pAnim.setTip(m_pCurrentTip);
   emit currentTipChanged(tip);
   m_pAnim.start(true);
   changeSize(true);
}

///Callback for new animation frame
void TipManager::animationStep(FrameDescription desc)
{
   m_CurrentFrame = desc;
   reload();
}

///Callback when size change
void TipManager::changeSize(bool ignoreAnim)
{
   if (m_pCurrentTip) {
      int width(m_pParent->width()),height(m_pParent->height());
      int effectiveHeight = height-m_BottomMargin-m_TopMargin;
      emit sizeChanged(QRect(15,m_TopMargin,width-30,effectiveHeight),ignoreAnim);
   }
}

bool TipManager::hideCurrentTip(bool skipAnimation)
{
   if (skipAnimation) {
      m_pCurrentTip = nullptr;
      m_pAnim.setTip(nullptr);
      emit currentTipChanged(nullptr);
      emit transitionStarted(QAbstractAnimation::Backward,QAbstractAnimation::Stopped);
   }
   else if (m_pCurrentTip) {
      m_pAnim.setTip(m_pCurrentTip);
      changeSize(false);
      m_pAnim.start(false);
      return false;
   }
   return true;
}

void TipManager::slotTransitionStarted(QAbstractAnimation::Direction direction, QAbstractAnimation::State state)
{
   emit transitionStarted(direction,state);
}
