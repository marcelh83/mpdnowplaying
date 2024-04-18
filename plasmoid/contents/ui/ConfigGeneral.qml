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
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

Kirigami.FormLayout {
    property alias cfg_defaults: defaults.checked
    property alias cfg_host:     host.text
    property alias cfg_port:     port.value
    property alias cfg_password: password.text
    property alias cfg_notify:   notify.checked
    property alias cfg_timeout:  timeout.value

    FontMetrics {
        id: fontMetrics
    }

    property var textWidth: 20 * fontMetrics.xHeight

    Item {
        Kirigami.FormData.isSection: true
        Kirigami.FormData.label: i18n("Connection")

        height: 0
    }

    CheckBox {
        Kirigami.FormData.isSection: true

        id: defaults
        text: i18n("Use default")
    }

    TextField {
        Kirigami.FormData.label: i18n("Host:")
        Layout.preferredWidth: textWidth

        id: host
        enabled: !defaults.checked
    }

    SpinBox {
        Kirigami.FormData.label: i18n("Port:")

        id: port
        enabled: !defaults.checked
        from: 1
        to: 65535

        textFromValue: function(value) {
            return value;
        }
    }

    TextField {
        Kirigami.FormData.label: i18n("Password:")
        Layout.preferredWidth: textWidth

        id: password
        enabled: !defaults.checked
        echoMode: TextInput.Password
    }

    Item {
        Kirigami.FormData.isSection: true
        Kirigami.FormData.label: i18n("Notifications")

        height: 0
    }

    CheckBox {
        Kirigami.FormData.isSection: true

        id: notify
        text: i18n("Enable notifications on song change")
    }

    SpinBox {
        Kirigami.FormData.label: i18n("Duration:")

        id: timeout
        enabled: notify.checked
        from: 1
        to: 99

        textFromValue: function(value) {
            return value + " " + i18n("seconds")
        }
    }
}
