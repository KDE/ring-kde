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
#include "tip.h"

//Qt
#include <QtSvg/QSvgRenderer>
#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>
#include <QtGui/QApplication>
#include <QtCore/QFile>

//KDE
#include <KDebug>

//SFLPhone
#include "tipmanager.h"

///Constructor
Tip::Tip(const QString& text, QWidget* parent) : QObject(parent),m_OriginalText(text),m_Position(TipPosition::Bottom),m_IsMaxSize(false),m_pR(nullptr),
m_OriginalPalette(QApplication::palette()),m_AnimationIn(TipAnimation::TranslationTop),m_AnimationOut(TipAnimation::TranslationTop),m_pFont(nullptr),
m_IsVisible(false),m_TimeOut(0),m_HasBg(true),m_HasText(true),m_Padding(15),m_MaxWidth(350)
{
}

///Destructor
Tip::~Tip()
{
   if (m_pFont) delete m_pFont;
}

/**
 * Reload the tip for new dimensions
 * @return The size required for the tip
 */
QSize Tip::reload(const QRect& availableSize,bool force)
{
   if ((m_CurrentRect != availableSize && !(m_IsMaxSize && m_CurrentSize.width()*1.25 < availableSize.width())) || force) {
      m_CurrentRect = availableSize;
      m_CurrentRect.setHeight(m_Padding);

      //One 1000px wide line is not so useful, this may change later (variable)
      if (m_CurrentRect.width() > m_MaxWidth) {
         m_CurrentRect.setWidth( m_MaxWidth );
      }

      //Get area required to display the text
      QRect textRect(getTextRect(m_OriginalText)),decoRect(getDecorationRect());
      m_CurrentRect.setHeight(m_CurrentRect.height() + (hasText()?textRect.height():0) + m_Padding + decoRect.height());
      if (m_CurrentRect.width() - textRect.width() > 20 && m_CurrentRect.width() > decoRect.width())
         m_CurrentRect.setWidth(textRect.width()+m_Padding+textRect.x());

      if (m_CurrentRect.width() < decoRect.x() + decoRect.width())
         m_CurrentRect.setWidth(decoRect.x() + decoRect.width()+2*m_Padding);

      //Create the background image
      m_CurrentImage = QImage(QSize(m_CurrentRect.width(),m_CurrentRect.height()),QImage::Format_ARGB32);
      QPainter p(&m_CurrentImage);
      p.setCompositionMode(QPainter::CompositionMode_Clear);
      p.fillRect(m_CurrentImage.rect(),QBrush(Qt::white));
      p.setCompositionMode(QPainter::CompositionMode_SourceOver);
      p.setRenderHint(QPainter::Antialiasing, true);
      p.setFont(font());


      //Draw the tip rectangle
      if (hasBackground()) {
         p.setPen(QPen(m_OriginalPalette.base().color()));
         p.setBrush(QBrush(brightOrDarkBase()?Qt::black:Qt::white));
         p.drawRoundedRect(QRect(0,0,m_CurrentRect.width(),m_CurrentRect.height()),10,10);
      }

      if (hasText()) {
         //Draw the wrapped text in textRectS
         p.drawText(textRect,Qt::TextWordWrap|Qt::AlignJustify,m_OriginalText);
      }


      //If the widget is subclassed, this would allow decorations to be added like images
      paintDecorations(p,textRect);

      //Set the size from the RECT //TODO redundant
      m_CurrentSize = QSize(m_CurrentRect.width(),m_CurrentRect.height());

      //Notify observers that they need to reapaint
      emit changed();
   }
   return m_CurrentSize;
}

QRect Tip::getTextRect(const QString& text)
{
   QFontMetrics metric(font());
   QRect rect = metric.boundingRect(QRect(m_Padding,m_Padding,m_CurrentRect.width()-2*m_Padding,999999),Qt::AlignJustify|Qt::TextWordWrap,text);
   return rect;
}

///Check if the theme color scheme is darker than #888888
///@return true = dark, false = bright
bool Tip::brightOrDarkBase()
{
   QColor color = m_OriginalPalette.base().color();
   return (color.red() > 128 && color.green() > 128 && color.blue() > 128);
}


QRect Tip::getDecorationRect()
{
   return QRect();
}

void Tip::paintDecorations(QPainter& p, const QRect& textRect)
{
   Q_UNUSED(p)
   Q_UNUSED(textRect)
}

const QFont& Tip::font()
{
   if (!m_pFont) {
      m_pFont = new QFont();
      m_pFont->setBold(true);
   }
   return (const QFont&) *m_pFont;
}

QByteArray Tip::loadSvg(const QString& path)
{
   QFile file(path);
   if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      kDebug() << "The tip" << path << "failed to load: No such file";
   }
   else {
      m_OriginalFile = file.readAll();
      m_OriginalFile.replace("BACKGROUD_COLOR_ROLE",brightOrDarkBase()?"#000000":"#ffffff");
      m_OriginalFile.replace("BASE_ROLE_COLOR",m_OriginalPalette.base().color().name().toAscii());
      file.close();
   }
   return m_OriginalFile;
}


void Tip::setVisible(bool visible)
{
   m_IsVisible = visible;
   emit visibilityChanged(m_IsVisible);
}
