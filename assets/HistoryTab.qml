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
    title: qsTr("History")
    imageSource: "asset:///images/historypng";
    Page {
        actions: [
            ActionItem {
                title: "Clear History"
                onTriggered: {
                    YVHistory.clearHistory();
                    historyDataModel.clear();
                }
            }
        ]
        id: tabH
        titleBar: TitleBar {
            title: "History"
            kind: TitleBarKind.Default;
        }
        Container {
            layout: DockLayout {}

            ListView {
                id: dataView;
                dataModel: ArrayDataModel {
                    id: historyDataModel
                }
                implicitLayoutAnimationsEnabled: false;
                onTriggered: {
                    YVHistory.activate(indexPath[0]);
                    root.activeTab = bibleTab;
                }
                opacity: 1.0
            }
        }
    }
    function sync() {
        historyDataModel.clear();
        var books = YVHistory.books;
        var chaps = YVHistory.chapters;
        for (var i = 0; i < books.length; ++i) {
            historyDataModel.append(books[i] + " " + chaps[i]);
        }
    }
}
