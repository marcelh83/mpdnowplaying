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

#include "mpdworker.h"

#include <QTime>

// ---------------------------------------------------------------------------------------------- //

using namespace std::chrono_literals;

// ---------------------------------------------------------------------------------------------- //

template <typename T, void(*freeFunc)(T*)>
class MpdGuard
{
public:
    MpdGuard(T* ptr) : m_ptr(ptr) {}
    ~MpdGuard() { freeFunc(m_ptr); }

private:
    T* m_ptr;
};

// ---------------------------------------------------------------------------------------------- //

using StatusGuard = MpdGuard<mpd_status, mpd_status_free>;
using SongGuard = MpdGuard<mpd_song, mpd_song_free>;

// ---------------------------------------------------------------------------------------------- //

MpdWorker::MpdWorker(const QString& host, quint16 port, const QString& password)
    : m_host(host),
      m_port(port),
      m_password(password),
      m_updateTimer(this)
{
    m_updateTimer.setInterval(1s);
    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(processUpdate()));
}

// ---------------------------------------------------------------------------------------------- //

MpdWorker::~MpdWorker()
{
    if (m_idleConnection || m_workConnection)
        disconnectFromDaemon();
}

// ---------------------------------------------------------------------------------------------- //

void MpdWorker::connectToDaemon()
{
    if (m_idleConnection || m_workConnection)
        disconnectFromDaemon();

    m_idleConnection = connectToDaemon(m_host, m_port, m_password);
    m_workConnection = connectToDaemon(m_host, m_port, m_password);

    if (m_idleConnection && m_workConnection)
    {
        mpd_connection_set_keepalive(m_idleConnection, true);
        mpd_connection_set_keepalive(m_workConnection, true);

        updateStatus(m_idleConnection);

        if (m_state == MPD_STATE_PLAY || m_state == MPD_STATE_PAUSE)
            updateSong(m_idleConnection);

        if (m_state != MPD_STATE_UNKNOWN)
            startProcessing();
    }
    else
        QTimer::singleShot(1s, this, SLOT(connectToDaemon()));
}

// ---------------------------------------------------------------------------------------------- //

void MpdWorker::disconnectFromDaemon()
{
    stopProcessing();

    setUnknownState();
    setUnknownSong();
    setUnknownTime();

    if (m_idleConnection)
    {
        mpd_connection_free(m_idleConnection);
        m_idleConnection = nullptr;
    }

    if (m_workConnection)
    {
        mpd_connection_free(m_workConnection);
        m_workConnection = nullptr;
    }
}

// ---------------------------------------------------------------------------------------------- //

void MpdWorker::play()
{
    const auto playOrPause = [this](mpd_connection* connection)
    {
        if (m_state == MPD_STATE_PLAY)
            return mpd_run_pause(connection, true);
        else
            return mpd_run_play(connection);
    };

    run(playOrPause);
}

// ---------------------------------------------------------------------------------------------- //

void MpdWorker::stop()
{
    if (m_state == MPD_STATE_PLAY || m_state == MPD_STATE_PAUSE)
        run(mpd_run_stop);
}

// ---------------------------------------------------------------------------------------------- //

void MpdWorker::next()
{
    if (m_state == MPD_STATE_PLAY || m_state == MPD_STATE_PAUSE)
        run(mpd_run_next);
}

// ---------------------------------------------------------------------------------------------- //

void MpdWorker::previous()
{
    if (m_state == MPD_STATE_PLAY || m_state == MPD_STATE_PAUSE)
        run(mpd_run_previous);
}

// ---------------------------------------------------------------------------------------------- //

void MpdWorker::processUpdate()
{
    if (!m_workConnection)
        return;

    const bool success = mpd_send_command(m_workConnection, "ping", nullptr) &&
                         mpd_response_finish(m_workConnection);
    if (!success)
        return handleError();

    if (m_state == MPD_STATE_PLAY)
    {
        ++m_elapsedTime;
        emit timeChanged(makeTime(m_elapsedTime, m_totalTime));
    }
}

// ---------------------------------------------------------------------------------------------- //

void MpdWorker::processIdle()
{
    if (!mpd_recv_idle(m_idleConnection, false))
        return handleError();

    updateStatus(m_idleConnection);

    if (m_state == MPD_STATE_PLAY || m_state == MPD_STATE_PAUSE)
        updateSong(m_idleConnection);
    else
    {
        setUnknownSong();
        setUnknownTime();
    }

    if (m_state != MPD_STATE_UNKNOWN)
    {
        mpd_send_idle_mask(m_idleConnection, MPD_IDLE_PLAYER);
        m_updateTimer.start();
    }
}

// ---------------------------------------------------------------------------------------------- //

template <typename Func>
void MpdWorker::run(Func func)
{
    if (m_workConnection && !func(m_workConnection))
        mpd_connection_clear_error(m_workConnection);
}

// ---------------------------------------------------------------------------------------------- //

void MpdWorker::startProcessing()
{
    const int fd = mpd_connection_get_fd(m_idleConnection);
    m_idleNotifier = std::make_unique<QSocketNotifier>(fd, QSocketNotifier::Read);

    connect(m_idleNotifier.get(), SIGNAL(activated(QSocketDescriptor,QSocketNotifier::Type)),
            this, SLOT(processIdle()));

    mpd_send_idle_mask(m_idleConnection, MPD_IDLE_PLAYER);
    m_updateTimer.start();
}

// ---------------------------------------------------------------------------------------------- //

void MpdWorker::stopProcessing()
{
    m_updateTimer.stop();
    m_idleNotifier = nullptr;
}

// ---------------------------------------------------------------------------------------------- //

void MpdWorker::updateStatus(mpd_connection* connection)
{
    mpd_status* status = mpd_run_status(connection);

    if (!status)
        return handleError();

    StatusGuard guard(status);

    const mpd_state state = mpd_status_get_state(status);

    if (state != m_state)
    {
        m_state = state;
        emit stateChanged(static_cast<MpdState::State>(m_state));
    }

    if (m_state == MPD_STATE_PLAY || m_state == MPD_STATE_PAUSE)
    {
        const unsigned int elapsed = mpd_status_get_elapsed_ms(status) / 1000;
        const unsigned int total = mpd_status_get_total_time(status);

        if (elapsed != m_elapsedTime || total != m_totalTime)
        {
            m_elapsedTime = elapsed;
            m_totalTime = total;

            emit timeChanged(makeTime(m_elapsedTime, m_totalTime));
        }
    }
}

// ---------------------------------------------------------------------------------------------- //

void MpdWorker::updateSong(mpd_connection* connection)
{
    mpd_song* song = mpd_run_current_song(connection);

    if (!song) // May happen if stopped while processing
        return;

    SongGuard guard(song);

    const unsigned int id = mpd_song_get_id(song);

    if (id != m_songId)
    {
        m_songId = id;

        const QString artist(mpd_song_get_tag(song, MPD_TAG_ARTIST, 0));
        const QString title(mpd_song_get_tag(song, MPD_TAG_TITLE, 0));
        const QString album(mpd_song_get_tag(song, MPD_TAG_ALBUM, 0));

        emit songChanged(artist, title, album);
    }
}

// ---------------------------------------------------------------------------------------------- //

void MpdWorker::setUnknownState()
{
    if (m_state != MPD_STATE_UNKNOWN)
    {
        m_state = MPD_STATE_UNKNOWN;
        emit stateChanged(static_cast<MpdState::State>(m_state));
    }
}

// ---------------------------------------------------------------------------------------------- //

void MpdWorker::setUnknownSong()
{
    if (m_songId != UnknownSongId)
    {
        m_songId = UnknownSongId;
        emit songChanged("", "", "");
    }
}

// ---------------------------------------------------------------------------------------------- //

void MpdWorker::setUnknownTime()
{
    if (m_elapsedTime != UnknownTime || m_totalTime != UnknownTime)
    {
        m_elapsedTime = UnknownTime;
        m_totalTime = UnknownTime;
        emit timeChanged("");
    }
}

// ---------------------------------------------------------------------------------------------- //

void MpdWorker::handleError()
{
    disconnectFromDaemon();
    QTimer::singleShot(500ms, this, SLOT(connectToDaemon()));
}

// ---------------------------------------------------------------------------------------------- //

auto MpdWorker::makeTime(unsigned int elapsed, unsigned int total) -> QString
{
    const auto elapsedTime = QTime(0, 0).addSecs(elapsed);
    const auto totalTime = QTime(0, 0).addSecs(total);

    return elapsedTime.toString("m:ss") + " / " + totalTime.toString("m:ss");
}

// ---------------------------------------------------------------------------------------------- //

auto MpdWorker::connectToDaemon(const QString& host, quint16 port,
                                const QString& password) -> mpd_connection*
{
    const std::chrono::milliseconds timeout = 3s;
    mpd_connection* connection = mpd_connection_new(host.toLocal8Bit(), port, timeout.count());

    if (!connection)
        return nullptr;

    const mpd_error error = mpd_connection_get_error(connection);

    if (error == MPD_ERROR_SUCCESS)
    {
        if (password.isEmpty() || mpd_run_password(connection, password.toLocal8Bit()))
            return connection;
    }

    mpd_connection_free(connection);
    return nullptr;
}

// ---------------------------------------------------------------------------------------------- //
