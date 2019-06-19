import QtQuick 2.0
import QtQuick.Window 2.0
import QtLocation 5.6
import QtPositioning 5.6

Rectangle {
width: 512
height: 512
visible: true

    Plugin {
        id: mapboxglPlugin
            name: "mapboxgl"

            PluginParameter {
                name: "mapbox.access_token";
                value: "pk.eyJ1IjoianBmbGFuYWdhbiIsImEiOiJjanJyZ3RpN3MxemIwNDVuMzVqdWJicmFmIn0.XTq-C08x005zKLDIoDzamw"
            }
            PluginParameter {
                name: "Mapboxgl.mapping.additional_style_urls ";
                value: "mapbox://styles/jpflanagan/cjrxaw1km20ji1fjnwugqzl2z"
            }
            //PluginParameter {
            //    name: "Mapboxgl.mapping.cache.directory "
            //    value: "C:/temp/"
            //}
            // PluginParameter {
            //        name: "mapbox.map_id"
            //        // value: "mapbox.streets"
            //        value: "mapbox.streets-satellite"
            // }
        }

        Map {
            anchors.fill: parent
            plugin: mapboxglPlugin
            center: QtPositioning.coordinate(37.318357, -120.487592) // Merced
            zoomLevel: 14

            activeMapType: supportedMapTypes[5]


        }
}
