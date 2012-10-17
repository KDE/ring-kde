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
#include "tip.h"

//Qt
#include <QtSvg/QSvgRenderer>
#include <QtGui/QPainter>
#include <QtCore/QFile>

//KDE
#include <KDebug>

//SFLPhone
#include "svgtiploader.h"

///Constructor
Tip::Tip(QWidget* parent,const QString& path, const QString& text, int maxLine) : QObject(parent),m_OriginalText(text),m_MaxLine(maxLine),m_Position(TipPosition::Middle),m_IsMaxSize(false),m_pR(nullptr),
m_OriginalPalette(parent->palette())
{
   QFile file(path);
   if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      kDebug() << "The tip" << path << "failed to load: No such file";
   }
   else {
      m_OriginalFile = file.readAll();
      m_OriginalFile.replace("BACKGROUD_COLOR_ROLE",brightOrDarkBase()?"#000000":"#ffffff");
      int lastIndexOf = -1;
      QStringList lines = SvgTipLoader::stringToLineArray(parent->font(),m_OriginalText,250);
      for (int i=(maxLine < lines.size())?maxLine:lines.size();i;i--) {
         m_OriginalFile.replace("BASE_ROLE_COLOR",m_OriginalPalette.base().color().name().toAscii());
         int idx = m_OriginalFile.lastIndexOf(QString("TLline" + QString::number(i)).toAscii(), lastIndexOf);
         if (idx > 0) {
            lastIndexOf = idx;
            if (i==maxLine && maxLine < lines.size())
               lines[i-1] += "...";
            m_OriginalFile.insert(idx+9,lines[i-1].toAscii());
         }
      }
   }
}

///Destructor
Tip::~Tip()
{
   
}

/**
 * Reload the tip for new dimensions
 * @return The size required for the tip
 */
QSize Tip::reload(const QRect& availableSize)
{
   if (m_CurrentRect != availableSize && !(m_IsMaxSize && m_CurrentSize.width()*1.25 < availableSize.width())) {
      m_CurrentRect = availableSize;
      int wwidth(availableSize.width()),wheight((availableSize.width())*0.539723102);

      if (wheight > 170) {
         wheight = 170;
         wwidth  = wheight*1.85280192;
         m_IsMaxSize = true;
      }
      else {
         m_IsMaxSize = false;
      }
      m_CurrentImage = QImage(QSize(wwidth,wheight),QImage::Format_RGB888);
      m_CurrentImage.fill(m_OriginalPalette.base().color() );
      QPainter p(&m_CurrentImage);

      if (!m_pR)
         m_pR = new QSvgRenderer(m_OriginalFile);

      m_CurrentSize = QSize(wwidth,wheight);

      if (availableSize.height() >= wheight)
         m_pR->render(&p,QRect(0,0,wwidth,wheight));
   }
   return m_CurrentSize;
}

///Check if the thene color scheme is darker than #888888
///@return true = bright, false = dark
bool Tip::brightOrDarkBase()
{
   QColor color = m_OriginalPalette.base().color();
   return (color.red() > 128 && color.green() > 128 && color.blue() > 128);
}