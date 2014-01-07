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
#include "conftip.h"

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
ConfTip::ConfTip(QWidget* parent) : Tip(i18n("Do you know you can create conferences or transfer the call by drag and dropping one call on an other one?"),parent),
m_pDnDR(nullptr),m_pDnDPix(nullptr)
{
   m_Position = TipPosition::Top;
   setAnimationIn(Tip::TipAnimation::TranslationBottom);
   setAnimationOut(Tip::TipAnimation::TranslationBottom);
   m_DragAndDrop = loadSvg(KStandardDirs::locate("data", "sflphone-client-kde/tips/dragAndDrop.svg"));
   loadSvg(KStandardDirs::locate("data", "sflphone-client-kde/tips/upArrow.svg"));
}

///Destructor
ConfTip::~ConfTip()
{
}

QRect ConfTip::getDecorationRect()
{
   return QRect(0,0,m_CurrentSize.width(),80);
}

void ConfTip::paintDecorations(QPainter& p, const QRect& textRect)
{
   if (!m_pR) {
      m_pR = new QSvgRenderer(m_OriginalFile);
      m_pDnDR = new QSvgRenderer(m_DragAndDrop);
   }
   m_pR->render(&p,QRect(m_Padding, m_Padding ,75*0.694923911334,75));
   m_pDnDR->render(&p,QRect(m_Padding, textRect.height()+textRect.y()+m_Padding ,m_CurrentRect.width()-2*m_Padding,75));
}

//Show the text on the right side of the screen
QRect ConfTip::getTextRect(const QString& text)
{
   QFontMetrics metric(font());
   QRect rect = metric.boundingRect(QRect(m_Padding+75,m_Padding,m_CurrentRect.width()-2*m_Padding-75,999999),Qt::AlignJustify|Qt::TextWordWrap,text);
   return rect;
}
