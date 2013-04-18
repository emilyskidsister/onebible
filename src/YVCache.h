/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

#ifndef YVCACHE_H
#define YVCACHE_H

#include <QObject>
#include <QNetworkReply>
#include <QFile>
#include <QSettings>
#include <QTimer>
#include "YVTranslation.h"

class YVCache : public QObject
{
    Q_OBJECT
public:
    YVCache(int idx, QString translation);

    Q_SLOT void emitFail();
    Q_SLOT void getNextBook();
    Q_SLOT void getNextChapter();
    Q_SLOT void failure();
    Q_SLOT void getChapter();

    static bool hasCache(QString translation);
    static QString getCache(QString translation, int chap, int book);

signals:
    void status(QString);
    void done();

private:
    int s_idx;
    TranslationNode* s_translation;
    int s_bookIdx, s_chapIdx;
    QNetworkAccessManager* m_nm;
    QNetworkReply* m_reply;
    QSettings m_settings;
};

#endif // YVCACHE_H
