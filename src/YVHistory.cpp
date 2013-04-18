/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

#include "YVHistory.h"
#include "YVBridge.h"

YVHistory* YVHistory::s_me = 0;

YVHistory::YVHistory()
    : m_history("Nettek", "OneBible History")
    , m_books(m_history.value("books",QStringList()).toStringList())
    , m_chapters(m_history.value("chapters", QVariantList()).toList())
    , m_trs(m_history.value("trs", QVariantList()).toList())
{
}

YVHistory::~YVHistory() {}

void YVHistory::activate(int i) {
    YVBridge::instance()->setTranslation(m_trs[i].toInt(), false);
    YVBridge::instance()->reqText(m_books[i], m_chapters[i].toInt());
}

void YVHistory::append(QString book, int chapter, int tr) {
    if (m_books.size() && m_books.front() == book && m_chapters.front().toInt() == chapter && m_trs.front().toInt() == tr)
        return;

    m_books.push_front(book);
    m_chapters.push_front(chapter);
    m_trs.push_front(tr);
    if (m_books.size() >= 100) {
        m_books.pop_back();
        m_chapters.pop_back();
        m_trs.pop_back();
    }

    // FIXME: Can be made more efficient
    m_history.setValue("books", m_books);
    m_history.setValue("chapters", m_chapters);
    m_history.setValue("trs", m_trs);
    emit updated();
}

void YVHistory::clearHistory() {
    m_books.clear();
    m_chapters.clear();
    m_trs.clear();

    m_history.setValue("books", m_books);
    m_history.setValue("chapters", m_chapters);
    m_history.setValue("trs", m_trs);
    emit updated();
}
