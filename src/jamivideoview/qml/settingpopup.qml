/***************************************************************************
 *   Copyright (C) 2019 by Bluesystems                                     *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
import QtQuick 2.7
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2 as Controls
import org.kde.ringkde.jamivideoview 1.0 as JamiVideoView
import org.kde.ringkde.jamicallview 1.0 as JamiCallView

Controls.Dialog {
    standardButtons: Controls.Dialog.Save | Controls.Dialog.Cancel

    parent: applicationWindow().contentItem
    x: applicationWindow().contentItem.width / 2 - width/2
    y: applicationWindow().contentItem.height / 2 - height/2

    width:  applicationWindow().contentItem.width  * 0.85
    height: applicationWindow().contentItem.height * 0.85

    contentItem: JamiVideoView.VideoWidget {
        id: videoWidget
        JamiVideoView.DeviceSetting {
            anchors.fill: parent
        }
    }

    Component.onCompleted: {
        RingSession.previewManager.startPreview()
        videoWidget.started = true
    }

    onAccepted: {
        RingSession.previewManager.stopPreview()
        videoWidget.started = false
    }

    onRejected: {
        videoWidget.started = false
        RingSession.previewManager.stopPreview()
    }
}
