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
#include "../lib/video/videodevice.h"
#include "../lib/video/videocodecmodel.h"
#include "../lib/video/videomodel.h"
#include "../lib/video/videoresolution.h"
#include "../lib/video/videochannel.h"
#include "../lib/video/videorate.h"
#include "../lib/video/videodevicemodel.h"

///Constructor
DlgVideo::DlgVideo(KConfigDialog* parent)
 : QWidget(parent),m_pDevice(nullptr),m_IsChanged(false),m_IsLoading(true),m_pChannel(nullptr),m_pResolution(nullptr)
{
   setupUi(this);

   //updateWidgets();

   connect(m_pPreviewPB   ,SIGNAL(clicked())                   , this   , SLOT(startStopPreview())     );
   connect( this          ,SIGNAL(updateButtons())             , parent , SLOT(updateButtons())        );
   connect(VideoModel::instance(),SIGNAL(previewStateChanged(bool)),this,SLOT(startStopPreview(bool))  );

   connect(VideoModel::instance(),SIGNAL(previewStarted(VideoRenderer*)),m_pPreviewGV,SLOT(addRenderer(VideoRenderer*))   );
   connect(VideoModel::instance(),SIGNAL(previewStopped(VideoRenderer*)),m_pPreviewGV,SLOT(removeRenderer(VideoRenderer*)));


   if (VideoModel::instance()->isPreviewing()) {
      m_pPreviewPB->setText(i18n("Stop preview"));
   }
//    m_pVideoSettings->slotReloadDevices();
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
   VideoDeviceModel::instance()->setActive(m_pVideoSettings->device());
   m_IsChanged = false;
}

void DlgVideo::slotReloadPreview()
{
   if (VideoModel::instance()->isPreviewing()) {
      VideoModel::instance()->stopPreview();
      VideoModel::instance()->startPreview();
   }
}


void DlgVideo::slotSettingsChanged()
{
   m_IsChanged = true;
   emit updateButtons();
   slotReloadPreview();
}
