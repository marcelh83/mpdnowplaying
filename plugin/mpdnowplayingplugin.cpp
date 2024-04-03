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

#include "mpdnowplaying.h"
#include "mpdnowplayingplugin.h"

#include <QtQml>

// ---------------------------------------------------------------------------------------------- //

void MpdNowPlayingPlugin::registerTypes(const char* uri)
{
    Q_ASSERT(uri == QLatin1String("org.kde.plasma.private.mpdnowplaying"));

    qmlRegisterType<MpdNowPlaying>(uri, 1, 0, "MpdNowPlaying");
    qmlRegisterUncreatableType<MpdState>(uri, 1, 0, "MpdState", "Exports MpdState to QML.");

    qRegisterMetaType<MpdState::State>("MpdState::State");
}

// ---------------------------------------------------------------------------------------------- //
