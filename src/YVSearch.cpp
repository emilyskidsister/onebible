/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

#include "YVSearch.h"

YVSearch::YVSearch()
  : m_reply(0)
  , m_manager(new QNetworkAccessManager)
  { QObject::connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(process(QNetworkReply*)));
}

YVSearch::~YVSearch() {}

void YVSearch::setRequest(QString query) {
    if (m_reply) m_reply->deleteLater();
    m_reply = 0;
    m_results.clear();
    m_book.clear();
    m_chapter.clear();
    if (query.size()) {
        m_reply = m_manager->get(QNetworkRequest(QUrl::fromPercentEncoding(QString("http://beta.biblegateway.com/api/1/search?q="+ query + "&t=niv,kjv,msg,lbla,bds,lsg&objects=data").toLatin1())));
        m_results.push_back("Searching...");
    }
    emit resultsChanged();
}

QStringList YVSearch::results() {
    return m_results;
}

bool YVSearch::activate(int j) {
    if (!m_book.size())
        return false; // we also use the list for hints.

    int i = -1;
    while (QString::fromUtf8(yvGetBookName(26, ++i).toLatin1().data()) != "zzzInvalid") { // 26 is NIV
        if (yvGetBookName(26,i).toUpper().startsWith(m_book[j].toUpper())) {
            YVBridge::instance()->reqText(yvGetBookAbbrev(26,i), m_chapter[j]);
            return true;
        }
    }
    return false;
}

QString YVSearch::request() {
    return "INVALID_FUNCTION_CALL!";
}

void YVSearch::process(QNetworkReply* reply) {
    using bb::data::JsonDataAccess;
    QByteArray str = reply->readAll();
    JsonDataAccess jda;
    QVariantList list = jda.loadFromBuffer(str).value<QVariantMap>().value("data").toList();;

    m_results.clear();
    QString tst;
    if (str.contains("We're sorry, but something went wrong.")) {
        m_results.push_back("Your query was too common.");
    } else if (str.contains("total_results\":0")) {
        m_results.push_back("No results. Try something else.");
    } else for (int i = 0; i < list.size(); ++i) {
        QString result = list[i].toMap().value("id").toString().replace('.', ' ');
        if (!result.size())
            continue;

        QString tmp = result;
        tmp.remove(0, tmp.indexOf(":") + 1);
        tmp.truncate(tmp.indexOf(" "));
        m_book.push_back(tmp);

        tmp = result;
        tmp.remove(0, tmp.indexOf(" ")+1);
        tmp.truncate(tmp.indexOf(" "));
        m_chapter.push_back(tmp.toInt());

        result[result.lastIndexOf(' ')] = ':';
        QString tr = result;
        tr.truncate(tr.indexOf(':'));
        result.remove(0, result.indexOf(':') + 1);
        m_results.push_back(result + " (" + tr + ")");
    }

    emit resultsChanged();
}
