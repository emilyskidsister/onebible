/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

import bb.cascades 1.0

Sheet {
    id: bookmarkTagger
    peekEnabled: false;
    content: Page {
        titleBar: TitleBar {
            title: "Add Tags"
            dismissAction: ActionItem {
                title: "Cancel"
                onTriggered: {
                    bookmarkTagger.close();
                }
            }
            acceptAction: ActionItem {
                title: "Done"
                onTriggered: {
                    bookmarkTagger.close();
                }
            }
        }
        Container {
            topPadding: 15;
            bottomPadding: 15;
            leftPadding: 15;
            rightPadding: 15;
            Container {
                id: comp;
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                Container {
                    topPadding: 11;
                    bottomPadding: 11;
                    TextField {
                        id: tf;
                        hintText: "Eg: love, faith, repentance..."
                    }
                }
                
                Button {
                    text: "Add"
                    preferredWidth: 40;
                    onClicked: {
                        if (tf.text != "") dm.append(tf.text);
                        tf.text = ""
                        tf.requestFocus()
                    }
                }
            }
            ListView {
                dataModel: ArrayDataModel {
                    id: dm;
                }
                
                listItemComponents: [
                    ListItemComponent {
                        type: "";
                        Container {
                            topPadding: 15;
                            bottomPadding: 15;
                            leftPadding: 15;
                            rightPadding: 15;
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Container {
                                topPadding: 11;
                                bottomPadding: 11;
                                Label {
                                    text: ListItemData;
                                }
                            }
                            Button {
                                text: "x"
                                preferredWidth: 40;
                            }
                        }
                    }
                ]
            }
        }
    }
    onCreationCompleted: {
        tf.requestFocus();
    }
}
