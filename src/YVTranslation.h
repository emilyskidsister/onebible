/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

#include "YVDatabase.h"

#include <QDebug>
#include <QStringList>
#include <QObject>
#include <QObjectList>
#include <QVariant>
#include <QtGui/QTextDocument>

#ifndef YVTRANSLATION_H
#define YVTRANSLATION_H

class BookNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ book CONSTANT)
    Q_PROPERTY(QString abbrev READ abbrev CONSTANT)
    Q_PROPERTY(int chapters READ chapters CONSTANT)
    Q_PROPERTY(int id READ id CONSTANT)

public:
    BookNode(QString b, QString a, int chapters, int i);

    QString book();
    QString abbrev();
    int chapters();
    int id();

private:
    QString m_book, m_abbrev;
    int m_chapters, m_id;
};

class TranslationNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString abbrev READ abbrev CONSTANT)
    Q_PROPERTY(int idx READ idx CONSTANT)
    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(QObjectList books READ books CONSTANT)

public:
    TranslationNode(QString n,QString a,int idx, int i) : m_name(n), m_abbrev(a), m_idx(idx), m_id(i) {}

    QString nainstance();
    QString abbrev();
    int id();
    int idx();
    QString name() { return m_name; }

    QObjectList books();

private:
    QString m_name, m_abbrev;
    int m_idx, m_id;
    QObjectList m_books;
};


class YVTranslation : public QObject
{
	Q_OBJECT
public:
    YVTranslation();
    static QObjectList cache;

    Q_INVOKABLE QObjectList names();
    static QObjectList namesSt();
};

#endif // YVTRANSLATION_H
