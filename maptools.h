#ifndef MAPTOOLS_H
#define MAPTOOLS_H

#include <QList>
#include <QtQml/QQmlEngine>
#include <QtLocation>
#include <QQmlComponent>
#include <QtPositioning>
#include <QQuickWidget>

class MapTools
{
public:
    MapTools();

    static bool addMarker(QString name, QGeoCoordinate *position, QQuickWidget *map, QColor color);
    static bool addCircle(QGeoCoordinate *center, double radius, QQuickWidget *map, QColor color );
    static bool addPolygon( QVariantList *poly, QQuickWidget *map, QColor color );
    static bool centerMap(QVariant *p, QQuickWidget *map);
    static bool clearMap(QQuickWidget *map);
};

#endif // MAPTOOLS_H
