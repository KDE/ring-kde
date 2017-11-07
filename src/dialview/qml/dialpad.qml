/******************************************************************************
 *   Copyright (C) 2012 by Savoir-Faire Linux                                 *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>   *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU Lesser General Public               *
 *   License as published by the Free Software Foundation; either             *
 *   version 2.1 of the License, or (at your option) any later version.       *
 *                                                                            *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU        *
 *   Lesser General Public License for more details.                          *
 *                                                                            *
 *   You should have received a copy of the Lesser GNU General Public License *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/
import QtQuick 2.0

Item {
    id:dialPad

    property var buttonsText: Array("1","2","3","4","5","6","7","8","9","*","0","#")
    property var buttonsMap: Array("","abc","def","ghi","jkl","mno","pqrs","tuv","wxyz","","+","","")

    Item {
        anchors.margins: 5

        //Attributes
        anchors.fill: parent

        //Signals
        signal numbrePressed(string number)

        //Content
        Grid {
            columns: 3
            spacing: 3
            width:   parent.width
            height:  height/4*4
            Repeater {
                model: 12
                Rectangle {
                    //Attributes
                    width:  Math.min((parent.width-3.333)/3 -1, dialPad.height/4 - 2)
                    height: dialPad.height/4 - 2
                    color:  activePalette.highlight
                    radius: 999

                    //Content
                    Item {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        height: label.height + ascii.height

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            horizontalAlignment: Text.AlignRight
                            text: dialPad.buttonsText[index]
                            color: activePalette.highlightedText
                            font.bold: true
                            id:label
                        } //Text

                        Text {
                            text: dialPad.buttonsMap[index]
                            anchors.top:label.bottom
                            color: activePalette.highlightedText
                            horizontalAlignment: Text.AlignRight
                            anchors.horizontalCenter: parent.horizontalCenter
                            id: ascii
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            var call = CallModel.selectedCall

                            if (!call)
                                return

                            call.appendText(dialPad.buttonsText[index])
                        }
                    } //MouseArea
                } //Rectangle
            } //Repeater
        } //Grid
    }
}
