/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

#include "YVCache.h"

YVCache::YVCache(int idx, QString translation)
  : s_idx(idx)
  , s_translation(0)
  , s_bookIdx(-1)
  , s_chapIdx(-1)
  , m_nm(new QNetworkAccessManager(this))
  , m_reply(0)
  , m_settings("Nettek", "OneBible")
  { translation = translation.toUpper();
    QObjectList t = YVTranslation::namesSt();
    for (int i = 0; i < t.size(); ++i) {

        TranslationNode* node = dynamic_cast<TranslationNode*>(t[i]);
        Q_ASSERT(node);
        if (node->abbrev() == translation) {
            s_translation = node;
            break;
        }
    }

    if (!s_translation) {
        QTimer::singleShot(0, this, SLOT(emitFail()));
        return;
    }

    QTimer::singleShot(0, this, SLOT(getNextBook()));
}

void YVCache::emitFail() {
    emit status("Could not find the requested translation. Restart the app and try again.");
}

void YVCache::getNextBook() {
    ++s_bookIdx;
    s_chapIdx = 0;  // NOTE: First index is 1
    if (s_bookIdx >= s_translation->books().size()) {
        m_settings.setValue("Cached_" + s_translation->abbrev(), true);
        emit status("Done");
        emit done();
        deleteLater();

        return;

    }
    getNextChapter();
}

void YVCache::getNextChapter() {
    ++s_chapIdx;
    if (s_chapIdx > qobject_cast<BookNode*>(s_translation->books()[s_bookIdx])->chapters()) { // recall that it's 1-indexed
        getNextBook();

        return;

    }

    emit status("Caching " + QString::fromUtf8(yvGetBookName(s_idx, s_bookIdx).toLatin1().data()) + " Chapter " +QString::number(s_chapIdx) + "...");
    m_reply = m_nm->get(QNetworkRequest(QUrl::fromPercentEncoding(QString("https://www.youversion.com/bible/"+QString::number(yvVersionIds[s_translation->idx()])+"/"+yvGetBookAbbrev(s_idx, s_bookIdx)+"."+QString::number(s_chapIdx)+".json").toLatin1())));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(failure()));
    connect(m_reply, SIGNAL(finished()), this, SLOT(getChapter()));
}

void YVCache::failure() {
    // Don't give a scary warning - people will complain!
    --s_chapIdx;
    QTimer::singleShot(200, this, SLOT(getNextChapter()));
}

void YVCache::getChapter() {
    QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
    if (reply->error() != QNetworkReply::NoError)
        return;   //dealt with in failure()

    QByteArray ba = reply->readAll();
    delete reply;
    m_reply = 0;

    if (!ba.contains("\"human\"")) {
        failure();
        return;
    }

    QString t = "cache_"+ (s_translation->abbrev() + QString::number(s_chapIdx) + "_" + QString::number(s_bookIdx));
    QFile f(qgetenv("HOME") + "/" + t);
    f.open(QFile::WriteOnly);
    f.write(ba);
    f.close();

    QTimer::singleShot(500, this, SLOT(getNextChapter()));
}

bool YVCache::hasCache(QString translation) {
    translation = translation.toUpper();
    return QSettings("Nettek", "OneBible").value("Cached_" + translation, false).toBool();
}

QString YVCache::getCache(QString translation, int chap, int book) {
    translation = translation.toUpper();
    QString t = "cache_"+ (translation + QString::number(chap) + "_" + QString::number(book));
    QFile f(qgetenv("HOME") + "/" + t);
    f.open(QFile::ReadOnly);
    return f.readAll();
}
