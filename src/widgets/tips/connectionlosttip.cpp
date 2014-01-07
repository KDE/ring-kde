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
#include "connectionlosttip.h"

//Qt
#include <QtSvg/QSvgRenderer>
#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>
#include <QtCore/QFile>
#include <QtCore/QTimer>

//KDE
#include <KDebug>
#include <KLocale>
#include <KStandardDirs>

///Constructor
ConnectionLostTip::ConnectionLostTip(QWidget* parent) : Tip(i18n("There was a network error, trying to reconnect in 60 seconds."),parent),m_pTimer(nullptr),m_Counter(0),
m_RenderCache(QSize(100,100),QImage::Format_ARGB32)
{
   setTimeOut(60);
   loadSvg(KStandardDirs::locate("data", "sflphone-client-kde/tips/reload.svg"));
   connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(startAnimation(bool)));
}

///Destructor
ConnectionLostTip::~ConnectionLostTip()
{
   delete m_pTimer;
}

///Return the decoration/image rect
QRect ConnectionLostTip::getDecorationRect()
{
   return QRect(0,0,100,100);
}

///The OCdecoration, a rotating arrow
void ConnectionLostTip::paintDecorations(QPainter& p, const QRect& textRect)
{
   Q_UNUSED(textRect);
   QSize size(100,100);
   QRect rect(m_CurrentRect.width()/2-50 ,m_Padding,size.width(),size.height());

   //Init the rederer, then render, then use the cache as long as possible
   if (!m_pR) {
      m_pR = new QSvgRenderer(m_OriginalFile);
      m_RenderCache.fill(0);
      QPainter p2;
      p2.begin(&m_RenderCache);
      p.setRenderHint(QPainter::Antialiasing, true);
      m_pR->render(&p2,QRect(0,0,size.width(),size.height()));
   }
   p.translate(QPoint(m_CurrentRect.width()/2,50));
   p.rotate((m_Counter*360)/256);
   p.drawImage(QRect(-50,-50,size.width(),size.height()),m_RenderCache);
} //paintDecorations

///Start tip animation
void ConnectionLostTip::startAnimation(bool visibility)
{
   if (!m_pTimer && visibility) {
      m_pTimer = new QTimer(this);
      connect(m_pTimer,SIGNAL(timeout()),this,SLOT(timeout()));
   }

   if (visibility)
      m_pTimer->start(1000/30);
   else if (m_pTimer) {
      m_pTimer->stop();
      m_Counter = 0;
   }
} //startAnimation

///Next frame
void ConnectionLostTip::timeout()
{
   m_Counter += 4;
   reload(m_CurrentRect,true);
}

///Return text allocated to this tip
QRect ConnectionLostTip::getTextRect(const QString& text)
{
   QRect r = Tip::getTextRect(text);
   r.setY(getDecorationRect().height()+m_Padding);
   r.setHeight(50);
   return r;
}
