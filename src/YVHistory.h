/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <QtCore/QVariantList>

#ifndef YVHISTORY_H
#define YVHISTORY_H

class YVHistory : public QObject
{
	Q_OBJECT
    Q_PROPERTY(QStringList books READ books NOTIFY updated);
    Q_PROPERTY(QVariantList chapters READ chapters NOTIFY updated);
    Q_PROPERTY(QVariantList trs READ trs NOTIFY updated);

	YVHistory();

public:
	static YVHistory* instance() { return s_me = (s_me ? s_me : new YVHistory); }
	virtual ~YVHistory();

	Q_INVOKABLE void activate(int i);
    Q_INVOKABLE void append(QString book, int chapter, int tr);
	Q_INVOKABLE QStringList books() { return m_books; }
	Q_INVOKABLE QVariantList chapters() { return m_chapters; }
    Q_INVOKABLE QVariantList trs() { return m_trs; }

    Q_SLOT void clearHistory();

signals:
	void updated();

private:
    QSettings m_history;
    QStringList m_books;
    QVariantList m_chapters;
    QVariantList m_trs;
    static YVHistory* s_me;
};

#endif//YVHISTORY_H
