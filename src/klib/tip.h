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
#ifndef TIP_H
#define TIP_H
//Base
#include <QtCore/QObject>

//SFLPhone
#include <QtCore/QStringList>
#include <QtCore/QSize>
#include <QtGui/QImage>
#include <QtGui/QPalette>
#include <QtGui/QWidget>

class QSvgRenderer;

//SFLPhone
#include "../lib/typedefs.h"

//Qt
class QPainter;

///A tip to be passed to the TipLoader
class Tip : public QObject
{
   Q_OBJECT
public:
   friend class TipManager;
   friend class TipAnimationWrapper;
   Tip(QWidget* parent = nullptr,const QString& path="", const QString& text="", int maxLine=4);
   virtual ~Tip();

   //Enum
   enum TipPosition {
      Top,
      Middle,
      Bottom
   };

   enum TipAnimation {
      Fade,
      TranslationTop,
      TranslationBottom,
      TranslationLeft,
      TranslationRight,
      None
   };

   //Mutator
   QSize reload(const QRect& availableSize);

protected:
   //Attributes
   TipPosition   m_Position       ;
   QByteArray    m_OriginalFile   ;
   QString       m_OriginalText   ;
   QStringList   m_Lines          ;
   int           m_MaxLine        ;
   QSize         m_CurrentSize    ;
   QImage        m_CurrentImage   ;
   QRect         m_CurrentRect    ;
   bool          m_IsMaxSize      ;
   QSvgRenderer* m_pR             ;
   QPalette      m_OriginalPalette;
   TipAnimation  m_AnimationIn    ;
   TipAnimation  m_AnimationOut   ;
   QFont*        m_pFont          ;

   static const int PADDING = 15;
   static const int MAX_WIDTH = 350;

   //Helper
   bool brightOrDarkBase();
   QString loadSvg(const QString& path);

   //To reimplement if needed
   virtual QRect getTextRect(const QString& text);
   virtual QRect getDecorationRect();
   virtual void  paintDecorations(QPainter& p, const QRect& textRect);
   virtual const QFont& font();
};

#endif