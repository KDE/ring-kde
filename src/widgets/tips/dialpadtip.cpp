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
#include "dialpadtip.h"

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
DialPadTip::DialPadTip(QWidget* parent) : Tip(i18n("Use the dialpad below or start typing a number. Press enter or double click on the call to launch the call. Press escape or \"Hang Up\" to end the call."),parent)

{
   loadSvg(KStandardDirs::locate("data", "sflphone-client-kde/tips/keyboard.svg"));
}

///Destructor
DialPadTip::~DialPadTip()
{
}

QRect DialPadTip::getDecorationRect()
{
   return QRect(0,0,m_CurrentSize.width()-2*m_Padding,60);
}

void DialPadTip::paintDecorations(QPainter& p, const QRect& textRect)
{
   if (!m_pR)
      m_pR = new QSvgRenderer(m_OriginalFile);
   m_pR->render(&p,QRect(m_CurrentRect.width() - m_Padding - 50*2.59143327842 - 10 ,textRect.y()+textRect.height() + 10,50*2.59143327842,50));
}
