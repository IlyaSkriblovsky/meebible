import QtQuick 1.1
import com.nokia.meego 1.0
import MeeBible 0.1

PageStackWindow {
    id: window

    property bool created: false

    Binding {
        target: settings
        property: "fullscreen"
        value: ! window.showStatusBar
        when: created
    }

    Binding {
        target: window
        property: "showStatusBar"
        value: ! settings.fullscreen
        when: ! created
    }

    Component.onCompleted: created = true

    initialPage: BibleViewPage {
        id: biblePage
    }
}
