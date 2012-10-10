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
   ,m_MaxLine(maxLine),m_OriginalPalette(parent->palette()),m_pParent(parent)
{
   QFile file(path);
   if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      kDebug() << "The tip" << path << "failed to load: No such file";
   }
   else {
      ResizeEventFilter* filter = new ResizeEventFilter(this);
      parent->installEventFilter(filter);
      m_OriginalFile = file.readAll();
      for (int i=0;i<maxLine;i++)
         m_OriginalFile.replace("BASE_ROLE_COLOR",m_OriginalPalette.base().color().name().toAscii());
      r = new QSvgRenderer(m_OriginalFile);
      reload();
   }
}

///Get the current image
//TODO remove this and use event filter to automagically update the parent widget
QImage SvgTipLoader::getImage()
{
   return m_CurrentImage;
}

///Reload the SVG with new size
void SvgTipLoader::reload()
{
   int width(m_pParent->width()),height(m_pParent->height());
   m_CurrentImage = QImage(QSize(width,height),QImage::Format_RGB888);
   m_CurrentImage.fill( m_OriginalPalette.base().color() );
   QPainter p(&m_CurrentImage);
   r->render(&p,QRect(10,(height-((width-30)*0.539723102))/2,width-30,(width-30)*0.539723102));

   QPalette p2 = m_pParent->viewport()->palette();
   p2.setBrush(QPalette::Base, QBrush(m_CurrentImage));
   m_pParent->viewport()->setPalette(p2);
   m_pParent->setPalette(p2);
}