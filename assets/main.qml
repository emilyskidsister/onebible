/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

import bb.cascades 1.0
import bb.system 1.0
import bb.platform 1.0
import utils 1.0

TabbedPane {
    id: root
    showTabsOnActionBar: false

    SearchTab {

    }

    BibleTab {
        id: bibleTab
    }

    BookmarksTab {
        id: bookmarksTab
    }

    HistoryTab {
        id: historyTab
    }
    
    onActiveTabChanged: {
        bookmarksTab.sync();
        historyTab.sync();
    }
    
    
    attachedObjects: [
        SystemToast {
            id: navigationHintToast
            body: "To navigate, tap the titlebar or swipe."
            button.label: "Ok"
        },
        SystemToast {
            id: bookmarkToast
            body: "Bookmark " + (YVBridge.bookmarked ? "added." : " removed.") // remember, it's already happened.
            button.label: "Ok"
            position: SystemUiPosition.BottomCenter;
        },
        ComponentDefinition {
            id:chapterSheet 
            source: "BookSelection.qml"
        },
        ComponentDefinition {
            id:trSheet 
            source: "TrSelection.qml"
        },
        ComponentDefinition {
            id:bookmarkTagger 
            source: "BookmarkTagger.qml"
        }
    ]
    
    property int chIdx : YVBridge.chapter;
    
    onCreationCompleted: {
        var bkIdx = 1;
        var tnstb = YVTranslation.names()[YVBridge.settingsTranslation()].books;
        var tbk = YVBridge.settingsBook();
        YVBridge.setTranslation(YVTranslation.names()[YVBridge.settingsTranslation()].idx);
        
        for (var i = 0; i < tnstb.length; ++i) {
            if (tnstb[i].abbrev === tbk) {
                bkIdx = i;
                break;
            }
        }
        
        YVBridge.reqText(tnstb[bkIdx].name, YVBridge.chapter);
        
        YVBridge.setLaunching(false);
        
        if (YVBridge.settingsFirst())
                navigationHintToast.show();
    }
    activeTab: bibleTab
    peekEnabled: (activeTab != bibleTab);
}
