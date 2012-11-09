/****************************************************************************
 *   Copyright (C) 2012 by Savoir-Faire Linux                               *
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
#include "riggingtip.h"

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
RiggingTip::RiggingTip(QWidget* parent) : Tip(QString(),parent),m_pTimer(nullptr),m_Counter(0)

{
   setHasBackground(false);
   setHasText(false);
   m_Padding = 0;
   loadSvg(KStandardDirs::locate("data", "sflphone-client-kde/tips/rigging.svg"));
   connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(startAnimation(bool)));
}

///Destructor
RiggingTip::~RiggingTip()
{
   if (m_pTimer) delete m_pTimer;
}

QRect RiggingTip::getDecorationRect()
{
   return QRect(0,0,100*1.13549618321,100);
}

void RiggingTip::paintDecorations(QPainter& p, const QRect& textRect)
{
   Q_UNUSED(textRect);
   if (!m_pR)
      m_pR = new QSvgRenderer(m_OriginalFile);
   m_pR->render(&p,QRect(0 ,0,100*1.13549618321 - m_Counter,100));
}

void RiggingTip::startAnimation(bool visibility)
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
}

void RiggingTip::timeout()
{
   m_Counter += 8;
   reload(m_CurrentRect,true);
}