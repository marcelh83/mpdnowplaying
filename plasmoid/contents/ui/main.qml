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

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore

import org.kde.plasma.private.mpdnowplaying 1.0

Item {
    id: main

    property var baseSize: theme.mSize(theme.defaultFont).height

    property var defaultWidth: 14 * baseSize
    property var defaultHeight: 9 * baseSize

    width: defaultWidth
    height: defaultHeight

    Plasmoid.switchWidth: 0.7 * defaultWidth
    Plasmoid.switchHeight: 0.7 * defaultHeight

    Plasmoid.compactRepresentation: CompactRepresentation {}
    Plasmoid.fullRepresentation: FullRepresentation {}
    Plasmoid.preferredRepresentation: Plasmoid.fullRepresentation

    Plasmoid.toolTipTextFormat: Text.RichText

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

    PlasmaCore.DataSource {
        id: notificationSource
        engine: "notifications"
    }

    function updateTooltip()
    {
        state = mpd.state

        if (state == MpdState.Play || state == MpdState.Pause)
        {
            Plasmoid.toolTipMainText = i18n("Now playing:")

            var format = "<b>%1</b> %2 <b>%3</b>"
            Plasmoid.toolTipSubText = format.arg(mpd.title).arg(i18n("by")).arg(mpd.artist)
        }
        else
        {
            Plasmoid.toolTipMainText = Plasmoid.title

            if (state == MpdState.Unknown)
                Plasmoid.toolTipSubText = i18n("Not connected")
            else
                Plasmoid.toolTipSubText = i18n("No media playing")
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
            operation["summary"] = Plasmoid.toolTipMainText
            operation["body"]    = Plasmoid.toolTipSubText
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
