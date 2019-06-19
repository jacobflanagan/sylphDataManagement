import QtQuick 2.0
import QtQuick.Window 2.0
import QtLocation 5.6
import QtPositioning 5.6

Rectangle {
    id: page
    width: 320; height: 480
    color: "lightgray"

    Plugin {
        id: mapPlugin
        name: "osm" // "osm" "mapboxgl", "esri", ...
        // specify plugin parameters if necessary
        PluginParameter {
            name: "osm.mapping.host";
            value: "https://cartodb-basemaps-{s}.global.ssl.fastly.net/light_all/{z}/{x}/{y}.png"
        }
//        PluginParameter {
//            name:"googlemaps.maps.apikey"
//            value:"<AIzaSyCJfeesi3jHhYVH36aZlPOHhCXU3JI5LYM>"
//        }

    }

    Map {
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(59.91, 10.75) // Oslo
        zoomLevel: 14

        activeMapType:  MapType.SatelliteMapDay;
    }
}
