/***************************************************************************
 *   Copyright (C) 2011-2013 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/
#include "videodock.h"

#include <QtGui/QSpacerItem>
#include <QtGui/QGridLayout>
#include <QtGui/QWidgetItem>

#include <KLocale>

#include "videowidget2.h"

class VideoWidgetItem : public QWidgetItem {
public:
   VideoWidgetItem(VideoWidget2* wdg) : QWidgetItem(wdg),m_pWdg(wdg){}
   virtual ~VideoWidgetItem(){}
   virtual bool hasHeightForWidth () const {
      return true;
   }
   virtual int heightForWidth ( int w ) const {
      return m_pWdg->heightForWidth(w);
   }
private:
   VideoWidget2* m_pWdg;
};

///Constructor
VideoDock::VideoDock(QWidget* parent) : QDockWidget(parent)
{
   setFloating(true);
   setWindowTitle(i18nc("Video conversation","Video"));
   QWidget* wdg = new QWidget(this);
   m_pVideoWidet = new VideoWidget2(wdg);
   auto l = new QGridLayout(wdg);
   l->addItem(new VideoWidgetItem(m_pVideoWidet),1,0);
   l->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding),0,0);
   l->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding),2,0);
   setWidget(wdg);
}

///Set current renderer
void VideoDock::setRenderer(VideoRenderer* r)
{
   m_pVideoWidet->setRenderer(r);
}
