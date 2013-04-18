/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

#ifndef YVSEARCH_H
#define YVSEARCH_H

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QObject>
#include <bb/data/JsonDataAccess>

#include "YVTranslation.h"
#include "YVBridge.h"

class YVSearch : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList results READ results NOTIFY resultsChanged)
    Q_PROPERTY(QString request READ request WRITE setRequest)

public:
    explicit YVSearch();
    virtual ~YVSearch();

    Q_INVOKABLE bool activate(int j);
    Q_INVOKABLE QStringList results();
    Q_INVOKABLE void setRequest(QString query);
    Q_INVOKABLE QString request();

    Q_SLOT void process(QNetworkReply* reply);

signals:
	void resultsChanged();

private:
    QNetworkReply* m_reply;
    QNetworkAccessManager* m_manager;
    QStringList m_results;
    QStringList m_book;
    QList<int> m_chapter;
};

#endif//YVSEARCH_H
