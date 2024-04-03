/****************************************************************************
 *                                                                          *
 *   This file is part of MPD Now Playing.                                  *
 *                                                                          *
 *   Copyright (C) 2010 - 2023                                              *
 *   Marcel Hasler <mahasler@gmail.com>                                     *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation, either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.   *
 *                                                                          *
 ****************************************************************************/

import QtQuick 2.0
import QtQuick.Layouts 1.0

import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.plasma.private.mpdnowplaying 1.0

ColumnLayout {
    property var baseSize: theme.mSize(theme.defaultFont).height

    Component.onCompleted: mpd.reconnect()
    enabled: mpd.state != MpdState.Unknown

    anchors.fill: parent

    GridLayout {
        columns: 2

        Layout.leftMargin:   0.5 * baseSize
        Layout.rightMargin:  0.5 * baseSize
        Layout.topMargin:    0.5 * baseSize
        Layout.bottomMargin: 0.2 * baseSize

        component TagLabel: PlasmaComponents.Label {
            font.weight: Font.Bold
            Layout.alignment: Qt.AlignRight
        }

        component TagField: PlasmaComponents.Label {
            elide: Text.ElideRight;
            Layout.leftMargin: 0.2 * baseSize
            Layout.fillWidth: true
        }

        TagLabel { text: i18n("Artist:") }
        TagField { text: mpd.artist }

        TagLabel { text: i18n("Title:") }
        TagField { text: mpd.title }

        TagLabel { text: i18n("Album:") }
        TagField { text: mpd.album }

        TagLabel { text: i18n("Time:") }
        TagField { text: mpd.time }
    }

    RowLayout {
        Layout.leftMargin:   0.7 * baseSize
        Layout.rightMargin:  0.7 * baseSize
        Layout.topMargin:    0.0 * baseSize
        Layout.bottomMargin: 0.4 * baseSize

        component Spacer: Item {
            Layout.minimumWidth: baseSize
            Layout.fillWidth: true
        }

        ControlButton {
            source: "media-skip-backward"
            onClicked: mpd.previous()
        }

        Spacer {}

        ControlButton {
            source: (mpd.state == MpdState.Play) ? "media-playback-pause"
                                                 : "media-playback-start"
            onClicked: mpd.play()
        }

        Spacer {}

        ControlButton {
            source: "media-playback-stop"
            onClicked: mpd.stop()
        }

        Spacer {}

        ControlButton {
            source: "media-skip-forward"
            onClicked: mpd.next()
        }
    }
}
