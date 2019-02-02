/***************************************************************************
 *   Copyright (C) 2017 by Bluesystems                                     *
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

import org.kde.playground.kquickitemviews 1.0 as KQuickItemViews
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import net.lvindustries.ringqtquick.video 1.0 as RingQtQuickVideo

GridLayout {
    property alias deviceSelectionModel    : device.selectionModel
    property alias channelSelectionModel   : channel.selectionModel
    property alias resolutionSelectionModel: resolution.selectionModel
    property alias rateSelectionModel      : rate.selectionModel

    columns: 2

    Text {
        color: "white"
        text: i18n("Device")
    }
    KQuickItemViews.ComboBoxView {
        id: device
        Layout.fillWidth: true
        selectionModel: RingSession.deviceModel.configurationProxy.deviceSelectionModel
    }

    Text {
        color: "white"
        text: i18n("Channel")
    }
    KQuickItemViews.ComboBoxView {
        id: channel
        selectionModel: RingSession.deviceModel.configurationProxy.channelSelectionModel
        Layout.fillWidth: true
    }

    Text {
        color: "white"
        text: i18n("Resolution")
    }
    KQuickItemViews.ComboBoxView {
        id: resolution
        selectionModel: RingSession.deviceModel.configurationProxy.resolutionSelectionModel
        Layout.fillWidth: true
    }

    Text {
        color: "white"
        text: i18n("Frame rate")
    }
    KQuickItemViews.ComboBoxView {
        id: rate
        selectionModel: RingSession.deviceModel.configurationProxy.rateSelectionModel
        Layout.fillWidth: true
    }
    //I got no idea why. If there is no extra row, the column width is wrong
    Item {
        Layout.fillHeight: true
    }
}
