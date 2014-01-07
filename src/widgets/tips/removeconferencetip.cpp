/****************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                          *
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
#include "removeconferencetip.h"

//Qt
#include <QtSvg/QSvgRenderer>
#include <QtGui/QPainter>

//KDE
#include <KLocale>
#include <KStandardDirs>

///Constructor
RemoveConferenceTip::RemoveConferenceTip(QWidget* parent) : Tip(i18n("Remove the call from the conference, the call will be put on hold"),parent)

{
   m_Position = TipPosition::Middle;
   setHasBackground(false);
   setHasText(false);
   m_Padding = 0;
   loadSvg(KStandardDirs::locate("data", "sflphone-client-kde/tips/removeconference.svg"));
}

///Destructor
RemoveConferenceTip::~RemoveConferenceTip()
{
}

QRect RemoveConferenceTip::getDecorationRect()
{
   return m_DecoRect;
}

///Paint the image/decoration
void RemoveConferenceTip::paintDecorations(QPainter& p, const QRect& textRect)
{
   Q_UNUSED(textRect);
   if (!m_pR) {
      m_pR = new QSvgRenderer(m_OriginalFile); //TODO delete
   }
   QPen pen = p.pen();
   pen.setCapStyle(Qt::RoundCap);
   pen.setJoinStyle(Qt::RoundJoin);
   pen.setStyle(Qt::DotLine);
   pen.setColor(QColor(Qt::black));
   pen.setWidth(7);
   p.setPen(pen);
   p.setBrush(Qt::transparent);
   p.setOpacity(1);
   int margin(30),width(m_DecoRect.width()-60);
   if ((m_DecoRect.width()-60)*.873 > m_DecoRect.height()-100) {
      width  = (m_DecoRect.height()-100)*1.1453362256;
      margin = (m_DecoRect.width()-width)/2;
   }
   p.drawRoundedRect(QRect(4,4,m_DecoRect.width()-8,m_DecoRect.height()-8),10,10);
   m_pR->render(&p,QRect(margin ,30,width,width*.873));
   p.drawText(QRect(30,m_DecoRect.height()-60,m_DecoRect.width()-60,60),Qt::AlignHCenter|Qt::TextWordWrap,m_OriginalText);
} //paintDecorations

///Necessary to get the maximum size allowed
QSize RemoveConferenceTip::reload(const QRect& availableSize,bool force)
{
   m_DecoRect = QRect(0,0,availableSize.width()-2*m_Padding,availableSize.height()-40);
   return Tip::reload(availableSize,force);
}
