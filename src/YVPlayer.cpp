/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

#include "YVPlayer.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <bps/audiomixer.h>
#include <bps/bps.h>
#include <bps/dialog.h>
#include <bps/navigator.h>
#include <mm/renderer.h>
#include <screen/screen.h>

YVPlayer* YVPlayer::m_me = 0;

#define MSG_SIZE 1024
static char msg[MSG_SIZE];

#if ! defined(DEFAULT_AUDIO_OUT)
    #define DEFAULT_AUDIO_OUT "audio:default"
#endif

YVPlayer::YVPlayer()
  : m_playing(false)
  , m_nm(new QNetworkAccessManager)
  , m_ctxt(NULL)
  , m_reply(0)
  , m_pos(-1)
  {
    m_me = this;
    connect(m_nm, SIGNAL(finished(QNetworkReply*)), this, SLOT(saveFile(QNetworkReply*)));
    s_previousEventFilter = QAbstractEventDispatcher::instance()->setEventFilter(s_eventFilter);
}

bool YVPlayer::s_eventFilter(void* message) {
    return m_me->nativeEventFilter(message);
}

void YVPlayer::playCurrent() {
    const char *mmrname = NULL;
    const char *ctxtname = "onebible";
    const char *audioout = NULL;
    const char *inputtype = "track";
    char inputurl[PATH_MAX];
    int rc;
    mmr_connection_t *connection = NULL;

    rc = snprintf(inputurl, PATH_MAX, "file://%s/%s", getenv("HOME"), m_t.toLatin1().data());

    mode_t mode = S_IRUSR | S_IXUSR;
    int audio_oid; // output ID
    strm_dict_t *aoparams = NULL; // output parameters

    audioout = DEFAULT_AUDIO_OUT;

    // TODO: Better error reporting
    if ( ( connection = mmr_connect( mmrname ) ) == NULL ) {
        snprintf(msg, MSG_SIZE, "mmr_connect: %s", strerror(errno));
    } else if ( ( m_ctxt = mmr_context_create( connection, ctxtname, 0, mode ) ) == NULL ) {
        snprintf(msg, MSG_SIZE, "%s: %s", ctxtname, strerror(errno));
    } else if ( audioout && ( audio_oid = mmr_output_attach( m_ctxt, audioout, "audio" ) ) < 0 ) {
    } else if ( aoparams && mmr_output_parameters( m_ctxt, audio_oid, aoparams ) ) {
    } else if ( mmr_input_attach( m_ctxt, inputurl, inputtype ) < 0 ) {
    } else if ( mmr_play( m_ctxt ) < 0 ) {
    } else {
    }

    mmrenderer_request_events(ctxtname, 0, 0); // bps
    return;
}

bool YVPlayer::nativeEventFilter(void *message) {
    bps_event_t * const event = static_cast<bps_event_t *>(message);

    unsigned int code = bps_event_get_code(event);

    switch (code) {

    case MMRENDERER_STATUS_UPDATE :
        m_pos = QString(mmrenderer_event_get_position(event)).toInt();
        break;
    case MMRENDERER_STATE_CHANGE :
        if (mmrenderer_event_get_state(event) == MMR_STOPPED) {
            if (m_playing && m_ctxt) {
                m_pos = -1; mmr_stop(m_ctxt); mmr_context_destroy(m_ctxt); m_ctxt = 0;
            }
            m_playing = false;
            emit playingChanged();
        }
        break;
    }

    if (s_previousEventFilter)
        return s_previousEventFilter(message);

    return false;
}

void YVPlayer::setPath(QString url) {
    if (m_ctxt) {
        m_pos = -1;
        mmr_stop(m_ctxt);
        mmr_context_destroy(m_ctxt);
        m_ctxt = 0;
    }
    if (m_reply) {
        m_reply->deleteLater();
        m_reply = 0;
    }
    m_t = "";
    m_url = url;
    emit playabilityChanged();
    setPlaying(false);
}

void YVPlayer::setPlaying(bool p) {
    m_playing = p;
    emit playingChanged();
    if (!p) {
        if (m_reply) {
            m_reply->deleteLater();
            m_reply = 0;
        } else if (m_ctxt) {
            mmr_stop(m_ctxt);
        }
        return;
    }

    if (!m_t.size()) {
        m_reply = m_nm->get(QNetworkRequest(QUrl::fromPercentEncoding(m_url.toLatin1())));
    } else {
        if (m_ctxt) {
            if (m_pos == -1) m_pos = 0;
            mmr_seek(m_ctxt, QString::number(m_pos ).toLatin1().data());
            mmr_play(m_ctxt);
        } else {
            playCurrent();
        }
    }
}

void YVPlayer::saveFile(QNetworkReply* reply) {
    QString t = "tmp_"+QString::number(qHash(reply->url())) + ".mp3";
    m_t = t;
    QFile f(qgetenv("HOME") + "/" + t);
    f.open(QFile::WriteOnly);
    f.write(reply->readAll());
    m_reply = 0;
    reply->deleteLater();
    setPlaying(true);
}
