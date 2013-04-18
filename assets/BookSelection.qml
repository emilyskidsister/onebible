/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

import bb.cascades 1.0
import utils 1.0

Sheet {
    id: bookSelection
    peekEnabled: false
    property bool init : false;
    
        Page {
            id: np;
            function go() {
                var newObj = chSheet.createObject();
                newObj.name = dataModel.data(dataView.selectionList()[0])
                newObj.init();
                newObj.open();
            }
            titleBar: TitleBar {
                title: "The Bible"
                dismissAction: ActionItem {
                    title: "Cancel"
                    onTriggered: {
                        bookSelection.close();
                    }
                }
                acceptAction: ActionItem {
                    title: "Next"
                    onTriggered: {
                        np.go();
                    }
                }
            }
            Container {
                id: c1;
                ListView {
                    id: dataView;
                    dataModel: ArrayDataModel {
                        id: dataModel
                    }
                    onCreationCompleted: {
                        var bkIdx;
                        var st = YVBridge.settingsTranslation();
                        var tn = YVTranslation.names();
                        var sb = YVBridge.settingsBook();
                        var books = tn[st].books;
                        for (var i = 0; i < books.length; ++i) {
                            dataModel.append(books[i].name);
                            if (books[i].abbrev === sb) {
                                bkIdx = i;
                            }
                        }
                        
                        select([bkIdx]);
                        scrollToItem([bkIdx], ScrollAnimation.None); // animations look cool, but are riddled with bugs
                    }
                    implicitLayoutAnimationsEnabled: false;
                    onTriggered: {
                        clearSelection();
                        select(indexPath);
                        goTimer.start();
                    }
                    opacity: 1.0
                }
            }
        }
    attachedObjects: [
        ComponentDefinition {
            id:chSheet 
            source: "ChapterSelection.qml"
        },
        QTimer {
            id: goTimer
            interval: 100;
            singleShot: true;
            onTimeout: {
                np.go();
            }
        }
    ]
}
