/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

#include "YVApp.h"

#include "YVPlayer.h"
#include "YVBookmarks.h"
#include "YVBridge.h"
#include "YVHistory.h"
#include "YVSearch.h"
#include "YVTranslation.h"

#include <bb/cascades/AbstractPane>
#include <bb/cascades/ActiveTextHandler>
#include <bb/cascades/ActiveTextTriggeredEvent>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>

#include <bb/platform/Notification>
#include <bb/platform/NotificationDialog>
#include <bb/platform/NotificationResult>
#include <bb/platform/NotificationError>

YVApp::YVApp(bb::cascades::Application *app) : QObject(app) {
    using namespace bb::cascades;

	qmlRegisterType<QTimer>("utils", 1, 0, "QTimer");
	qmlRegisterType<YVSearch>("utils", 1, 0, "YVSearch");
    qmlRegisterType<QPropertyAnimation>("bb.cascades", 1, 0, "QPropertyAnimation");
    qmlRegisterType<bb::platform::Notification> ("bb.platform", 1, 0, "Notification");
    qmlRegisterType<bb::platform::NotificationDialog> ("bb.platform", 1, 0, "NotificationDialog");
    qmlRegisterType<ActiveTextHandler>("utils", 1, 0, "ActiveTextHandler");
    qmlRegisterUncreatableType<ActiveTextTriggeredEvent>("utils", 1, 0, "ActiveTextTriggeredEvent", "");

    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    qml->setContextProperty("YVBridge", new YVBridge);
    qml->setContextProperty("YVTranslation", new YVTranslation);
    qml->setContextProperty("YVPlayer", YVPlayer::instance());
    qml->setContextProperty("YVHistory", YVHistory::instance());
    qml->setContextProperty("YVBookmarks", YVBookmarks::instance());

    AbstractPane *root = qml->createRootObject<AbstractPane>();
    app->setScene(root);
}
