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
#include "../lib/videodevicemodel.h"

///Constructor
DlgVideo::DlgVideo(KConfigDialog* parent)
 : QWidget(parent),m_pDevice(nullptr),m_IsChanged(false),m_IsLoading(true)
{
   setupUi(this);

   updateWidgets();

   const QList<VideoDevice*> devices =  VideoModel::instance()->devices();

   m_pDeviceCB     -> setModel(VideoDeviceModel::instance());
   m_pChannelCB    -> setModel(VideoDeviceModel::instance()->channelModel());
   m_pResolutionCB -> setModel(VideoDeviceModel::instance()->resolutionModel());
   m_pRateCB       -> setModel(VideoDeviceModel::instance()->rateModel());

   connect(m_pDeviceCB    ,SIGNAL(currentIndexChanged(int)), VideoDeviceModel::instance()                   , SLOT(setActive(int)));
   connect(m_pChannelCB   ,SIGNAL(currentIndexChanged(int)), VideoDeviceModel::instance()->channelModel()   , SLOT(setActive(int)));
   connect(m_pResolutionCB,SIGNAL(currentIndexChanged(int)), VideoDeviceModel::instance()->resolutionModel(), SLOT(setActive(int)));
   connect(m_pRateCB      ,SIGNAL(currentIndexChanged(int)), VideoDeviceModel::instance()->rateModel()      , SLOT(setActive(int)));

//    connect(VideoDeviceModel::instance()                   ,SIGNAL(currentIndexChanged(int)), m_pDeviceCB    , SLOT(setCurrentIndex(int)));
   connect(VideoDeviceModel::instance()->channelModel()   ,SIGNAL(currentIndexChanged(int)), m_pChannelCB   , SLOT(setCurrentIndex(int)));
   connect(VideoDeviceModel::instance()->resolutionModel(),SIGNAL(currentIndexChanged(int)), m_pResolutionCB, SLOT(setCurrentIndex(int)));
   connect(VideoDeviceModel::instance()->rateModel()      ,SIGNAL(currentIndexChanged(int)), m_pRateCB      , SLOT(setCurrentIndex(int)));

   qDebug() << "SETTING" << VideoDeviceModel::instance()-> currentIndex() << m_pDeviceCB->count();
   m_pDeviceCB    -> setCurrentIndex( VideoDeviceModel::instance()                   -> currentIndex() );
   m_pChannelCB   -> setCurrentIndex( VideoDeviceModel::instance()->channelModel()   -> currentIndex() );
   m_pResolutionCB-> setCurrentIndex( VideoDeviceModel::instance()->resolutionModel()-> currentIndex() );
   m_pRateCB      -> setCurrentIndex( VideoDeviceModel::instance()->rateModel()      -> currentIndex() );

   connect(m_pPreviewPB   ,SIGNAL(clicked())                   , this   , SLOT(startStopPreview())     );
   connect( this          ,SIGNAL(updateButtons())             , parent , SLOT(updateButtons())        );
   connect(VideoModel::instance(),SIGNAL(previewStateChanged(bool)),this,SLOT(startStopPreview(bool))  );

   connect(VideoModel::instance(),SIGNAL(previewStarted(VideoRenderer*)),m_pPreviewGV,SLOT(addRenderer(VideoRenderer*))   );
   connect(VideoModel::instance(),SIGNAL(previewStopped(VideoRenderer*)),m_pPreviewGV,SLOT(removeRenderer(VideoRenderer*)));


   m_pConfGB->setEnabled(devices.size());

   if ((devices.size() && devices[0]) || (devices[0] && (!m_pDeviceCB->count())))
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
   Q_UNUSED(device)
   if (!m_IsLoading) {
      m_IsChanged = true;
      emit updateButtons();
   }
}

///Load resolution
void DlgVideo::loadResolution(const QString& channel)
{
   Q_UNUSED(channel)
   if (!m_IsLoading) {
      m_IsChanged = true;
      emit updateButtons();
   }
}

///Load the rate
void DlgVideo::loadRate(const QString& resolution)
{
   Q_UNUSED(resolution)
   if (!m_IsLoading) {
      m_IsChanged = true;
      emit updateButtons();
   }
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
      VideoModel::instance()->stopPreview();
   }
   else {
      VideoModel::instance()->startPreview();
   }
}

void DlgVideo::startStopPreview(bool state)
{
   if (state) {
      m_pPreviewPB->setText(i18n("Stop preview"));
   }
   else {
      m_pPreviewPB->setText(i18n("Start preview"));
   }
}

void DlgVideo::updateWidgets ()
{
   //The models should take care of that
}

void DlgVideo::updateSettings()
{
   const QList<VideoDevice*> devices =  VideoModel::instance()->devices();
   VideoDeviceModel::instance()->setActive(devices.size() > m_pDeviceCB->currentIndex()? devices[m_pDeviceCB->currentIndex()]:nullptr);
   m_IsChanged = false;
}
