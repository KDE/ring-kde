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
class LIB_EXPORT Tip : public QObject
{
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:
   friend class TipAnimationWrapper;

   explicit Tip(const QString& text="",QWidget* parent = nullptr);
   virtual ~Tip();

   //Enum
   enum TipPosition {
      Top   ,
      Middle,
      Bottom,
   };

   enum class TipAnimation {
      Fade             ,
      TranslationTop   ,
      TranslationBottom,
      TranslationLeft  ,
      TranslationRight ,
      None             ,
   };

   //Mutator
   virtual QSize reload(const QRect& availableSize,bool force = false);

   //Getter
   bool isVisible        () { return m_IsVisible;    }
   int  timeout          () { return m_TimeOut;      }
   bool hasText          () { return m_HasText;      }
   bool hasBackground    () { return m_HasBg;        }
   virtual float opacity () { return 1.0f;           }
   QString text          () { return m_OriginalText; }

   //Setter
   void setVisible(bool visible);
   void setTimeOut      ( int  timeOut      ) { m_TimeOut      = timeOut; }
   void setAnimationIn  ( TipAnimation a    ) { m_AnimationIn  = a;       }
   void setAnimationOut ( TipAnimation a    ) { m_AnimationOut = a;       }
   void setMaximumWidth ( int width         ) { m_MaxWidth     = width;   }
   void setText         (const QString& text) {
      if (text != m_OriginalText) {
         m_OriginalText = text;
         emit changed();
         reload(m_CurrentRect,true);
      }
   }

private:
   bool m_HasBg;
   bool m_HasText;

protected:
   //Attributes
   TipPosition   m_Position       ;
   QByteArray    m_OriginalFile   ;
   QString       m_OriginalText   ;
   QStringList   m_Lines          ;
   QSize         m_CurrentSize    ;
   QImage        m_CurrentImage   ;
   QRect         m_CurrentRect    ;
   bool          m_IsMaxSize      ;
   QSvgRenderer* m_pR             ;
   QPalette      m_OriginalPalette;
   TipAnimation  m_AnimationIn    ;
   TipAnimation  m_AnimationOut   ;
   QFont*        m_pFont          ;
   bool          m_IsVisible      ;
   int           m_TimeOut        ;
   int           m_Padding        ;
   int           m_MaxWidth       ;

   //Helper
   bool brightOrDarkBase();
   QByteArray loadSvg(const QString& path);

   //Protected setters
   void setHasText       (bool value) { m_HasText = value; }
   void setHasBackground (bool value) { m_HasBg = value;   }

   //To reimplement if needed
   virtual QRect getTextRect       ( const QString& text                );
   virtual QRect getDecorationRect (                                    );
   virtual void  paintDecorations  ( QPainter& p, const QRect& textRect );
   virtual const QFont& font       (                                    );
Q_SIGNALS:
   void visibilityChanged(bool);
   void changed();
};

#endif
