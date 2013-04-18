/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

#include "YVBookmarks.h"
#include "YVBridge.h"

YVBookmarks* YVBookmarks::s_me = 0;

YVBookmarks::YVBookmarks()
    : m_bookmarks("Nettek", "OneBible Bookmarks")
    , m_books(m_bookmarks.value("books",QStringList()).toStringList())
    , m_chapters(m_bookmarks.value("chapters", QVariantList()).toList())
    , m_trs(m_bookmarks.value("trs", QVariantList()).toList())
{
}

YVBookmarks::~YVBookmarks() {}

YVBookmarks* YVBookmarks::instance() {
    return s_me = (s_me ? s_me : new YVBookmarks);
}

void YVBookmarks::activate(int i) {
    YVBridge::instance()->reqText(m_books[i], m_chapters[i].toInt());
}

void YVBookmarks::append(QString book, int chapter, int tr) {
    if (m_books.size() && m_books.front() == book && m_chapters.front().toInt() == chapter && m_trs.front().toInt() == tr)
        return;

    m_books.push_front(book);
    m_chapters.push_front(chapter);
    m_trs.push_front(tr);

    // FIXME: O(n^2) case possible on startup
    save();
    emit updated();
}

void YVBookmarks::clearBookmarks() {
    m_books.clear();
    m_chapters.clear();
    m_trs.clear();
    save();
    emit updated();
}

void YVBookmarks::save() {
    m_bookmarks.setValue("books", m_books);
    m_bookmarks.setValue("chapters", m_chapters);
    m_bookmarks.setValue("trs", m_trs);
}
