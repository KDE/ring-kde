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
#include <QtGui/QLabel>

#include <KLocale>
#include <KComboBox>

#include "videowidget3.h"
#include "videoscene.h"
#include "videotoolbar.h"
#include "actioncollection.h"
#include "extendedaction.h"
#include "lib/videodevicemodel.h"

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
   m_pVideoWidet->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
   auto l = new QGridLayout(wdg);
//    l->addItem(new VideoWidgetItem(m_pVideoWidet),1,0);
   l->addWidget(m_pVideoWidet,1,0,1,3);
//    l->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding),0,0);
//    l->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding),2,0);
   setWidget(wdg);
   setMinimumSize(320,240);

   VideoToolbar* tb = new VideoToolbar(this);
   l->addWidget(tb,2,0);

   QToolButton* btn = new QToolButton(this);
   btn->setIcon(KIcon("arrow-down-double"));
   btn->setCheckable(true);
   l->addWidget(btn,2,2);

   QWidget* moreOptions = new QWidget(this);
   moreOptions->setVisible(false);
   moreOptions->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
   l->addWidget(moreOptions,3,0,1,3);

   QGridLayout* moreOpts = new QGridLayout(moreOptions);
   QLabel* devL = new QLabel(i18n("Device:"));
   QLabel* resL = new QLabel(i18n("Resolution:"));
   moreOpts->addWidget(devL,1,0,1,1);
   moreOpts->addWidget(resL,1,1,1,1);

   KComboBox* device = new KComboBox(this);
   KComboBox* res    = new KComboBox(this);
   device->setModel(VideoDeviceModel::instance());
   moreOpts->addWidget(device,1,0,2,1);
   moreOpts->addWidget(res   ,1,1,2,1);
   connect(btn,SIGNAL(toggled(bool)),moreOptions,SLOT(setVisible(bool)));
   connect(device,SIGNAL(currentIndexChanged(int)),VideoDeviceModel::instance(),SLOT(setActive(int)));

   connect(ActionCollection::instance()->videoRotateLeftAction() ,SIGNAL(triggered(bool)),m_pVideoWidet,SLOT(slotRotateLeft()));
   connect(ActionCollection::instance()->videoRotateRightAction(),SIGNAL(triggered(bool)),m_pVideoWidet,SLOT(slotRotateRight()));
   connect(ActionCollection::instance()->videoPreviewAction()    ,SIGNAL(triggered(bool)),m_pVideoWidet,SLOT(slotShowPreview(bool)));
   connect(ActionCollection::instance()->videoMuteAction()       ,SIGNAL(triggered(bool)),m_pVideoWidet,SLOT(slotMuteOutgoindVideo(bool)));
}

///Set current renderer
void VideoDock::addRenderer(VideoRenderer* r)
{
   Q_UNUSED(r)
   m_pVideoWidet->addRenderer(r);
}
