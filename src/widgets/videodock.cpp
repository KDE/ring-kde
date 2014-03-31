/***************************************************************************
 *   Copyright (C) 2011-2014 by Savoir-Faire Linux                         *
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
#include <QtGui/QGraphicsView>

#include <KLocale>

#include "videowidget3.h"
#include "videoscene.h"
#include "videotoolbar.h"
#include "actioncollection.h"
#include "extendedaction.h"

class VideoWidgetItem : public QWidgetItem {
public:
   VideoWidgetItem(VideoWidget3* wdg) : QWidgetItem(wdg),m_pWdg(wdg){}
   virtual ~VideoWidgetItem(){}
   virtual bool hasHeightForWidth () const {
      return true;
   }
   virtual int heightForWidth ( int w ) const {
      return m_pWdg->heightForWidth(w);
   }
private:
   VideoWidget3* m_pWdg;
};

///Constructor
VideoDock::VideoDock(QWidget* parent) : QDockWidget(parent)
{
   setFloating(true);
   setWindowTitle(i18nc("Video conversation","Video"));
   QWidget* wdg = new QWidget(this);

   m_pVideoWidet = new VideoWidget3(wdg);
   auto l = new QGridLayout(wdg);
//    l->addItem(new VideoWidgetItem(m_pVideoWidet),1,0);
   l->addWidget(m_pVideoWidet,1,0);
//    l->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding),0,0);
//    l->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding),2,0);
   setWidget(wdg);
   setMinimumSize(320,240);

   VideoToolbar* tb = new VideoToolbar(this);
   l->addWidget(tb,2,0);

   connect(ActionCollection::instance()->videoRotateLeftAction() ,SIGNAL(triggered(bool)),m_pVideoWidet,SLOT(slotRotateLeft()));
   connect(ActionCollection::instance()->videoRotateRightAction(),SIGNAL(triggered(bool)),m_pVideoWidet,SLOT(slotRotateRight()));
   connect(ActionCollection::instance()->videoPreviewAction()    ,SIGNAL(triggered(bool)),m_pVideoWidet,SLOT(slotShowPreview(bool)));
}

///Set current renderer
void VideoDock::addRenderer(VideoRenderer* r)
{
   Q_UNUSED(r)
   m_pVideoWidet->addRenderer(r);
}
