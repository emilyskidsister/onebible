/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

#ifndef QNXAUDIOPLAYER_H
#define QNXAUDIOPLAYER_H

#include <QCoreApplication>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QString>
#include <QUrl>

#include <bps/bps.h>
#include <bps/mmrenderer.h>
#include <mm/renderer.h>
#include <qabstracteventdispatcher.h>

class YVPlayer : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool playable READ playable NOTIFY playabilityChanged)
    Q_PROPERTY(bool playing READ playing WRITE setPlaying NOTIFY playingChanged)
public:
    static YVPlayer* instance() { return m_me = (m_me ? m_me : new YVPlayer); }

    Q_INVOKABLE bool playable() { return m_url.size(); }
    Q_INVOKABLE bool playing() { return m_playing; }
    Q_SLOT void setPath(QString url);
    Q_SLOT void setPlaying(bool p);
    Q_SLOT void saveFile(QNetworkReply* reply);

signals:
    void playabilityChanged();
    void playingChanged();

private:
    QString m_url;
    bool m_playing;
    QString m_t;
    QNetworkAccessManager* m_nm;
    mmr_context_t* m_ctxt;
    QNetworkReply* m_reply;
    int m_pos;
    QAbstractEventDispatcher::EventFilter s_previousEventFilter;
    static YVPlayer* m_me;

    YVPlayer();
    static bool s_eventFilter(void* message);
    virtual bool nativeEventFilter(void *message);
    void playCurrent();
};

#endif // QNXAUDIOPLAYER_H
