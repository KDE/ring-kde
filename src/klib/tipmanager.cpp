/****************************************************************************
 *   Copyright (C) 2012-2013 by Savoir-Faire Linux                          *
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

//KDE
#include <KDebug>
#include <KStandardDirs>

bool ResizeEventFilter::eventFilter(QObject *obj, QEvent *event)
{
   Q_UNUSED(obj);
   if (event->type() == QEvent::Resize) {
      m_pLoader->changeSize();
   }
   return false;
}

///Constructor
TipManager::TipManager(QTreeView* parent):QObject(parent),
m_OriginalPalette(parent->palette()),m_pParent(parent),m_BottomMargin(0),m_TopMargin(0),
m_pAnim(this),m_pCurrentTip(nullptr),m_pTimer(new QTimer())
{
   ResizeEventFilter* filter = new ResizeEventFilter(this);
   parent->installEventFilter(filter);
   reload();
   
   connect(m_pTimer,SIGNAL(timeout()),this,SLOT(timeout()));

   connect(&m_pAnim,SIGNAL(animationStep(FrameDescription)),this,SLOT(animationStep(FrameDescription)));
   connect(&m_pAnim,SIGNAL(animationEnded()),this,SLOT(animationEnded()));
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
   m_CurrentImage.fill( m_OriginalPalette.base().color().rgb() );
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
   bool changed = !(m_TopMargin == margin);
   m_TopMargin = margin;
   if (changed) changeSize(true);
}

///Set the bottom margin
void TipManager::setBottomMargin(int margin)
{
   bool changed = !(m_BottomMargin == margin);
   m_BottomMargin = margin;
   if (changed) changeSize(true);
}

///Set the current tip, hide the previous one, if any
void TipManager::setCurrentTip(Tip* tip)
{
   if (tip == m_pCurrentTip) return;

   //Set current tip
   if (!m_pCurrentTip && tip) {
      setCurrentTip_private(tip);
   }
   else if (m_pCurrentTip) {
      m_pAnim.start(false);
      changeSize(true);
      m_lTipQueue << tip;
   }
   else {
      setCurrentTip_private(tip);
   }
}

void TipManager::timeout()
{
   Tip* toHide = m_lHidingTipQueue.takeFirst();
   if (toHide && toHide == m_pCurrentTip) {
      m_lTipQueue.removeAll(toHide);
      setCurrentTip(nullptr);
   }
   if (m_lTipQueue.size()) {
      setCurrentTip(m_lTipQueue.takeFirst());
   }
   else
      setCurrentTip(nullptr);
}

void TipManager::setCurrentTip_private(Tip* tip)
{
   m_pAnim.setTip(tip);
   m_pCurrentTip =  (m_lTipQueue.size()&&(m_pCurrentTip&&!m_pCurrentTip->isVisible()))?m_lTipQueue.takeFirst():tip;
   emit currentTipChanged(tip);
   m_pAnim.start(m_pCurrentTip != nullptr);

   //Start timer if necessary
   if (m_pCurrentTip && m_pCurrentTip->timeOut()) {
      //QTimer::singleShot(tip->timeOut(), this, SLOT(timeout()));
      m_pTimer->setSingleShot(true);
      m_pTimer->setInterval(m_pCurrentTip->timeOut());
      m_lHidingTipQueue << m_pCurrentTip;
      m_pTimer->start();
   }
   changeSize(true);
}

void TipManager::animationEnded()
{
   //TODO stop timer
   if (m_pCurrentTip && !m_pCurrentTip->isVisible()) {
      m_lTipQueue.removeAll(m_pCurrentTip);
      m_pCurrentTip = nullptr;
      emit currentTipChanged(nullptr);
   }

   if (!m_pCurrentTip && m_lTipQueue.size())
      setCurrentTip_private(m_lTipQueue.takeFirst());
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

///Hide a tip, check if it is the current one or in the queue
void TipManager::hideTip(Tip* tip)
{
   if (m_pCurrentTip == tip)
      setCurrentTip(nullptr);
   m_lTipQueue.removeAll(tip);
}
