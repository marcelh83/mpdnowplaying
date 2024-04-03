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

cmake_minimum_required(VERSION 3.16)
project(MPDNowPlaying)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

find_package(KF5 REQUIRED COMPONENTS Config CoreAddons I18n Notifications Plasma)
find_package(Qt5 REQUIRED COMPONENTS Core Quick)

add_subdirectory(plugin)
add_subdirectory(po)

plasma_install_package(plasmoid org.kde.plasma.mpdnowplaying)
