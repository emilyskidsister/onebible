/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

import bb.cascades 1.0
import utils 1.0

Sheet {
    id: chapterSelection
    peekEnabled: false;
    property string name : "undefined";
    function init() {
        lv.init();
    }
    content: NavigationPane {
        id: np;
        function go() {
            YVBridge.reqText(name, parseInt(dataModel.data(lv.selectionList()[0])));
            chapterSelection.close();
            bookSelection.close();
        }
        Page {
            titleBar: TitleBar {
                title: name
                dismissAction: ActionItem {
                    title: "Back"
                    onTriggered: {
                        chapterSelection.close();
                    }
                }
                acceptAction: ActionItem {
                    title: "Go"
                    onTriggered: {
                        np.go();
                    }
                }
            }
            Container {
                id: c1;
                ListView {
                    id: lv;
                    layout: GridListLayout {
                        columnCount: 5
                        cellAspectRatio: 1.0
                        horizontalCellSpacing: 6.0
                        verticalCellSpacing: 6.0
                        spacingBeforeHeader: 8.0
                        spacingAfterHeader: 6.0
                    }
                    dataModel: ArrayDataModel {
                        id: dataModel
                    }
                    function init() {
                        var tnstb = YVTranslation.names()[YVBridge.settingsTranslation()].books;
                        var tbk = chapterSelection.name;
                        var bkIdx = 0;
                        
                        for (var i = 0; i < tnstb.length; ++i) {
                            if (tnstb[i].name === tbk) {
                                bkIdx = i;
                                break;
                            }
                        }

                        for (var i = 1; i <= tnstb[bkIdx].chapters; ++i)
                            dataModel.append(i); 
                        
                        var j = 1;
                        if (YVBridge.bookName === tbk) {
                            j = YVBridge.chapter;
                        }
                        select([j-1]);
                        scrollToItem([j-1], ScrollAnimation.None); // animations look cool, but we don't get touch events until a second after it ends... :/
                    }
                    onTriggered: {
                        clearSelection();
                        select(indexPath);
                        goTimer.start();
                    }
                    opacity: 1.0
                    scrollIndicatorMode: ScrollIndicatorMode.Default
                }
            }
        }
    }
    attachedObjects: [
        QTimer {
            id: goTimer
            interval: 150;
            singleShot: true;
            onTimeout: {
                np.go();
            }
        }
    ]
}
