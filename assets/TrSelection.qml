/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

import bb.cascades 1.0
import utils 1.0

Sheet {
    id: trSelection
    peekEnabled: false;
    function go() {
        YVBridge.setTranslation(lv.selectionList()[0]);
        trSelection.close();
    }
    content: Page {
        titleBar: TitleBar {
            title: "Translations"
            dismissAction: ActionItem {
                title: "Cancel"
                onTriggered: {
                    trSelection.close();
                }
            }
            acceptAction: ActionItem {
                title: "Done"
                onTriggered: {
                    trSelection.go();
                }
            }
        }
        Container {
            ListView {
                id: lv;
                dataModel: ArrayDataModel {
                    id: dataModel
                }
                onCreationCompleted: {
                }
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
        QTimer {
            id: goTimer
            interval: 150;
            singleShot: true;
            onTimeout: {
                trSelection.go();
            }
        }
    ]
    
    onCreationCompleted: {
        var tr = YVTranslation.names();
        var trId = 0;
        
        for (var i = 0; i < tr.length; ++i) {
            dataModel.append(tr[i].name);
        }
    
        lv.select([YVBridge.settingsTranslation()]);
        lv.scrollToItem([YVBridge.settingsTranslation()], ScrollAnimation.None); // animations look cool, but are riddled with bugs
    }
}
