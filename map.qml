import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.5
import QtLocation 5.11
import QtPositioning 5.6


Rectangle {
objectName: "rec"
width: 512
height: 512
visible: true

    Plugin {
        id: mapPlugin

        name: "mapboxgl"
            //PluginParameter {
            //name: "mapboxgl.mapping.items.insert_before"
            //value: "road-label-small"
            //}
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
            //PluginParameter {
            //name: "Mapboxgl.mapping.cache.directory "
            //value: "C:\\Temp\\maptiles"
            //}
    }

    Map {
        objectName: "mainMap"
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(37.30, -120.48)  //(-27, 153.0) // Merced
        zoomLevel: 14


        function addPolygon(path, color, borderWidth) {
            var component = Qt.createComponent("qrc:///qml/polygon.qml")
            var item = component.createObject(window, { path: path, color:
            color, borderWidth: borderWidth })
            map.addMapItem(item)
        }



        MapPolygon {
            objectName: "mapPoly"
            color: 'green'
            border.width: 2
            border.color: "black"
            path: [
                { latitude: -27, longitude: 153.0 },
                { latitude: -27, longitude: 154.1 },
                { latitude: -28, longitude: 153.5 }
            ]
        }

        MapPolygon {
            objectName: "mapPoly2"
            color: 'green'
            border.width: 2
            border.color: "black"
            path: [
                { latitude: -28, longitude: 153.0 },
                { latitude: -27, longitude: 154.1 },
                { latitude: -28, longitude: 153.5 }
            ]
        }

    }

}
