import com.meego 1.0

BusyIndicator {
    property bool inDialog: false

    platformStyle: BusyIndicatorStyle {
        size: "large"
        inverted: inDialog
    }
}
