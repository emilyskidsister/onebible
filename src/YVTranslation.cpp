/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

#include "YVTranslation.h"

BookNode::BookNode(QString b, QString a, int chapters, int i)
  : m_book(b)
  , m_abbrev(a)
  , m_chapters(chapters)
  , m_id(i)
  { m_book = QString::fromUtf8(m_book.toLatin1().data());
}

QString BookNode::book() { return m_book; }
QString BookNode::abbrev() { return m_abbrev; }
int BookNode::chapters() { return m_chapters; }
int BookNode::id() { return m_id; }

QString TranslationNode::nainstance() { return m_name; }
QString TranslationNode::abbrev() { return m_abbrev; }
int TranslationNode::id() { return m_id; }
int TranslationNode::idx() { return m_idx; }

QObjectList TranslationNode::books() { // lazy evaluation.
    if (m_books.size()) return m_books;
    int i = -1;
    while (yvGetBookName(m_idx, ++i) != "zzzInvalid" ) {
        m_books.push_back(new BookNode(yvGetBookName(m_idx, i), yvGetBookAbbrev(m_idx, i), yvGetBookChapCount(m_idx,i), i));
    }
    return m_books;
}

QObjectList YVTranslation::cache;

YVTranslation::YVTranslation()
{
}

QObjectList YVTranslation::names() {
    return namesSt();
}

QObjectList YVTranslation::namesSt() {
    if (cache.size()) return cache;

    for (int i = 0; i < YV_VERSION_COUNT; ++i) {
        cache.push_back(new TranslationNode(QString::fromUtf8(yvVersionNames[i].toAscii().data()), yvVersionAbb[i].toUpper(), i, yvVersionIds[i]));
    }
    return cache;
}
