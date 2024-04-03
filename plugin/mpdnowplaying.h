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

#pragma once

#include "mpdworker.h"

#include <QObject>
#include <QThread>

class MpdNowPlaying : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString host MEMBER m_host)
    Q_PROPERTY(quint16 port MEMBER m_port)
    Q_PROPERTY(QString password MEMBER m_password)

    Q_PROPERTY(MpdState::State state READ state NOTIFY stateChanged)

    Q_PROPERTY(QString artist READ artist NOTIFY artistChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString album READ album NOTIFY albumChanged)
    Q_PROPERTY(QString time READ time NOTIFY timeChanged)

public:
    MpdNowPlaying(QObject* parent = nullptr);
    ~MpdNowPlaying() override;

    auto state() const -> MpdState::State;

    auto artist() const -> const QString&;
    auto title() const -> const QString&;
    auto album() const -> const QString&;
    auto time() const -> const QString&;

signals:
    void stateChanged();
    void artistChanged();
    void titleChanged();
    void albumChanged();
    void timeChanged();

public slots:
    void reconnect();

    void play();
    void stop();
    void next();
    void previous();

private slots:
    void updateState(MpdState::State state);
    void updateSong(const QString& artist, const QString& title, const QString& album);
    void updateTime(const QString& time);

private:
    QThread m_workerThread;
    MpdWorker* m_worker = nullptr;

    QString m_host = "localhost";
    quint16 m_port = 6600;
    QString m_password;

    MpdState::State m_state = MpdState::Unknown;

    QString m_title;
    QString m_artist;
    QString m_album;
    QString m_time;
};
