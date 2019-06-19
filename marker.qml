import QtQuick 2.0
import QtQuick.Controls 2.0
import QtLocation 5.3

MapQuickItem
{
    id: marker
    anchorPoint.x: marker.width / 4
    anchorPoint.y: marker.height
    property string labelText
    property string circleColor: "red"
    sourceItem: Item
    {
//        Image
//        {
//            id: icon
//            source: "file:///C:/Users/flana/Documents/Qt Creator/SylphDataManagement/photo.png"
//            sourceSize.width: 10
//            sourceSize.height: 10
//        }

        Rectangle {
            id: circle
            width: 10
            height: width
            color: circleColor
            border.color: "black"
            border.width: 1
            radius: width*0.5
        }

        Rectangle
        {
            id: tag
            anchors.centerIn: label
            width: label.width + 4
            height: label.height + 2
            color: "#00000000"
        }

        Label
        {
            id: label
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: 20
            anchors.verticalCenterOffset: -12
            font.pixelSize: 16
            text: labelText
            color: "#00FFFFFF"
        }
    }
}
