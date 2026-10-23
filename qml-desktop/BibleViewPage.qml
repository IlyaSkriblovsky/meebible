import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtWebEngine
import MeeBible 0.1
import QtWebChannel 1.9

Page {
    property StackView stack

    header: Header {
        id: header
        Layout.fillWidth: true
        text: bibleViewController.title || "MeeBible"
        withIcon: true
        rtl: settings.translation !== null && settings.translation.rtl
    }

    footer: ToolBar {
        RowLayout{
            Button {
                text: '<<'
                onClicked: bibleViewController.loadPrevChapter()
            }

            Button {
                text: 'Go To'
                onClicked: (settings.gridPlaceSelector ? gridPlaceDialog : placeDialog).run(settings.place)
            }

            Button {
                text: '>>'
                onClicked: bibleViewController.loadNextChapter()
            }

            Button {
                text: 'Bookmarks'
                onClicked: {
                    const page = stack.push('qrc:/BookmarksPage.qml');
                    page.onBookmarkSelected.connect((place) => { bibleViewController.place = place })
                }
            }

            Button {
                text: 'Settings'
                onClicked: {
                    const page = stack.push('qrc:/SettingsPage.qml');
                    // page.onBookmarkSelected.connect((place) => { bibleViewController.place = place })
                }
            }

            Button {
                text: 'About'
                onClicked: stack.push('qrc:/AboutPage.qml')
            }
        }
    }

    Component.onCompleted: {
        languages.reload(true);
    }

    property bool created: false

    BibleViewController {
        id: bibleViewController
        translation: settings.translation
        place: settings.place
        webView: webView

        Component.onCompleted: {
            created = true
        }
    }

    Binding {
        target: settings
        property: "place"
        value: bibleViewController.place
        when: created
    }

    Binding {
        webView.url: bibleViewController.webViewUrl
    }

    QtObject {
        id: selectedVersesHolder
        property list<int> verses: bibleViewController.selectedVerses
        WebChannel.id: "selectedVersesHolder"

        function setVerses(verses, textWithoutNumbers, textWithNumbers) {
            bibleViewController.verseSelectionChanged(verses, textWithoutNumbers, textWithNumbers);
        }
    }

    WebChannel {
        id: socket

        registeredObjects: [selectedVersesHolder]
    }

    WebEngineView {
        id: webView
        anchors.fill: parent

        webChannel: socket

        onLoadingChanged: (loadingInfo) => {
            if (loadingInfo.status === WebEngineView.LoadSucceededStatus) {
                runJavaScript(bibleViewController.scriptJS)
            }
        }
    }

    PlaceDialog {
        id: placeDialog
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        onAccepted: bibleViewController.place = placeDialog.place()
    }

    GridPlaceDialog {
        id: gridPlaceDialog
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        onAccepted: bibleViewController.place = gridPlaceDialog.place()
    }



    ToolBar {
        id: verseActions

        // anchors.top: header.bottom
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter

        anchors.topMargin: 8

        opacity: 0.0


        Behavior on opacity {
            NumberAnimation {
                duration: 100
            }
        }


        states: [
            State {
                name: "visible"
                when: bibleViewController.selectedVerses.length > 0

                PropertyChanges {
                    target: verseActions
                    opacity: 1.0
                }
            }
        ]


        RowLayout {
            ToolButton {
                text: qsTr("Copy")

                onClicked: {
                    bibleViewController.copySelectedVerses(settings.copyVerseNumbers)
                    bibleViewController.clearSelection()
                    // if (SYMBIAN)
                    //     copyBanner.open()
                    // else
                    //     copyBanner.show()
                }
            }

            // ToolButton {
            //     text: qsTr("Share")

            //     onClicked: {
            //         if (!SYMBIAN)
            //         {
            //             bibleViewController.shareSelectedVerses(settings.copyVerseNumbers)
            //         }
            //         else
            //         {
            //             simpleShareDialog.open(
            //                 bibleViewController.place,
            //                 bibleViewController.selectedText(settings.copyVerseNumbers)
            //             )
            //         }

            //         bibleViewController.clearSelection()
            //     }
            // }

            ToolButton {
                text: qsTr("Bookmark")

                onClicked: {
                    if (bibleViewController.bookmarkSelectedVerses()) {
                    //     bookmarkBanner.text = qsTr("Bookmarked");
                    } else {
                    //     bookmarkBanner.text = qsTr("Already in bookmarks");
                    }

                    // if (SYMBIAN)
                    //     bookmarkBanner.open()
                    // else
                    //     bookmarkBanner.show()

                    bibleViewController.clearSelection()
                }
            }

            ToolButton {
                icon.source: "verse-actions-close-symbian.png"

                onClicked: bibleViewController.clearSelection()
            }
        }
    }

    Rectangle {
        id: busyIndicator

        enabled: false

        anchors.fill: parent

        color: settings.inverted ? '#000' : '#e0e0e0'

        property bool active: (languages.loading || (settings.translation && settings.translation.loading) || bibleViewController.loadingChapter)

        opacity: active ? 0.8 : 0.0

        // BusyIndicator {
        //     id: realBusyIndicator
        //     anchors.centerIn: parent
        //     running: busyIndicator.visible && active
        // }

        Label {
            id: busyLabel

            anchors.centerIn: parent
            // anchors.top: realBusyIndicator.bottom
            // anchors.topMargin: 30
            // anchors.left: parent.left
            // anchors.right: parent.right

            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap

            font.pixelSize: 36

            text: qsTr("Loading chapter")

            color: '#222'
        }

        MouseArea {
            anchors.fill: parent
        }

        Behavior on opacity {
            NumberAnimation {
                duration: 100
            }
        }

        states: [
            State {
                name: "chapter"
                when: bibleViewController.loadingChapter

                PropertyChanges { target: busyLabel; text: qsTr("Loading chapter") }
            },
            State {
                name: "languages"
                when: languages.loading

                PropertyChanges { target: busyLabel; text: qsTr("Loading translation list") }
            },
            State {
                name: "translation"
                when: settings.translation.loading

                PropertyChanges { target: busyLabel; text: qsTr("Loading book list") }
            }
        ]
    }

}
