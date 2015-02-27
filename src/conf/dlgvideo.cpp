/****************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                          *
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
#include <KConfigDialog>
#include <klocalizedstring.h>

//Ring
#include "video/device.h"
#include "video/manager.h"
#include "video/resolution.h"
#include "video/channel.h"
#include "video/rate.h"
#include "video/devicemodel.h"

///Constructor
DlgVideo::DlgVideo(KConfigDialog* parent)
 : QWidget(parent),m_pDevice(nullptr),m_IsChanged(false),m_IsLoading(true),m_pChannel(nullptr),m_pResolution(nullptr)
{
   setupUi(this);

   //updateWidgets();

   connect(m_pPreviewPB   ,SIGNAL(clicked())                   , this   , SLOT(startStopPreview())     );
   connect( this          ,SIGNAL(updateButtons())             , parent , SLOT(updateButtons())        );
   connect(Video::Manager::instance(),SIGNAL(previewStateChanged(bool)),this,SLOT(startStopPreview(bool))  );

   connect(Video::Manager::instance(),SIGNAL(previewStarted(Video::Renderer*)),m_pPreviewGV,SLOT(addRenderer(Video::Renderer*))   );
   connect(Video::Manager::instance(),SIGNAL(previewStopped(Video::Renderer*)),m_pPreviewGV,SLOT(removeRenderer(Video::Renderer*)));


   if (Video::Manager::instance()->isPreviewing()) {
      m_pPreviewPB->setText(i18n("Stop preview"));
   }
//    m_pVideoSettings->slotReloadDevices();
   m_IsChanged = false;
   m_IsLoading = false;
}

///Destructor
DlgVideo::~DlgVideo()
{
   Video::Manager::instance()->stopPreview();
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
   if (Video::Manager::instance()->isPreviewing()) {
      Video::Manager::instance()->stopPreview();
   }
   else {
      Video::Manager::instance()->startPreview();
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
   Video::DeviceModel::instance()->setActive(m_pVideoSettings->device());
   m_IsChanged = false;
}

void DlgVideo::slotReloadPreview()
{
   if (Video::Manager::instance()->isPreviewing()) {
      Video::Manager::instance()->stopPreview();
      Video::Manager::instance()->startPreview();
   }
}


void DlgVideo::slotSettingsChanged()
{
   m_IsChanged = true;
   emit updateButtons();
   slotReloadPreview();
}
