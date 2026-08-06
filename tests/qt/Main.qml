import QtQuick

Window {
    width: 720
    height: 420
    visible: true
    color: "#111827"
    title: qsTr("Mandel Qt 6 Toolchain")

    Column {
        anchors.centerIn: parent
        spacing: 14

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#f9fafb"
            font.pixelSize: 30
            text: qsTr("Qt %1 is ready").arg(qtVersion)
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#93c5fd"
            font.pixelSize: 18
            text: qsTr("Portable core: %1 modules").arg(registryModuleCount)
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#9ca3af"
            font.family: "monospace"
            font.pixelSize: 16
            text: registryIdentity
        }
    }

}
