import QtQuick 2.12
import QtLocation 5.11
import QtPositioning 5.6


Item
{
    id: window

    Plugin
    {
        id: mapPlugin
        name: "mapboxgl"

        PluginParameter {
        name: "mapboxgl.mapping.use_fbo"
        value: "false"
        }
        PluginParameter {
        name: "mapboxgl.access_token"
        value: "pk.eyJ1IjoianBmbGFuYWdhbiIsImEiOiJjanJyZ3RpN3MxemIwNDVuMzVqdWJicmFmIn0.XTq-C08x005zKLDIoDzamw"
        }
        PluginParameter {
        name: "mapboxgl.mapping.additional_style_urls"
        value: "mapbox://styles/jpflanagan/cjrxaw1km20ji1fjnwugqzl2z"
        //value: "mapbox://styles/jpflanagan/cjrymz50g38rx1fmrztnhie2l"
        }

    }

    function addMarker(name, latitude, longitude, color)
    {
        var component = Qt.createComponent("file:///C:/Users/flana/Documents/Qt Creator/SylphDataManagement/marker.qml")
        var item = component.createObject(window, { coordinate: QtPositioning.coordinate(latitude, longitude), labelText: name, circleColor: color })
        map.addMapItem(item)
    }

    function addCircle(latitude, longitude, radius, color, borderWidth)
    {
        var component = Qt.createComponent("file:///C:/Users/flana/Documents/Qt Creator/SylphDataManagement/circle.qml")
        var item = component.createObject(window, { center: QtPositioning.coordinate(latitude, longitude), radius: radius, color: color, borderWidth: borderWidth })
        map.addMapItem(item)
    }

    function addRectangle(startLat, startLong, endLat, endLong, color, borderWidth)
    {
        var component = Qt.createComponent("qrc:///qml/rectangle.qml")
        var item = component.createObject(window, { topLeft: QtPositioning.coordinate(startLat, startLong), bottomRight: QtPositioning.coordinate(endLat, endLong), color: color, borderWidth: borderWidth })
        map.addMapItem(item)
    }

    function addPolygon(path, color, borderWidth)
    {
        var component = Qt.createComponent( "file:///C:/Users/flana/Documents/Qt Creator/SylphDataManagement/polygon.qml" ); //"qrc:///qml/polygon.qml")
        var item = component.createObject(window, { path: path, color: color, borderWidth: borderWidth})
        map.addMapItem(item)
    }

    function clearMap()
    {
        map.clearMapItems();
    }

    function fitAll()
    {
        map.fitViewportToMapItems();
    }

    Map
    {
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(40.7264175,-73.99735)
        zoomLevel: 14

        Behavior on center {
          CoordinateAnimation {
            duration: 400
            easing.type: Easing.InOutExpo
           }
        }

        Component.onCompleted:
        {
            console.log("Component Completed");
            // Use QML to add markers to the map
            /*addMarker("Restaurant", 40.7274175,-73.99835)
            addMarker("My Home", 40.7276432,-73.98602)
            addMarker("School", 40.7272175,-73.98935)*/

            //addCircle(40.7274175,-73.99835, 250, "green", 3);
            //addRectangle(40.7274175,-73.99835, 40.7376432, -73.98602, "red", 2)
            var path = [{ latitude: 40.7324281, longitude: -73.97602 },
                        { latitude: 40.7396432, longitude: -73.98666 },
                        { latitude: 40.7273266, longitude: -73.99835 },
                        { latitude: 40.7264281, longitude: -73.98602 }];
            //addPolygon(path, "blue", 3);
            //console.log("Tried to add polygon");

        }
    }

    Rectangle {
        id: reset

        width: 30
        height: 30
        color: "#AADDDDDD"
        border.width: 1
        border.color: "#767676"
        anchors.right: parent.right //button position
        anchors.top: parent.top
        anchors.rightMargin: 5
        anchors.topMargin: 5
        anchors.bottomMargin: 5

        Text {
            anchors.centerIn: parent
            text: "Ø"
        }

        TapHandler {
            onTapped: map.fitViewportToMapItems();
        }
    }
}
