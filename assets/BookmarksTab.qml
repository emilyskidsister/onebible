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
    title: qsTr("Bookmarks")
    imageSource: "asset:///images/bookmark.png";
    Page {
        id: tab2
        titleBar: TitleBar {
            title: "Bookmarks"
            kind: TitleBarKind.Default;
        }
        Container {
            layout: DockLayout {}

            Label {
                id: bookmarkText;
                multiline: true;
                textFormat: TextFormat.Html
                text: qsTr("<center>You have no bookmarks.\nAdd them from the Bible tab.</center>")
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                textStyle.textAlign: TextAlign.Center
            }
            ListView {
                id: bookmarksDataView;
                dataModel: ArrayDataModel {
                    id: bookmarkDataModel
                }
                implicitLayoutAnimationsEnabled: false;
                onTriggered: {
                    YVBookmarks.activate(indexPath[0]);
                    root.activeTab = bibleTab;
                }
                opacity: 1.0
            }
        }
    }
    function sync()  {
        bookmarkDataModel.clear();
        var books = YVBookmarks.books;
        var chaps = YVBookmarks.chapters;
        for (var i = 0; i < books.length; ++i) {
            bookmarkDataModel.append(books[i] + " " + chaps[i]);
        }
        bookmarkText.visible = !books.length;
    }
}
