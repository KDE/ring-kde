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
#include <QtGui/QPainter>
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
SvgTipLoader::SvgTipLoader(QTreeView* parent, const QString& path, const QString& text, int maxLine):QObject(parent),
m_OriginalPalette(parent->palette()),m_pParent(parent),m_BottomMargin(0),m_TopMargin(0),m_Tip(parent,path,text,maxLine),
m_pAnim(&m_Tip,this)
{
   ResizeEventFilter* filter = new ResizeEventFilter(this);
   parent->installEventFilter(filter);
   reload();

   connect(&m_pAnim,SIGNAL(animationStep(FrameDescription)),this,SLOT(animationStep(FrameDescription)));
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

   if (effectiveHeight >= m_pAnim.tipSize().height() && m_pCurrentTip) {
      p.setOpacity(0.1*m_CurrentFrame.opacity);
      p.drawImage(m_CurrentFrame.point.x(),m_CurrentFrame.point.y(),m_pAnim.currentImage());
   }

   QPalette p2 = m_pParent->viewport()->palette();
   p2.setBrush(QPalette::Base, QBrush(m_CurrentImage));
   m_pParent->viewport()->setPalette(p2);
   m_pParent->setPalette(p2);
}

/**
 * Take a long string and manually wrap it using a specific font. This is needed because SVG
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
      int mW = metric.width(word+' ');
      if (mW + total > width){
         result << tmp;
         tmp = QString();
         total = 0;
      }
      total += mW;
      tmp += word + ' ';
   }
   if (tmp.size()) {
      result << tmp;
   }

   return result;
}

///Set the top margin
void SvgTipLoader::setTopMargin(int margin)
{
   bool changed = !(m_TopMargin == margin);
   m_TopMargin = margin;
   if (changed) changeSize();
}

///Set the bottom margin
void SvgTipLoader::setBottomMargin(int margin)
{
   bool changed = !(m_BottomMargin == margin);
   m_BottomMargin = margin;
   if (changed) changeSize();
}

///Set the current tip, hide the previous one, if any
void SvgTipLoader::setCurrentTip(bool tip)
{
   m_pCurrentTip =  tip;
   if (tip) {
      m_pAnim.start();
   }
   else {
      m_CurrentFrame = {QPoint(0,0),QRect(0,0,0,0),0};
   }
   changeSize();
}

///Callback for new animation frame
void SvgTipLoader::animationStep(FrameDescription desc)
{
   m_CurrentFrame = desc;
   reload();
}

///Callback when size change
void SvgTipLoader::changeSize()
{
   int width(m_pParent->width()),height(m_pParent->height());
   int effectiveHeight = height-m_BottomMargin-m_TopMargin;
   qDebug() << height << (height-effectiveHeight) << effectiveHeight << (width-30);
   emit sizeChanged(QRect(15,m_TopMargin,width-30,effectiveHeight));
}