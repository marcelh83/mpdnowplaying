#############################################################################
#                                                                           #
#   This file is part of MPD Now Playing.                                   #
#                                                                           #
#   Copyright (C) 2010 - 2024                                               #
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

set(BUILD_WITH_QT6 ON)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt6 REQUIRED NO_MODULE COMPONENTS Core Quick)
find_package(KF6 REQUIRED COMPONENTS Config CoreAddons I18n Notifications)
find_package(Plasma REQUIRED)

add_subdirectory(plugin)

plasma_install_package(plasmoid org.kde.plasma.mpdnowplaying)
ki18n_install(po)
