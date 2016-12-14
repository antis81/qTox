import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1

Item {
    id: root

    width: 480
    height: 200

    Rectangle {
        id: background

        anchors.fill: parent
        color: "#d6d2cf"
    }

    RowLayout {
        anchors.fill: parent

        Rectangle {
            id: selector

            width: 240
            Layout.fillHeight: true

            color: "#1c1c1c"

            Image {
                id: image1
                x: 72
                y: 50
                width: 97
                height: 101
                source: "qrc:/img/login_logo.svg"
            }


        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ComboBox {
                id: profileSelector

                model: 10

                Layout.fillWidth: true
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.minimumHeight: 24

                color: "#ffffff"

                TextField {
                    id: txtPassword
                    text: ""

                    anchors.fill: parent

                    placeholderText: qsTr("Enter the profile's password")
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignJustify

                Button {
                    id: btnStart

                    text: qsTr("Load Profile")
                }

                Button {
                    id: btnImport

                    text: qsTr("Import")
                }
            }
        }
    }
}
