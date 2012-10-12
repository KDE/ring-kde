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
#ifndef SVG_TIP_LOADER
#define SVG_TIP_LOADER

//Qt
#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtGui/QPalette>
#include <QtGui/QTreeView>
#include <QtGui/QImage>

//SFLPhone
#include "../lib/typedefs.h"
class SvgTipLoader;

//Classes
class ResizeEventFilter : public QObject
{
   Q_OBJECT
public:
   ResizeEventFilter(SvgTipLoader* parent) : QObject(0) {
      m_pLoader =  parent;
   }
protected:
   bool eventFilter(QObject *obj, QEvent *event);
private:
   SvgTipLoader* m_pLoader;
};

///This class create a background brush for a QWidget with a tip window
class LIB_EXPORT SvgTipLoader : public QObject
{
   friend class ResizeEventFilter;
public:
   SvgTipLoader(QTreeView* parent, QString path, QString text, int maxLine);
   QImage getImage();

   //Setters
   void setTopMargin(int margin);
   void setBottomMargin(int margin);

   //Helper
   static QStringList stringToLineArray         (const QFont& font, QString text, int width = -1 );
   bool brightOrDarkBase();

private:
   //Methods
   void reload();

   //Attributes
   QByteArray  m_OriginalFile   ;
   QString     m_OriginalText   ;
   QStringList m_Lines          ;
   QPalette    m_OriginalPalette;
   int         m_MaxLine        ;
   QSize       m_CurrentSize    ;
   QImage      m_CurrentImage   ;
   QTreeView*  m_pParent        ;
   int         m_TopMargin      ;
   int         m_BottomMargin   ;
};

#endif