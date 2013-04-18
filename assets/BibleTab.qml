/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

import bb.cascades 1.0
import bb.system 1.0
import bb.platform 1.0
import utils 1.0

Tab {
    title: qsTr("Bible")
    imageSource: "asset:///images/pbicon.png";

    Page {
        id: tab1
        actionBarVisibility : ChromeVisibility.Overlay;
        actions: [
            ActionItem {
                title: YVPlayer.playing ? qsTr("Pause") : qsTr("Read Aloud");
                imageSource: YVPlayer.playing ? "asset:///images/pause.png" : "asset:///images/play.png";
                ActionBar.placement: ActionBarPlacement.OnBar
                enabled: YVPlayer.playable;
                onTriggered: {
                    if (YVBridge.settingsAudioFirst()) {
                        navigationHintToast.body = "The audio features uses data. To avoid charges, only use this feature on wifi, or if you have a data plan.";
                        navigationHintToast.show();
                    }
                    YVPlayer.playing = !YVPlayer.playing
                }
            },
            ActionItem {
                title: qsTr("Decrease Font Size")
                imageSource: "asset:///images/fontMinus.png";
                onTriggered: {
                    if (YVBridge.fontSize <= 28) {
                        navigationHintToast.body = "Can't decrease font anymore";
                        navigationHintToast.show();
                    } else {
                        YVBridge.fontSize -= 5;
                    }
                }
            },
            ActionItem {
                title: qsTr("Increase Font Size")
                imageSource: "asset:///images/fontPlus.png";
                onTriggered: {
                    if (YVBridge.fontSize > 56) {
                       navigationHintToast.body = "Can't increase font anymore";
                       navigationHintToast.show();
                    } else {
                        YVBridge.fontSize += 5;
                    }
                }
            },
            ActionItem {
                title: qsTr("Toggle Night Mode");
                imageSource: "asset:///images/night.png";
                onTriggered: {
                    YVBridge.night = !YVBridge.night;
                }
            },
            ActionItem {
                title: YVBridge.bookmarked ? "Unmark" : "Bookmark";

                ActionBar.placement: ActionBarPlacement.OnBar
                imageSource: "asset:///images/bookmarks.png";
                onTriggered: {
                    YVBridge.bookmark();
                    bookmarkToast.show();
                }
            },
            ActionItem {
                title: "Share"
                imageSource: "asset:///images/share.png";
                onTriggered: {
                    YVBridge.shareLink();
                }
                ActionBar.placement: ActionBarPlacement.OnBar
            }
        ]
        Container {
            layout: DockLayout {}
            id: ctab1;

            onTouch: {
                if (event.isDown() && event.localY < 100 && event.localX < 600)
                    chapterSheet.createObject().open();

            }

            attachedObjects: [
                ImagePaintDefinition {
                    id: headerPaint;
                    imageSource: "asset:///images/header.amd";
                }
            ]
            background: headerPaint.imagePaint
            opacity: 1.0
            topPadding: -5.0
            Container {
                layout: DockLayout {}

                leftPadding:24;
                topPadding: 18;

                horizontalAlignment: HorizontalAlignment.Left
                verticalAlignment: VerticalAlignment.Top
                minHeight: 110;

                Label {
                    id: nx;
                    property int hWidth : 0;
                    text: (YVBridge.bookName === "invalid") ? "Oops!" : (YVBridge.bookName+" "+YVBridge.chapter);
                    textStyle.textAlign: TextAlign.Center
                    horizontalAlignment: HorizontalAlignment.Left;
                    verticalAlignment: VerticalAlignment.Center
                    textStyle.fontSize: FontSize.PointValue
                    textStyle.fontSizeValue: 10.0
                    attachedObjects: [
                        LayoutUpdateHandler {
                            id: nxHandler;
                            onLayoutFrameChanged: {
                                nx.hWidth = layoutFrame.width;
                            }
                        }
                    ]
                    opacity: 1.0
                }
            }
            Container {
                layout: AbsoluteLayout {}

                leftPadding:24;
                topPadding: 22;

                horizontalAlignment: HorizontalAlignment.Left
                verticalAlignment: VerticalAlignment.Top
                minHeight: 110;

                ImageView {
                    layoutProperties: AbsoluteLayoutProperties {
                        positionX: nx.hWidth + 22;
                        positionY: 23;
                    }
                    imageSource: "asset:///images/down.png"
                    horizontalAlignment: HorizontalAlignment.Left;
                    verticalAlignment: VerticalAlignment.Center
                    opacity: 1.0
                }
            }
            Container {
                layout: DockLayout {}

                rightPadding: 24;
                topPadding: 22;

                horizontalAlignment: HorizontalAlignment.Right
                verticalAlignment: VerticalAlignment.Top
                id: headerTr

                minHeight: 110

                Label {
                    text: YVTranslation.names()[YVBridge.translation].abbrev;
                    textStyle.textAlign: TextAlign.Center
                    textStyle.fontSizeValue: 10.0

                    horizontalAlignment: HorizontalAlignment.Center;
                    verticalAlignment: VerticalAlignment.Center

                    textStyle.fontSize: FontSize.PointValue
                    onTouch: {
                        if (event.isDown())
                            trSheet.createObject().open();
                    }
                }
            }
            SwipeHandler {
                id: mainSH;
                property int dragScrollSpeed: -2 // Lower value gives higher scroll speed
                property int momentumScrollSpeed: -1 // Lower value gives higher scroll speed
                property int xSwipe : 0;
                property bool curtain : false;
                property bool active : false;

                background: YVBridge.night ? Color.Black : Color.White;
                layout: AbsoluteLayout {}
                ImageView {
                    id: leftArrow
                    opacity: (mainSH.active && mainSH.xSwipe > 0) ? 1.0 : 0.0;
                    imageSource: "asset:///images/back.png";
                    layoutProperties: AbsoluteLayoutProperties {
                        positionX: -20;
                        positionY: mainSH.preferredHeight / 2 - leftArrow.preferredHeight / 2;
                    }
                    preferredHeight: 180;
                    minHeight: 180
                    minWidth: 180;
                }
                ImageView {
                    id: rightArrow
                    opacity: (mainSH.active && mainSH.xSwipe < 0) ? 1.0 : 0.0;
                    imageSource: "asset:///images/forward.png";
                    layoutProperties: AbsoluteLayoutProperties {
                        positionX: 768 - rightArrow.minWidth + 20;
                        positionY: mainSH.preferredHeight / 2 - rightArrow.preferredHeight / 2;
                    }
                    preferredHeight: 180;
                    minHeight: 180
                    minWidth: 180;
                }
                ActivityIndicator {
                    layoutProperties: AbsoluteLayoutProperties {
                        positionX: 768/2 - 300/2
                        positionY: 1024/2 - 300/2
                    }
                    preferredWidth: 300;
                    preferredHeight: 300;
                    running: YVBridge.loading;
                }
                ScrollView {
                    Container {
                        id: textCenter;
                        leftPadding: 5;
                        rightPadding: 5;
                        topPadding: 5;
                        bottomPadding: 35;
                        TextArea {
                            textFormat: TextFormat.Html
                            id: scr
                            text: YVBridge.content
                            editable: false;
                            textStyle.color: YVBridge.night ? Color.White : Color.Black
                            textStyle.fontSize: FontSize.PointValue
                            textStyle.fontSizeValue: (1.0*YVBridge.fontSize)/5.0;
                            activeTextHandler: ActiveTextHandler {
                                onTriggered: {
                                    YVBridge.sendFootnote(event.href); // and oh look, a toast.
                                }
                            }
                            opacity: 1.0
                        }
                        opacity: mainSH.curtain ? 0.0 : (1.0 - Math.sqrt(Math.max(0.01, mainSH.xSwipe/720*((mainSH.xSwipe < 0.0) ? -1.0 : 1.0))));
                    }
                    layoutProperties: AbsoluteLayoutProperties {
                        positionX: mainSH.xSwipe;
                        positionY: 0;
                    }
                    attachedObjects: [
                        ImplicitAnimationController {
                            enabled: !mainSH.curtain
                        }
                    ]
                    preferredHeight: 1280 - 110;
                }

                onDragOffsetChanged: {
                    if (dragOffset / dragScrollSpeed > 150 || dragOffset / dragScrollSpeed < -150) {
                        active = true;
                        xSwipe = 150*((dragOffset > 0.0) ? -1.0 : 1.0) + dragOffset / dragScrollSpeed;
                    } else {
                        active = false;
                        xSwipe = 0;
                    }
                }
                onScrollingStart: {
                    xSwipe = 0;
                }
                onDragScrollingEnd: {
                    if (xSwipe > 100) {
                        prev();
                    } else if (xSwipe < -100) {
                        next();
                    } else {
                        active = false;
                        xSwipe = 0;
                    }
                }
                function next() {
                    var bkIdx = 1;
                    var tnstb = YVTranslation.names()[YVBridge.settingsTranslation()].books;
                    var tbk = YVBridge.settingsBook();

                    for (var i = 0; i < tnstb.length; ++i) {
                        if (tnstb[i].abbrev === tbk) {
                            bkIdx = i;
                            break;
                        }
                    }

                    if (YVBridge.settingsChapter() >= YVTranslation.names()[YVBridge.settingsTranslation()].books[bkIdx].chapters) {
                        chapterSheet.createObject().open();
                    } else {
                        YVBridge.reqText(YVTranslation.names()[YVBridge.settingsTranslation()].books[bkIdx].name, YVBridge.settingsChapter()+1);
                    }
                    active = false;
                    xSwipe = -768;
                    reverseTimer.start();
                    swipeTimer.start();
                }

                function prev() {
                    var bkIdx = 1;
                    var tnstb = YVTranslation.names()[YVBridge.settingsTranslation()].books;
                    var tbk = YVBridge.settingsBook();

                    for (var i = 0; i < tnstb.length; ++i) {
                        if (tnstb[i].abbrev === tbk) {
                            bkIdx = i;
                            break;
                        }
                    }

                    if (YVBridge.settingsChapter() <= 1) {
                        chapterSheet.createObject().open();
                    } else {
                        YVBridge.reqText(YVTranslation.names()[YVBridge.settingsTranslation()].books[bkIdx].name, YVBridge.settingsChapter()-1);
                    }
                    active = false;
                    xSwipe = 768;
                    reverseTimer.start();
                    swipeTimer.start();
                }

                attachedObjects: [
                    QTimer {
                        id: reverseTimer;
                        interval: 150;
                        singleShot: true;
                        onTimeout: {
                            mainSH.curtain = true;
                            mainSH.xSwipe = mainSH.xSwipe * -1.0;
                            textArea.passageContent = "Loading...";
                        }
                    },
                    QTimer {
                        id: swipeTimer;
                        interval: 420;
                        singleShot: true;
                        onTimeout: {
                            mainSH.curtain = false;
                            mainSH.xSwipe = 0;
                            active = false;
                        }
                    }
                ]
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Bottom
                preferredHeight: 1280 - 110
                opacity: 1.0
            }
        }
    }
}
