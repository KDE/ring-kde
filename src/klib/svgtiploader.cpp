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
#include "svgtiploader.h"

//Qt
#include <QtCore/QFile>
#include <QtGui/QPainter>
#include <QtSvg/QSvgRenderer>
#include <QtCore/QEvent>

//KDE
#include <KDebug>
#include <KStandardDirs>

bool ResizeEventFilter::eventFilter(QObject *obj, QEvent *event)
{
   Q_UNUSED(obj);
   if (event->type() == QEvent::Resize) {
      m_pLoader->reload();
   }
   return false;
}

QSvgRenderer* r;
///Constructor
SvgTipLoader::SvgTipLoader(QTreeView* parent, QString path, QString text, int maxLine):QObject(parent),m_OriginalText(text)
   ,m_MaxLine(maxLine),m_OriginalPalette(parent->palette()),m_pParent(parent),m_BottomMargin(0),m_TopMargin(0)
{
   QFile file(path);
   if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      kDebug() << "The tip" << path << "failed to load: No such file";
   }
   else {
      ResizeEventFilter* filter = new ResizeEventFilter(this);
      parent->installEventFilter(filter);
      m_OriginalFile = file.readAll();
      m_OriginalFile.replace("BACKGROUD_COLOR_ROLE",brightOrDarkBase()?"#000000":"#ffffff");
      int lastIndexOf = -1;
      QStringList lines = stringToLineArray(m_pParent->font(),text,250);
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
      r = new QSvgRenderer(m_OriginalFile);
      reload();
   }
}

///Get the current image
QImage SvgTipLoader::getImage()
{
   return m_CurrentImage;
}

///Reload the SVG with new size
void SvgTipLoader::reload()
{
   int width(m_pParent->width()),height(m_pParent->height());
   int effectiveHeight = height-m_BottomMargin-m_TopMargin;
   m_CurrentImage = QImage(QSize(width,height),QImage::Format_RGB888);
   m_CurrentImage.fill( m_OriginalPalette.base().color() );
   QPainter p(&m_CurrentImage);
   int wwidth(width-30),wheight((width-30)*0.539723102);
   int wx(10),wy((effectiveHeight-wheight)/2 + m_TopMargin);


   //Prevent supersize tips
   if (wheight > 170) {
      wheight = 170;
      wwidth = wheight*1.85280192;
      wx = (width - wwidth) /2;
      wy = (effectiveHeight - wheight) /2 + m_TopMargin;
   }

   if (effectiveHeight >= wheight)
      r->render(&p,QRect(wx,wy,wwidth,wheight));

   QPalette p2 = m_pParent->viewport()->palette();
   p2.setBrush(QPalette::Base, QBrush(m_CurrentImage));
   m_pParent->viewport()->setPalette(p2);
   m_pParent->setPalette(p2);
}



/**Take a long string and manually wrap it using a specific font. This is needed because SVG
 * does not natively support wrapping and Qt does not implement the few hacks around this
 * so it is better to create a <text> field for each line
 */
QStringList SvgTipLoader::stringToLineArray(const QFont& font, QString text, int width )
{
   QFontMetrics metric(font);
   int total = 0;
   QStringList result;
   QStringList words = text.split(' ');

   QString tmp;
   foreach(QString word, words) {
      int mW = metric.width(word+" ");
      if (mW + total > width){
         result << tmp;
         tmp = "";
         total = 0;
      }
      total += mW;
      tmp += word + " ";
   }
   if (tmp.size()) {
      result << tmp;
   }

   return result;
}

///Check if the thene color scheme is darker than #888888
///@return true = bright, false = dark
bool SvgTipLoader::brightOrDarkBase()
{
   QColor color = m_OriginalPalette.base().color();
   return (color.red() > 128 && color.green() > 128 && color.blue() > 128);
}

///Set the top margin
void SvgTipLoader::setTopMargin(int margin)
{
   bool changed = !(m_TopMargin == margin);
   m_TopMargin = margin;
   if (changed) reload();
}

///Set the bottom margin
void SvgTipLoader::setBottomMargin(int margin)
{
   bool changed = !(m_BottomMargin == margin);
   m_BottomMargin = margin;
   if (changed) reload();
}