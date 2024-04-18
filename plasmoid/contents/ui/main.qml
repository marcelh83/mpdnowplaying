/****************************************************************************
 *                                                                          *
 *   This file is part of MPD Now Playing.                                  *
 *                                                                          *
 *   Copyright (C) 2010 - 2024                                              *
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

import QtQuick

import org.kde.kirigami as Kirigami

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasma5support as Plasma5Support
import org.kde.plasma.plasmoid

import org.kde.plasma.private.mpdnowplaying

PlasmoidItem {
    id: main

    property var baseSize: Kirigami.Units.gridUnit

    property var defaultWidth: 14 * baseSize
    property var defaultHeight: 9 * baseSize

    width: defaultWidth
    height: defaultHeight

    switchWidth: 0.7 * defaultWidth
    switchHeight: 0.7 * defaultHeight

    compactRepresentation: CompactRepresentation {}
    fullRepresentation: FullRepresentation {}
    preferredRepresentation: fullRepresentation

    toolTipTextFormat: Text.RichText

    property var active: mpd.state == MpdState.Play || mpd.state == MpdState.Pause
    Plasmoid.status: active ? PlasmaCore.Types.ActiveStatus : PlasmaCore.Types.PassiveStatus

    Component.onCompleted: {
        setConfig() // Only set here, representation must connect for properties to update properly
        updateTooltip()
    }

    MpdNowPlaying {
        id: mpd

        onStateChanged: updateTooltip()

        onTitleChanged: {
            updateTooltip()
            notify()
        }
    }

    Plasma5Support.DataSource {
        id: notificationSource
        engine: "notifications"
    }

    function updateTooltip()
    {
        state = mpd.state

        if (state == MpdState.Play || state == MpdState.Pause)
        {
            toolTipMainText = i18n("Now playing:")

            var format = "<b>%1</b> %2 <b>%3</b>"
            toolTipSubText = format.arg(mpd.title).arg(i18n("by")).arg(mpd.artist)
        }
        else
        {
            toolTipMainText = Plasmoid.title

            if (state == MpdState.Unknown)
                toolTipSubText = i18n("Not connected")
            else
                toolTipSubText = i18n("No media playing")
        }
    }

    function notify()
    {
        if (plasmoid.configuration.notify && mpd.state == MpdState.Play)
        {
            var service = notificationSource.serviceForSource("notification")
            var operation = service.operationDescription("createNotification")

            operation["appName"] = Plasmoid.title
            operation["appIcon"] = Plasmoid.icon
            operation["summary"] = toolTipMainText
            operation["body"]    = toolTipSubText
            operation["timeout"] = plasmoid.configuration.timeout * 1000

            service.startOperationCall(operation);
        }
    }

    function setConfig()
    {
        if (plasmoid.configuration.defaults)
        {
            mpd.host = "localhost"
            mpd.port = 6600
            mpd.password = ""
        }
        else
        {
            mpd.host = plasmoid.configuration.host
            mpd.port = plasmoid.configuration.port
            mpd.password = plasmoid.configuration.password
        }
    }

    function updateConfig()
    {
        setConfig()
        mpd.reconnect()
    }

    Connections {
        target: plasmoid.configuration

        function onDefaultsChanged() {
            Qt.callLater(updateConfig)
        }

        function onHostChanged() {
            Qt.callLater(updateConfig)
        }

        function onPortChanged() {
            Qt.callLater(updateConfig)
        }

        function onPasswordChanged() {
            Qt.callLater(updateConfig)
        }
    }
}
