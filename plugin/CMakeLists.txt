#############################################################################
#                                                                           #
#   This file is part of MPD Now Playing.                                   #
#                                                                           #
#   Copyright (C) 2010 - 2023                                               #
#   Marcel Hasler <mahasler@gmail.com>                                      #
#                                                                           #
#   This program is free software; you can redistribute it and/or modify    #
#   it under the terms of the GNU General Public License as published by    #
#   the Free Software Foundation, either version 3 of the License, or       #
#   (at your option) any later version.                                     #
#                                                                           #
#   This program is distributed in the hope that it will be useful,         #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of          #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            #
#   GNU General Public License for more details.                            #
#                                                                           #
#   You should have received a copy of the GNU General Public License       #
#   along with this program. If not, see <http://www.gnu.org/licenses/>.    #
#                                                                           #
#############################################################################

set(mpdnowplayingplugin_SRCS
    mpdnowplaying.cpp
    mpdnowplaying.h
    mpdnowplayingplugin.cpp
    mpdnowplayingplugin.h
    mpdstate.h
    mpdworker.cpp
    mpdworker.h
)

qt5_wrap_cpp(mpdnowplayingplugin_MOCS mpdstate.h)

set(mpdnowplayingplugin_LIBS
    mpdclient
    Qt5::Core
    Qt5::Quick
    KF5::ConfigCore
    KF5::CoreAddons
    KF5::I18n
    KF5::Notifications
)

add_library(mpdnowplayingplugin SHARED ${mpdnowplayingplugin_SRCS} ${mpdnowplayingplugin_MOCS})
target_link_libraries(mpdnowplayingplugin ${mpdnowplayingplugin_LIBS})

set(mpdnowplayingplugin_DEST ${QML_INSTALL_DIR}/org/kde/plasma/private/mpdnowplaying)

install(TARGETS mpdnowplayingplugin DESTINATION ${mpdnowplayingplugin_DEST})
install(FILES qmldir DESTINATION ${mpdnowplayingplugin_DEST})
