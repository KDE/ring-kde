/****************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
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
#ifndef DLG_VIDEO_H
#define DLG_VIDEO_H
#ifdef ENABLE_VIDEO

#include <QtWidgets/QWidget>

#include "ui_dlgvideobase.h"
namespace Video {
   class Device;
   class Channel;
   class Resolution;
}
class KConfigDialog;


///DlgVideo: video preferences for Ring
class DlgVideo : public QWidget, public Ui_DlgVideoBase
{
Q_OBJECT
public:
   //Constructor
   explicit DlgVideo(KConfigDialog* parent = nullptr);

   //Destructor
   virtual ~DlgVideo();

   //Getter
   bool hasChanged();

private:
   //Attribute
   Video::Device* m_pDevice;
   bool m_IsChanged;
   bool m_IsLoading;
   Video::Channel* m_pChannel;
   Video::Resolution* m_pResolution;

public Q_SLOTS:
   void updateWidgets ();
   void updateSettings();
   void slotSettingsChanged();

private Q_SLOTS:
   void slotReloadPreview();
   void startStopPreview();
   void startStopPreview(bool state);

Q_SIGNALS:
   ///Emitted when the buttons need to be updated in the parent dialog
   void updateButtons();
};

#endif
#endif

// kate: space-indent on; indent-width 3; replace-tabs on;
