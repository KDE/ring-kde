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
#include "dlgvideo.h"

//KDE
#include <KDebug>
#include <KConfigDialog>
#include <KLocale>

//SFLPhone
#include "../lib/videodevice.h"
#include "../lib/videocodecmodel.h"
#include "../lib/videomodel.h"

///Constructor
DlgVideo::DlgVideo(KConfigDialog* parent)
 : QWidget(parent),m_pDevice(nullptr),m_IsChanged(false),m_IsLoading(true)
{
   setupUi(this);

   updateWidgets();

   const QList<VideoDevice*> devices =  VideoModel::instance()->devices();

   connect(m_pDeviceCB    ,SIGNAL(currentIndexChanged(QString)), this   , SLOT(loadDevice(QString))    );
   connect(m_pChannelCB   ,SIGNAL(currentIndexChanged(QString)), this   , SLOT(loadResolution(QString)));
   connect(m_pResolutionCB,SIGNAL(currentIndexChanged(QString)), this   , SLOT(loadRate(QString))      );
   connect(m_pRateCB      ,SIGNAL(currentIndexChanged(QString)), this   , SLOT(changeRate(QString))    );
   connect(m_pPreviewPB   ,SIGNAL(clicked())                   , this   , SLOT(startStopPreview())     );
   connect( this          ,SIGNAL(updateButtons())             , parent , SLOT(updateButtons())        );


   m_pConfGB->setEnabled(devices.size());

   if (devices.size())
      loadDevice(devices[0]->id());

   if (VideoModel::instance()->isPreviewing()) {
      m_pPreviewPB->setText(i18n("Stop preview"));
   }
   m_IsChanged = false;
   m_IsLoading = false;
}

///Destructor
DlgVideo::~DlgVideo()
{
   VideoModel::instance()->stopPreview();
}

///Has the dialog chnaged
bool DlgVideo::hasChanged()
{
   return m_IsChanged;
}

///Load the device list
void DlgVideo::loadDevice(const QString& device)
{
   if (!m_IsLoading) {
      m_IsChanged = true;
      emit updateButtons();
   }
   m_pDevice = VideoModel::instance()->device(device);
   if (m_pDevice) {
      const QString curChan = m_pDevice->channel();
      m_pChannelCB->clear();
      foreach(const VideoChannel& channel,m_pDevice->channelList()) {
         m_pChannelCB->addItem(channel);
         if (channel == curChan)
            m_pChannelCB->setCurrentIndex(m_pChannelCB->count()-1);
      }
   }
}

///Load resolution
void DlgVideo::loadResolution(const QString& channel)
{
   if (!m_IsLoading) {
      m_IsChanged = true;
      emit updateButtons();
   }
   Resolution current = m_pDevice->resolution();
   m_pResolutionCB->clear();
   foreach(const Resolution& res,m_pDevice->resolutionList(channel)) {
      m_pResolutionCB->addItem(res.toString());
      if (current == res) {
         m_pResolutionCB->setCurrentIndex(m_pResolutionCB->count()-1);
      }
   }
   m_pDevice->setChannel(channel);
}

///Load the rate
void DlgVideo::loadRate(const QString& resolution)
{
   if (!m_IsLoading) {
      m_IsChanged = true;
      emit updateButtons();
   }
   m_pRateCB->clear();
   const QString rate = m_pDevice->rate();
   foreach(const QString& r,m_pDevice->rateList(m_pChannelCB->currentText(),resolution)) {
      m_pRateCB->addItem(r);
      if (r == rate)
         m_pRateCB->setCurrentIndex(m_pRateCB->count()-1);
   }
   m_pDevice->setResolution(resolution);
}

///Changes the rate
void DlgVideo::changeRate(const QString& rate)
{
   if (!m_IsLoading) {
      m_IsChanged = true;
      emit updateButtons();
   }
   m_pDevice->setRate(rate);
}

///Start or stop preview
void DlgVideo::startStopPreview()
{
   //TODO check if the preview is already running
   if (VideoModel::instance()->isPreviewing()) {
      m_pPreviewPB->setText(i18n("Start preview"));
      VideoModel::instance()->stopPreview();
   }
   else {
      m_pPreviewPB->setText(i18n("Stop preview"));
      VideoModel::instance()->startPreview();
   }
}


void DlgVideo::updateWidgets ()
{
   const QList<VideoDevice*> devices =  VideoModel::instance()->devices();
   m_pDeviceCB->clear();
   foreach(VideoDevice* dev,devices) {
      m_pDeviceCB->addItem(dev->id());
   }
   m_pDeviceCB->setCurrentIndex(devices.indexOf(VideoModel::instance()->activeDevice()));
}

void DlgVideo::updateSettings()
{
   const QList<VideoDevice*> devices =  VideoModel::instance()->devices();
   VideoModel::instance()->setActiveDevice(devices[m_pDeviceCB->currentIndex()]);
   m_IsChanged = false;
}
