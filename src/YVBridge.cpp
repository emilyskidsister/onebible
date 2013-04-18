/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

#include "YVBridge.h"

#include "YVBookmarks.h"
#include "YVCache.h"
#include "YVDatabase.h"
#include "YVHistory.h"
#include "YVPlayer.h"
#include "YVTranslation.h"

#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QNetworkReply>
#include <QStringList>
#include <QVariant>
#include <QTextCodec>
#include <QtGui/QTextDocument>

#include <bb/system/SystemToast.hpp>
#include <bb/cascades/Invocation>
#include <bb/cascades/InvokeQuery>

YVBridge* YVBridge::s_me = 0;;

YVBridge::YVBridge()
  : m_nm(new QNetworkAccessManager(this))
  , m_lastChapter(-1)
  , m_translation(0)
  , m_reply(0)
  , m_reply_audio(0)
  , m_settings("Nettek", "OneBible")
  , m_launching(1)
  , m_fontSize(settingsFontIdx())
  , m_night(settingsNightMode())
  , m_footnotesEnabled(settingsFootnotes())
  , m_defaultCacheText("Saves current translation for offline reading.<br>Check this for every translation you want to have offline.<br><br>Note: You must leave OneBible open for about 15-20 minutes for a translation to finish downloading.")
  , m_cacheText(m_defaultCacheText)
  , m_cacheGen(0)
  , m_loadingIndic(true)
  { s_me = this;

    connect(&m_loading, SIGNAL(timeout()), this, SLOT(sendPCText()));
    m_loading.setSingleShot(true);

    connect(&m_indic, SIGNAL(timeout()), this, SLOT(setIndicator()));
    m_indic.setSingleShot(true);

    connect(&m_fail, SIGNAL(timeout()), this, SLOT(sendFail()));
    m_fail.setSingleShot(true);

    QObject::connect(m_nm, SIGNAL(finished(QNetworkReply*)), this, SLOT(getText(QNetworkReply*)));
    QObject::connect(m_nm, SIGNAL(finished(QNetworkReply*)), this, SLOT(getAudio(QNetworkReply*)));

    sendPCText();
}

void YVBridge::reqText(QString book, int chap) {
    updateCacheText();
    YVPlayer* player = YVPlayer::instance();
    if (!m_chMap.size() || !book.size()) {
        m_lastBook = book;
        m_lastChapter = chap;
        emit shareLinkChanged();
        m_cache = "";
        QTimer::singleShot(100, this, SLOT(sendSelf()));
        player->setPath("");
        return;
    }
    QString hrbook = book;
    book = m_chMap.value(book, book);

    if (!m_launching) {
        m_settings.setValue("chapter", chap);
        m_settings.setValue("book", m_chMap.value(book, book));
        emit chapterChanged();
        emit bookChanged();
    }
    emit bookmarkedChanged();

    if (m_reply && book == m_lastBook && chap == m_lastChapter) {
        // It's on it's way
        return;
    }
    if (m_reply) m_reply->deleteLater();
    m_reply = 0;

    if (m_chMap.value(m_lastBook, m_lastBook) == m_chMap.value(book, book) && m_lastChapter == chap && m_cache.size()) {
        sendPCText();
        QTimer::singleShot(150, this, SLOT(sendCache()));
    } else {
    	if (!m_launching || m_settings.value("firstRun", true).toBool()) { // don't add the same thing twice!
            YVHistory::instance()->append(bookName(), chap, m_translation);
    	}
        if (m_reply_audio) m_reply_audio->deleteLater();
        player->setPath("");
        m_reply_audio = 0;
        m_cache = "";
        m_lastBook = book;
        m_lastChapter = chap;
        emit shareLinkChanged();

        m_reply_audio = m_nm->get(QNetworkRequest(QUrl::fromPercentEncoding(QString("https://www.youversion.com/bible/"+QString::number(yvVersionIds[m_translation])+"/"+book+"."+QString::number(chap)+".html").toLatin1())));
        if (YVCache::hasCache(yvVersionAbb[m_translation])) {
            int i = -1;
            for (int j = 0; yvGetBookAbbrev(m_translation, j) != "zzzInvalid"; ++j) {
                if (yvGetBookAbbrev(m_translation, j).toUpper() == book.toUpper()) {
                    i = j;
                }
            }
            if (i == -1) {
                m_cache = "Your off-line cache is incomplete. Try restarting OneBible or turning off-line mode off, then on again.";
            } else {
                m_cache = YVCache::getCache(yvVersionAbb[m_translation], chap, i);
            }
            QTimer::singleShot(150, this, SLOT(sendCache()));
        } else {
            m_reply = m_nm->get(QNetworkRequest(QUrl::fromPercentEncoding(QString("https://www.youversion.com/bible/"+QString::number(yvVersionIds[m_translation])+"/"+book+"."+QString::number(chap)+".json").toLatin1())));
            m_loading.start(150);
            m_indic.start(600);
            m_fail.start(4200);
        }
    }
}

QString YVBridge::getBookAbbrev() {
     return m_chMap.value(m_lastBook, m_lastBook);
}

static QString footnote(int i) {
    QString ret;
    if(i - 'a' + 1 < 'z') i += ('z'-'a'+1);
    do {
        i -= 1;
        ret = QString(i%('z'-'a'+1) + 'a') + ret;
    } while ((i = i / ('z'-'a'+1)));
    return ret;
}

QString strip(QString a) {
    QString striped = a;
    while(striped.contains("<")) {
        int i = striped.indexOf("<");
        striped.remove(i, striped.indexOf(">", i) - i);
    }
    return striped;
}

void YVBridge::getText(QNetworkReply *reply) {
    if (!reply->url().toString().contains("json"))
        return;
    m_loading.stop();
    m_indic.stop();
    m_fail.stop();
    m_footnotes.clear();

	m_loadingIndic = false;
	emit loadingChanged();

    if(reply->error() == QNetworkReply::NoError) {
        getText(m_cache = reply->readAll());
    } else {
        m_cache = "";
        m_displayedContent = "<html>Could not load chapter. Check your Internet connection and make sure this chapter is in this translation.<br/><br/>Off-line mode will return in a future release.</html>";
        emit displayedContentChanged();
    }

    reply->deleteLater();
    m_reply = 0;
}

void YVBridge::getText(QString t) {
    // Ideally, we would have the CSS file used in the other YouVersion ports...
    // Sadly, we don't, so we have to go lot's of hacks


	m_loadingIndic = false;
	emit loadingChanged();
    emit shareLinkChanged();
    // hack for NIV footnotes
    t.replace("</span><span class=\\\"fqa\\\">", "");
    t.replace("</span><span class=\\\"ft\\\">", "");

    t.replace("&#", "CL________X"); // hack to allow html non-latin characters while...
    t.replace("#", "");             // ... getting rid of random pound signs
    t.replace("CL________X", "&#");
    t.remove(0, t.indexOf("\"content\":\"")+11);
    t.remove(0,t.indexOf("<div class=\\\"label\\\">"));
    t.remove(0,t.indexOf("</div>")+6);

    // FOOTNOTES

    int TN = 0;

    while(t.contains("<span class=\\\"ft\\\">")) {
        int i = t.indexOf("<span class=\\\"ft\\\">");
        QString n = t;
        n.remove(0, i);
        n.truncate(n.indexOf("</span"));
        m_footnotes.push_back(n);
        t.remove(i, t.indexOf("</span>", i) - i);
        ++TN;

        QList<int> versionsWithBannedFootnotes;
        versionsWithBannedFootnotes.push_back(68);
        versionsWithBannedFootnotes.push_back(69);
        versionsWithBannedFootnotes.push_back(296);
        versionsWithBannedFootnotes.push_back(294);
        versionsWithBannedFootnotes.push_back(303);
        versionsWithBannedFootnotes.push_back(392);
        versionsWithBannedFootnotes.push_back(416);
        versionsWithBannedFootnotes.push_back(431);

        if ((1||m_footnotesEnabled) && !(versionsWithBannedFootnotes.contains(yvVersionIds[m_translation]))) {
            QString fnStart = "<a style=\"text-decoration: none; color: grey;\" href='" + footnote(TN)+"'> ";
            t.insert(qMax(0,i), fnStart);
            i += fnStart.size();
            QString fnStyle;
            	fnStyle = "<span style='text-decoration:underline'>"+footnote(TN)+"</span>";
            t.insert(i, fnStyle);
            t.insert(qMin(i + fnStyle.size(), t.size()), " </a>");
        }
    }

    QString exorcismOfTheLetters[] = {
        "f",
        "x",
        "END"
    };

    int i = -1;
    while(exorcismOfTheLetters[++i] != "END") {
        while(t.contains("<span class=\\\""+exorcismOfTheLetters[i])) {
            t.remove(t.indexOf("<span class=\\\""+exorcismOfTheLetters[i]), t.indexOf("</span", t.indexOf("<span class=\\\""+exorcismOfTheLetters[i])+1) - t.indexOf("<span class=\\\""+exorcismOfTheLetters[i]) + 7);
        }
    }


    // VERSES

    TN = 0;
    while(t.contains("<span class=\\\"label\\\">")) {
        int i = t.indexOf("<span class=\\\"label\\\">");
        bool ok = false;
        for(int v = 1; v < 5; ++v) {
            if (t[i+21+v].toLatin1()>='0' && t[i+21+v].toLatin1()<='9') ok = true;
        }
        t.remove(i, t.indexOf("</span>",i) - i);
        if (ok) t.insert(i, "<b> "+ QString::number(++TN) + "</b> "); // a breaking space is used on purpose.
    }
    t.replace("</div>","</div><br/>");
    t.truncate(t.indexOf("\\n"));
    t.replace("\\\"","\"");
    t.replace("<span class=\"content\">", "");
    t.replace("</span>", "");

    // TITLES

    int iStart = 0;
    while ((iStart = t.indexOf("<span class=\"heading\">", iStart)) != -1) {
    	t.replace(t.indexOf("</div>", iStart), 6, "</p></div>");
    	iStart++;
    }
    t.replace("<span class=\"heading\">", "<p style='font-size: "+QString::number(double(m_fontSize)*(25.0/21.0)/5.0)+"; font-weight: bold;'>");

    while(t.contains("<span")) {
        int i = t.indexOf("<span");
        t.remove(i,t.indexOf(">",i)-i+1);
    }
    t = "<div style=' text-align: justify'>" + t;

    t.replace("<div class=\"p\"> ", "<div class=\"p\">");

    QStringList paragraphs = t.split("<br>");

    // merge small paragraphs.
    bool merging = false;
    for (int i = 0; i < paragraphs.size() - 1; ++i) {
        QString striped = strip(paragraphs[i]);
        if (striped.size() < 200) {
            if (merging) {
                paragraphs[i-1] = paragraphs[i-1] + paragraphs[i];
                paragraphs.removeAt(i--);
            }
            merging = true;
        } else merging = false;
    }

    if (paragraphs.size()) {
        m_displayedContent = "<html>" + paragraphs.join("<br/><br/>") + "</div></html>";

        while (m_displayedContent.contains(" <b"))
        	m_displayedContent.replace(" <b", "<b");
        while (m_displayedContent.contains(" </b"))
            m_displayedContent.replace(" </b", "</b");
        emit displayedContentChanged();
    }
    return;
}

QString YVBridge::bookName() {
	for(int i = 0; i < static_cast<TranslationNode*>(YVTranslation::namesSt()[m_translation])->books().size(); ++i) {
		if (static_cast<BookNode*>(
			static_cast<TranslationNode*>(YVTranslation::namesSt()[m_translation])->books()[i]
		   )->abbrev().toUpper() == settingsBook().toUpper()) {

			return static_cast<BookNode*>(
					static_cast<TranslationNode*>(YVTranslation::namesSt()[m_translation])->books()[i]
				)->book();
		}
	}
	return "invalid";
}

void YVBridge::sendPCText() {
   m_displayedContent = getPlaceholder();
   emit displayedContentChanged();
}

void YVBridge::sendFail() {
    m_displayedContent = "Still loading...";
    emit displayedContentChanged();
    reqText(m_lastBook, m_lastChapter);
}

void YVBridge::setIndicator() {
    m_loadingIndic = true;
    emit loadingChanged();
}

void YVBridge::setFootnotesEnabled(bool b) {
    if (!m_launching) {
        m_settings.setValue("footnotes", b);
    }
    if (m_footnotesEnabled == b) {
        emit feChanged();
        return;
    }
    m_footnotesEnabled = b;
    reqText(m_lastBook, m_lastChapter);

    emit feChanged();
}

void YVBridge::setTranslation(int idx, bool request) {
    if (!m_launching) {
        m_settings.setValue("translation", idx);
    }

    m_translation = idx;
    int i = -1;
    m_chMap.clear();
    while (QString::fromUtf8(yvGetBookName(idx, ++i).toLatin1().data()) != "zzzInvalid") {
    	if (yvGetBookAbbrev(idx,i) == m_lastBook)
    		m_lastBook = QString::fromUtf8(yvGetBookName(idx, i).toLatin1().data());
        m_chMap.insert(QString::fromUtf8(yvGetBookName(idx, i).toLatin1().data()), yvGetBookAbbrev(idx,i));
    }
    m_cache = "";
    emit translationChanged();
    if(request)
    	reqText(m_lastBook, m_lastChapter);
}

void YVBridge::sendFootnote(QString id) {
    if (id == "copyright") {
        openCopyrightInfo();
        return;
    }
    int idx = 0;
    for (int i = 0; i < 500; ++i) {
        if (footnote(i)==id) {
            idx = i;
            break;
        }
    }
    idx -= 1;
    QString footnote;
    if (idx == 499 || m_footnotes.size() < idx) {
        footnote = "Could not find the requested footnote.";
    } else {
        footnote = m_footnotes[idx];
        footnote.replace(QRegExp("<[^>]*>"), "");
    }

    bb::system::SystemToast *toast = new bb::system::SystemToast(this);

    QTextDocument text;
    text.setHtml(footnote);
    QString plain = text.toPlainText();

    toast->setBody(plain);
    toast->button()->setLabel("Ok");
    toast->show();
}

void YVBridge::getAudio(QNetworkReply* reply) {
    if (!reply->url().toString().contains("html"))
        return;
    QString t = reply->readAll();
    if (!m_reply_audio)
        return;

    YVPlayer* player = YVPlayer::instance();
    if (t.contains("mp3")) {
        QString audio = t;
        audio.truncate(audio.indexOf("mp3")+3);
        audio.remove(0, audio.lastIndexOf("src='")+5);
        audio = "http:" + audio;
        player->setPath(audio);
    } else {
        player->setPath("");
    }

    m_reply_audio->deleteLater();
    m_reply_audio = 0;
}

void YVBridge::openCopyrightInfo() {
    QDesktopServices::openUrl(QUrl("https://www.youversion.com/versions/"+ QString::number(yvVersionIds[m_translation])));
}

int YVBridge::settingsTranslation() {
    return m_settings.value("translation", 26).toInt()%406; // "% 406" to workaround a bug in version 1
}

QString YVBridge::settingsBook() {
    return m_settings.value("book", "jhn").toString();
}

int YVBridge::settingsChapter() {
    return m_settings.value("chapter", 3).toInt();
}

int YVBridge::settingsFontIdx() {
    return m_settings.value("font", 38).toInt();
}

bool YVBridge::settingsFootnotes() {
    return m_settings.value("footnotes", 0).toBool();
}

bool YVBridge::settingsNightMode() {
    return m_settings.value("night", 0).toBool();
}

bool YVBridge::cached() {
    return YVCache::hasCache(yvVersionAbb[m_translation]);
}

void YVBridge::setCached(bool t) {
    if (t) {
        setCacheStatus("Beginning download...");
        m_cacheGen = new YVCache(m_translation, yvVersionAbb[m_translation]);
        connect(m_cacheGen, SIGNAL(status(QString)), this, SLOT(setCacheStatus(QString)));
        connect(m_cacheGen, SIGNAL(done()), this, SLOT(onCacheComplete()));
    } else {
        QStringList list = QDir(qgetenv("HOME")).entryList(QStringList("cache_"+ yvVersionAbb[m_translation].toUpper() + "*"));

        QSettings("Nettek", "OneBible").setValue("Cached_" + yvVersionAbb[m_translation].toUpper(), false);

        QDir d(qgetenv("HOME"));
        while(list.size())
            d.remove(list.takeFirst());

        updateCacheText();

    }
}

void YVBridge::setCacheStatus(QString s) {
    m_cacheText = s;
    emit cacheTextChanged();
}

void YVBridge::onCacheComplete() {
    m_cacheGen = 0;
    updateCacheText();
}

void YVBridge::updateCacheText() {
    if (m_cacheGen)
        return;

    if (!cached()) {
        m_cacheText = m_defaultCacheText;
    } else {
        m_cacheText = "Tap above to remove this translation from offline use.";
    }
    emit cacheTextChanged();
    emit cachedChanged();
}

void YVBridge::shareLink() {
	QString uri = QString("https://www.youversion.com/bible/"+QString::number(yvVersionIds[m_translation])+"/"+m_lastBook+"."+QString::number(m_lastChapter)+".html").toLatin1();
	using bb::cascades::Invocation;
	Invocation* i = Invocation::create(bb::cascades::InvokeQuery::create().parent(this).uri(uri));
	connect(i, SIGNAL(armed()), this, SLOT(onArmed()));
	connect(i, SIGNAL(finished()), i, SLOT(deleteLater()));
}

bool YVBridge::settingsFirst() {
    bool ret = m_settings.value("firstRun", true).toBool();
    m_settings.setValue("firstRun", false);
    return ret;
}

bool YVBridge::settingsAudioFirst() {
    bool ret = m_settings.value("audioFirst", true).toBool();
    m_settings.setValue("audioFirst", false);
    return ret;
}

void YVBridge::onArmed() {
	static_cast<bb::cascades::Invocation*>(sender())->trigger("bb.action.SHARE");
}

void YVBridge::bookmark() {
	YVBookmarks* bk = YVBookmarks::instance();

	for (int i = 0; i < bk->books().size(); ++i) {
        if (bk->books()[i] == bookName() && bk->chapters()[i] == settingsChapter() /*&& bk->trs()[i] == m_translation*/) {
			bk->m_books.removeAt(i);
			bk->m_chapters.removeAt(i);
			bk->m_trs.removeAt(i);
			bk->save();
			emit bookmarkedChanged();
			return;
		}

	}
    YVBookmarks::instance()->append(bookName(), settingsChapter(), m_translation);
	emit bookmarkedChanged();
}

bool YVBridge::bookmarked() {
	YVBookmarks* bk = YVBookmarks::instance();
	for (int i = 0; i < bk->books().size(); ++i) {
        if (bk->books()[i] == bookName() && bk->chapters()[i] == settingsChapter() /*&& bk->trs()[i] == m_translation*/)
			return true;
	}
	return false;
}
