/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

#ifndef YVBridge_H
#define YVBridge_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QMap>
#include <QVariant>
#include <QTimer>
#include <QStringList>
#include <QDebug>
#include <QSettings>

class YVCache;

class YVMap : public QMap<QString, QString>
{
public:
    void insertRev(QString a, QString b) { insert(b, a); }
};

class YVBridge : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)
    Q_PROPERTY(bool night READ night WRITE setNight NOTIFY nightChanged)
    Q_PROPERTY(bool footnotesEnabled READ footnotesEnabled WRITE setFootnotesEnabled NOTIFY feChanged)
    Q_PROPERTY(bool launching READ launching WRITE setLaunching NOTIFY launchingChanged)
    Q_PROPERTY(bool cached READ cached WRITE setCached NOTIFY cachedChanged)
    Q_PROPERTY(QString cacheText READ cacheText NOTIFY cacheTextChanged)
    Q_PROPERTY(bool caching READ caching NOTIFY cacheTextChanged)
    Q_PROPERTY(QString content READ displayedContent NOTIFY displayedContentChanged)
    Q_PROPERTY(QString bookName READ bookName NOTIFY bookChanged)
    Q_PROPERTY(int chapter READ settingsChapter NOTIFY chapterChanged)
    Q_PROPERTY(int translation READ settingsTranslation NOTIFY translationChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(bool bookmarked READ bookmarked NOTIFY bookmarkedChanged)

public:
    YVBridge();
    static YVBridge* instance() { return s_me; }

    Q_INVOKABLE QString getPlaceholder() { return "  "; }
    Q_INVOKABLE QString getBookAbbrev();
    Q_INVOKABLE int getChapter() { return m_lastChapter; }
    Q_INVOKABLE int settingsTranslation();
    Q_INVOKABLE QString settingsBook();
    Q_INVOKABLE QString bookName();
    Q_INVOKABLE int settingsChapter();
    Q_INVOKABLE int settingsFontIdx();
    Q_INVOKABLE bool settingsFootnotes();
    Q_INVOKABLE bool settingsNightMode();
    Q_INVOKABLE bool night() { return m_night; }
    Q_INVOKABLE bool launching() { return m_launching; }
    Q_INVOKABLE bool footnotesEnabled() { return m_footnotesEnabled; }
    Q_INVOKABLE QString displayedContent() { return m_displayedContent; }
    Q_INVOKABLE bool cached();
    Q_INVOKABLE void setCached(bool);
    Q_INVOKABLE QString cacheText() { return m_cacheText; }
    Q_INVOKABLE bool caching() { return m_cacheGen; }
    Q_INVOKABLE bool loading() { return m_loadingIndic; }
    Q_INVOKABLE void shareLink();
    Q_INVOKABLE bool settingsFirst();
    Q_INVOKABLE bool settingsAudioFirst();
    Q_INVOKABLE bool bookmarked();
    Q_INVOKABLE int fontSize() { return m_fontSize; }

    Q_SLOT void setFontSize(int a) { if (!m_launching) m_settings.setValue("font", a); m_fontSize = a; reqText(m_lastBook, m_lastChapter); emit fontSizeChanged(a); }
    Q_SLOT void setLaunching(bool a) { m_launching = a; emit launchingChanged(); }
    Q_SLOT void setNight(bool a) { if (!m_launching) m_settings.setValue("night", a); m_night= a; emit nightChanged(); }
    Q_SLOT void reqText(QString book, int chap);
    Q_SLOT void getText(QNetworkReply* r);
    Q_SLOT void getText(QString data);
    Q_SLOT void getAudio(QNetworkReply *r);
    Q_SLOT void sendSelf() { reqText(m_lastBook, m_lastChapter); }
    Q_SLOT void sendCache() { getText(m_cache); }
    Q_SLOT void sendFootnote(QString id);
    Q_SLOT void setNCText(QString x) { m_nc = x; }
    Q_SLOT void sendPCText();
    Q_SLOT void setIndicator();
    Q_SLOT void sendFail();
    Q_SLOT void setFootnotesEnabled(bool);
    Q_SLOT void setTranslation(int idx, bool request = true);
    Q_SLOT void openCopyrightInfo();
    Q_SLOT void setCacheStatus(QString);
    Q_SLOT void onArmed();
    Q_SLOT void bookmark();
    Q_SLOT void onCacheComplete();
    Q_SLOT void updateCacheText();

signals:
    void fontSizeChanged(int i);
    void launchingChanged();
    void nightChanged();
    void feChanged();
    void cachedChanged();
    void cacheTextChanged();
    void displayedContentChanged();
    void chapterChanged();
    void bookChanged();
    void loadingChanged();
    void translationChanged();
    void shareLinkChanged();
    void bookmarkedChanged();

private:
    QNetworkAccessManager* m_nm;
    QString m_nc;
    YVMap m_chMap;
    QTimer m_loadinonCacheCompleteg, m_indic, m_fail;
    QString m_lastBook;
    int m_lastChapter;
    QString m_cache;
    QStringList m_footnotes;
    int m_translation;
    QNetworkReply* m_reply;
    QNetworkReply* m_reply_audio;
    QSettings m_settings;
    bool m_launching;
    int m_fontSize;
    bool m_night;
    bool m_footnotesEnabled;
    QString m_defaultCacheText;
    QString m_cacheText;
    YVCache* m_cacheGen;
    QString m_displayedContent;
    QTimer m_loading;
    bool m_loadingIndic;
    static YVBridge* s_me;
};

#endif // YVBridge_H
