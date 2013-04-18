/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <QtCore/QVariantList>

#ifndef YVBOOKMARKS_H
#define YVBOOKMARKS_H

class YVBridge;

class YVBookmarks : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList books READ books NOTIFY updated);
    Q_PROPERTY(QVariantList chapters READ chapters NOTIFY updated);
    Q_PROPERTY(QVariantList trs READ trs NOTIFY updated);
    friend class YVBridge;
    explicit YVBookmarks();

public:
    static YVBookmarks* instance();
	virtual ~YVBookmarks();

	Q_INVOKABLE void activate(int i);
    Q_INVOKABLE void append(QString book, int chapter, int tr);
    Q_INVOKABLE QStringList books() { return m_books; }
    Q_INVOKABLE QVariantList chapters() { return m_chapters; }
    Q_INVOKABLE QVariantList trs() { return m_trs; }

    Q_SLOT void clearBookmarks();
    Q_SLOT void save();

signals:
	void updated();

private:
    QSettings m_bookmarks;
    QStringList m_books;
    QVariantList m_chapters;
    QVariantList m_trs;

    static YVBookmarks* s_me;
};

#endif//YVBOOKMARKS_H
