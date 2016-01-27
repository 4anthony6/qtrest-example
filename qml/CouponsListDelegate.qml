import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

Item {
    height: information.height

    Rectangle {
        id: rectangle1
        anchors.fill: parent
        color: "black"
        opacity: 0.5

        border.color: "black"
        border.width: 0
    }

    Column {
        id: information
        width: parent.width
        //height: parent.height - couponsList.spacing
        anchors.centerIn: parent

        Item {
            id: imageContainer
            width: parent.width
            height: width * 0.5

            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            Image {
                id: bgImage
                width: parent.width
                height: parent.height
                source: "qrc:/images/skid_bg_2.jpg"
                fillMode: Image.PreserveAspectCrop
                clip: true
                visible: image.status == Image.Error || image.status == Image.Null
            }

            Image {
                id: image
                width: parent.width
                height: parent.height
                source: mainImageLink
                fillMode: Image.PreserveAspectCrop
                clip: true
            }

            BusyIndicator {
                running: image.status === Image.Loading
                visible: image.status != Image.Ready
                anchors.centerIn: parent
            }
        }

        Text {
            id: titleText
            text: title
            font.bold: true
            font.pointSize: 16
            color: "white"
            wrapMode: Text.WordWrap
            width: parent.width
            clip: true
            maximumLineCount: 2
            horizontalAlignment: Text.AlignHCenter
        }

        Rectangle {
            width: parent.width
            height: 1
            border.color: "grey"
            color: "grey"
        }

        RowLayout {
            width: parent.width
            height: boughtCol.height*1.5
            spacing: 0

            Item {
                id: r1
                height: boughtCol.height

                Layout.fillWidth: true
                Column {
                    id: boughtCol
                    anchors.centerIn: parent
                    Text {
                        id: boughtText
                        text: qsTr("Bought")
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: "white"
                        font.pointSize: 10
                    }
                    Text {
                        id: boughtLbl
                        text: boughtCount
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: "white"
                        font.pointSize: 8
                    }
                }
            }

            Rectangle {
                width: 1
                height: parent.height
                border.color: "grey"
                color: "grey"
            }

            Item {
                id: r2
                height: cityCol.height

                Layout.fillWidth: true
                Column {
                    id: cityCol
                    anchors.centerIn: parent
                    Text {
                        id: cityText
                        text: qsTr("City")
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: "white"
                        font.pointSize: 10
                    }
                    Text {
                        id: cityLbl
                        text: cityName
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: "white"
                        font.pointSize: 8
                    }
                }
            }

            Rectangle {
                width: 1
                height: parent.height
                border.color: "grey"
                color: "grey"
            }

            Item {
                id: r3
                height: dateCol.height

                Layout.fillWidth: true
                Column {
                    id: dateCol
                    anchors.centerIn: parent
                    Text {
                        id: dateText
                        text: qsTr("Date")
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: "white"
                        font.pointSize: 10
                    }
                    Text {
                        id: dateLbl
                        text: createDate
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: "white"
                        font.pointSize: 8
                    }
                }
            }
        }
    }
}
