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

#pragma once

#include "mpdstate.h"

#include <QObject>
#include <QSocketNotifier>
#include <QTimer>

#include <memory>

class MpdWorker : public QObject
{
    Q_OBJECT

public:
    MpdWorker(const QString& host, quint16 port, const QString& password);
    ~MpdWorker() override;

public slots:
    void connectToDaemon();
    void disconnectFromDaemon();

    void play();
    void stop();
    void next();
    void previous();

signals:
    void stateChanged(MpdState::State state);
    void songChanged(const QString& artist, const QString& title, const QString& album);
    void timeChanged(const QString& time);

private slots:
    void processUpdate();
    void processIdle();

private:
    template <typename Func>
    void run(Func func);

    void startProcessing();
    void stopProcessing();

    void updateStatus(mpd_connection* connection);
    void updateSong(mpd_connection* connection);

    void setUnknownState();
    void setUnknownSong();
    void setUnknownTime();

    void handleError();

    static auto makeTime(unsigned int elapsed, unsigned int total) -> QString;

    static auto connectToDaemon(const QString& host, quint16 port,
                                const QString& password) -> mpd_connection*;

private:
    static constexpr auto UintMax = std::numeric_limits<unsigned int>::max();

    static constexpr auto UnknownSongId = UintMax;
    static constexpr auto UnknownTime = UintMax;

private:
    const QString m_host;
    const quint16 m_port;
    const QString m_password;

    mpd_connection* m_idleConnection = nullptr;
    mpd_connection* m_workConnection = nullptr;

    mpd_state m_state = MPD_STATE_UNKNOWN;

    QTimer m_updateTimer;
    std::unique_ptr<QSocketNotifier> m_idleNotifier;

    unsigned int m_songId = UnknownSongId;

    unsigned int m_elapsedTime = UnknownTime;
    unsigned int m_totalTime = UnknownTime;
};
