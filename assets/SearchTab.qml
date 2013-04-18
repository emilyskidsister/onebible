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
    title: qsTr("Search")
    imageSource: "asset:///images/search.png";
    Page {
        id: tab0
        titleBar: TitleBar {
            title: "Search (English, Español, Français)"
            kind: TitleBarKind.Default;
        }
        Container {
            leftPadding: 20;
            rightPadding: 20;
            topPadding: 20;
            bottomPadding: 20;
            TextField {
                id: searchField;
                hintText: "Search Term"
                onTextChanging: {
                    search.request = text;
                }
                input {
                    submitKey: SubmitKey.Search
                }
            }
            ScrollView {
                Container {
                    ListView {
                        dataModel : ArrayDataModel {
                            id: dataModel
                        }
                        opacity: 1.0
                        maxHeight: 900;
                        onTriggered: {
                            if (search.activate(indexPath[0]))
                                root.activeTab = bibleTab;
                        }
                    }
                }
            }
        }
        attachedObjects: [
            YVSearch {
                id: search;
                request: ""
                onResultsChanged: {
                    dataModel.clear();
                    for (var i = 0; i < results.length; ++i)
                        dataModel.append(results[i]);
                }
            }
        ]
    }
}
