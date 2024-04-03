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

// ---------------------------------------------------------------------------------------------- //

MpdNowPlaying::MpdNowPlaying(QObject* parent)
    : QObject(parent)
{
}

// ---------------------------------------------------------------------------------------------- //

MpdNowPlaying::~MpdNowPlaying()
{
    m_workerThread.quit();
    m_workerThread.wait();
}

// ---------------------------------------------------------------------------------------------- //

void MpdNowPlaying::reconnect()
{
    m_workerThread.quit();
    m_workerThread.wait();

    updateState(MpdState::Unknown);

    m_worker = new MpdWorker(m_host, m_port, m_password);
    m_worker->moveToThread(&m_workerThread);

    connect(m_worker, SIGNAL(stateChanged(MpdState::State)),
            this, SLOT(updateState(MpdState::State)));
    connect(m_worker, SIGNAL(songChanged(QString,QString,QString)),
            this, SLOT(updateSong(QString,QString,QString)));
    connect(m_worker, SIGNAL(timeChanged(QString)), this, SLOT(updateTime(QString)));

    connect(&m_workerThread, SIGNAL(started()), m_worker, SLOT(connectToDaemon()));
    connect(&m_workerThread, SIGNAL(finished()), m_worker, SLOT(deleteLater()));

    m_workerThread.start();
}

// ---------------------------------------------------------------------------------------------- //

auto MpdNowPlaying::state() const -> MpdState::State
{
    return m_state;
}

// ---------------------------------------------------------------------------------------------- //

auto MpdNowPlaying::artist() const -> const QString&
{
    return m_artist;
}

// ---------------------------------------------------------------------------------------------- //

auto MpdNowPlaying::title() const -> const QString&
{
    return m_title;
}

// ---------------------------------------------------------------------------------------------- //

auto MpdNowPlaying::album() const -> const QString&
{
    return m_album;
}

// ---------------------------------------------------------------------------------------------- //

auto MpdNowPlaying::time() const -> const QString&
{
    return m_time;
}

// ---------------------------------------------------------------------------------------------- //

void MpdNowPlaying::play()
{
    QMetaObject::invokeMethod(m_worker, "play", Qt::QueuedConnection);
}

// ---------------------------------------------------------------------------------------------- //

void MpdNowPlaying::stop()
{
    QMetaObject::invokeMethod(m_worker, "stop", Qt::QueuedConnection);
}

// ---------------------------------------------------------------------------------------------- //

void MpdNowPlaying::next()
{
    QMetaObject::invokeMethod(m_worker, "next", Qt::QueuedConnection);
}

// ---------------------------------------------------------------------------------------------- //

void MpdNowPlaying::previous()
{
    QMetaObject::invokeMethod(m_worker, "previous", Qt::QueuedConnection);
}

// ---------------------------------------------------------------------------------------------- //

void MpdNowPlaying::updateState(MpdState::State state)
{
    if (state != m_state)
    {
        m_state = state;
        emit stateChanged();
    }
}

// ---------------------------------------------------------------------------------------------- //

void MpdNowPlaying::updateSong(const QString& artist, const QString& title, const QString& album)
{
    if (artist != m_artist)
    {
        m_artist = artist;
        emit artistChanged();
    }

    if (title != m_title)
    {
        m_title = title;
        emit titleChanged();
    }

    if (album != m_album)
    {
        m_album = album;
        emit albumChanged();
    }
}

// ---------------------------------------------------------------------------------------------- //

void MpdNowPlaying::updateTime(const QString& time)
{
    if (time != m_time)
    {
        m_time = time;
        emit timeChanged();
    }
}

// ---------------------------------------------------------------------------------------------- //
