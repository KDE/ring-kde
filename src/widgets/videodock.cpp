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
#include <QtGui/QDesktopWidget>
#include <QtGui/QLabel>

#include <KLocale>
#include <KComboBox>

#include "videowidget3.h"
#include "videoscene.h"
#include "videotoolbar.h"
#include "actioncollection.h"
#include "extendedaction.h"
#include "lib/video/videodevicemodel.h"
#include "ui_mediafilepicker.h"
#include "ui_screensharingwidget.h"
#include "videosettings.h"

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

class MediaPicker : public QWidget, public Ui_MediaPicker
{
   Q_OBJECT
public:
   MediaPicker(QWidget* parent) : QWidget(parent)
   {
      setupUi(this);
   }
};

class ScreenSharingWidget : public QWidget, public Ui_ScreenSharing
{
   Q_OBJECT
public:
   ScreenSharingWidget(QWidget* parent) : QWidget(parent)
   {
      setupUi(this);
      setupScreenCombo();
      m_pFrame->setVisible(false);
      slotScreenIndexChanged(0);
   }
private:
   //Helper
   void setupScreenCombo();
private Q_SLOTS:
   void slotScreenIndexChanged(int idx);
};

void ScreenSharingWidget::setupScreenCombo()
{
   for (int i =0; i < QApplication::desktop()->screenCount();i++) {
      m_pScreens->addItem(QString::number(i));
   }
   m_pScreens->addItem(i18n("Custom"));
}


void ScreenSharingWidget::slotScreenIndexChanged(int idx)
{
   if (idx == m_pScreens->count()-1) {
      m_pFrame->setVisible(true);
   }
   else {
      ExtendedVideoDeviceModel::instance()->setDisplay(0,QApplication::desktop()->screenGeometry(idx));
      m_pFrame->setVisible(false);
   }
}


///Constructor
VideoDock::VideoDock(QWidget* parent) : QDockWidget(parent),m_pVideoSettings(nullptr),
   m_pScreenSharing(nullptr), m_pMediaPicker(nullptr)
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

   m_pMoreOpts = new QGridLayout(moreOptions);
   QLabel* devL = new QLabel(i18n("Device:"));
   m_pMoreOpts->addWidget(devL,1,0,1,1);

   KComboBox* device = new KComboBox(this);
   device->setModel(ExtendedVideoDeviceModel::instance());
   m_pMoreOpts->addWidget(device,1,1,2,1);
   connect(btn,SIGNAL(toggled(bool)),moreOptions,SLOT(setVisible(bool)));
   connect(device,SIGNAL(currentIndexChanged(int)),ExtendedVideoDeviceModel::instance(),SLOT(switchTo(int)));
   connect(device,SIGNAL(currentIndexChanged(int)),this,SLOT(slotDeviceChanged(int)));

   connect(ActionCollection::instance()->videoRotateLeftAction() ,SIGNAL(triggered(bool)),m_pVideoWidet,SLOT(slotRotateLeft()));
   connect(ActionCollection::instance()->videoRotateRightAction(),SIGNAL(triggered(bool)),m_pVideoWidet,SLOT(slotRotateRight()));
   connect(ActionCollection::instance()->videoPreviewAction()    ,SIGNAL(triggered(bool)),m_pVideoWidet,SLOT(slotShowPreview(bool)));
   connect(ActionCollection::instance()->videoMuteAction()       ,SIGNAL(triggered(bool)),m_pVideoWidet,SLOT(slotMuteOutgoindVideo(bool)));
   connect(ActionCollection::instance()->videoScaleAction()      ,SIGNAL(triggered(bool)),m_pVideoWidet,SLOT(slotKeepAspectRatio(bool)));
}

///Set current renderer
void VideoDock::addRenderer(VideoRenderer* r)
{
   Q_UNUSED(r)
   m_pVideoWidet->addRenderer(r);
}




void VideoDock::slotDeviceChanged(int index)
{
   switch (index) {
      case ExtendedVideoDeviceModel::ExtendedDeviceList::NONE:
         if ( m_pVideoSettings )
            m_pVideoSettings->setVisible(false);
         if ( m_pScreenSharing         )
            m_pScreenSharing->setVisible(false);
         if ( m_pMediaPicker   )
            m_pMediaPicker->setVisible(false);
         break;
      case ExtendedVideoDeviceModel::ExtendedDeviceList::SCREEN:
         if ( m_pVideoSettings  )
            m_pVideoSettings->setVisible(false);
         if ( !m_pScreenSharing ) {
            m_pScreenSharing = new ScreenSharingWidget(this);
            m_pMoreOpts->addWidget(m_pScreenSharing,10,0,1,4);
         }
         if ( m_pMediaPicker    )
            m_pMediaPicker->setVisible(false);
         m_pScreenSharing->setVisible(true);
         break;
      case ExtendedVideoDeviceModel::ExtendedDeviceList::FILE:
         if ( m_pVideoSettings )
            m_pVideoSettings->setVisible(false);
         if ( m_pScreenSharing )
            m_pScreenSharing->setVisible(false);
         if ( !m_pMediaPicker  ) {
            m_pMediaPicker = new MediaPicker(this);
            m_pMoreOpts->addWidget(m_pMediaPicker,11,0,1,4);
            connect(m_pMediaPicker->m_pPicker,SIGNAL(urlSelected(KUrl)),
               this,SLOT(slotFileSelected(KUrl)));
         }
         m_pMediaPicker->setVisible(true);
         break;
      default:
         if ( !m_pVideoSettings ) {
            m_pVideoSettings = new VideoSettings(this);
            m_pVideoSettings->hideDevices();
            m_pMoreOpts->addWidget(m_pVideoSettings,12,0,1,4);
         }
         if ( m_pScreenSharing  )
            m_pScreenSharing->setVisible(false);
         if ( m_pMediaPicker    )
            m_pMediaPicker->setVisible(false);
         m_pVideoSettings->setDevice(ExtendedVideoDeviceModel::instance()->deviceAt(
            ExtendedVideoDeviceModel::instance()->index(index,0))
         );
         m_pVideoSettings->setVisible(true);
   };
}


void VideoDock::slotFileSelected(const KUrl& url)
{
   ExtendedVideoDeviceModel::instance()->setFile(url);
}

#include "moc_videodock.cpp"
#include "videodock.moc"
