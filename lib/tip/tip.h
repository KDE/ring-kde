/****************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
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

//Ring
#include <QtCore/QStringList>
#include <QtCore/QSize>
#include <QtGui/QImage>
#include <QtGui/QPalette>
#include <QtWidgets/QWidget>

class QSvgRenderer;

//Qt
class QPainter;

///A tip to be passed to the TipLoader
class Q_DECL_EXPORT Tip : public QObject
{
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:
   friend class TipAnimationWrapper;

   explicit Tip(const QString& text=QString(),QWidget* parent = nullptr);
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
   bool m_HasBg {true};
   bool m_HasText {true};

protected:
   //Attributes
   QByteArray    m_OriginalFile   ;
   QString       m_OriginalText   ;
   QStringList   m_Lines          ;
   QSize         m_CurrentSize    ;
   QImage        m_CurrentImage   ;
   QRect         m_CurrentRect    ;
   QPalette      m_OriginalPalette;
   QSvgRenderer* m_pR          { nullptr                      };
   bool          m_IsMaxSize   { false                        };
   TipPosition   m_Position    { TipPosition::Bottom          };
   TipAnimation  m_AnimationIn { TipAnimation::TranslationTop };
   TipAnimation  m_AnimationOut{ TipAnimation::TranslationTop };
   QFont*        m_pFont       { nullptr                      };
   bool          m_IsVisible   { false                        };
   int           m_TimeOut     { 0                            };
   int           m_Padding     { 15                           };
   int           m_MaxWidth    { 350                          };

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
