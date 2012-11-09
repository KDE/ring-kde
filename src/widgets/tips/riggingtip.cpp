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

//KDE
#include <KDebug>
#include <KLocale>
#include <KStandardDirs>

///Constructor
RiggingTip::RiggingTip(QWidget* parent) : Tip(QString(),parent)

{
   setHasBackground(false);
   setHasText(false);
   m_Padding = 0;
   loadSvg(KStandardDirs::locate("data", "sflphone-client-kde/tips/rigging.svg"));
}

///Destructor
RiggingTip::~RiggingTip()
{
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
   m_pR->render(&p,QRect(0 ,0,100*1.13549618321,100));
}