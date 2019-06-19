#include "maptools.h"

MapTools::MapTools()
{

}

bool MapTools::addMarker(QString name, QGeoCoordinate *position, QQuickWidget *map, QColor color)
{
    QObject* target = qobject_cast<QObject*>(map->rootObject());
    QString functionName = "addMarker";
    QMetaObject::invokeMethod(target, functionName.toUtf8(), Qt::AutoConnection, Q_ARG(QVariant, name), Q_ARG(QVariant, position->latitude()), Q_ARG(QVariant, position->longitude()), Q_ARG(QVariant, QVariant::fromValue(color)));
    QMetaObject::invokeMethod(target, "fitAll", Qt::AutoConnection);

    return true;
}

bool MapTools::addCircle(QGeoCoordinate *center, double radius, QQuickWidget *map, QColor color)
{
    QObject* target = qobject_cast<QObject*>(map->rootObject());
    QString functionName = "addCircle";
    QMetaObject::invokeMethod(target, functionName.toUtf8(), Qt::AutoConnection, Q_ARG(QVariant, center->latitude()), Q_ARG(QVariant, center->longitude()), Q_ARG(QVariant, radius), Q_ARG(QVariant, QVariant::fromValue(color)), Q_ARG(QVariant, 1));
    QMetaObject::invokeMethod(target, "fitAll", Qt::AutoConnection);

    return true;
}

bool MapTools::addPolygon(QVariantList *poly, QQuickWidget *map, QColor color)
{
    QObject* target = qobject_cast<QObject*>(map->rootObject());
    QString functionName = "addPolygon";
    QMetaObject::invokeMethod(target, functionName.toUtf8(), Qt::AutoConnection, Q_ARG(QVariant, *poly), Q_ARG(QVariant, QVariant::fromValue(color)), Q_ARG(QVariant, 1));
    QMetaObject::invokeMethod(target, "fitAll", Qt::AutoConnection);

    return true;
}

bool MapTools::centerMap(QVariant *p, QQuickWidget *map)
{
    QObject *obj = map->rootObject();
    QQuickItem *mainMap = obj->findChild<QQuickItem*>("mainMap");
    mainMap->setProperty("center", *p);

    //QPolygon

    QQuickItem *c = new QQuickItem();
    // (QQuickWidget)

    //QPolygon

    QGeoPolygon *gp = new QGeoPolygon();
    //gp->addCoordinate()

    return true;
}

bool MapTools::clearMap(QQuickWidget *map)
{
    QObject* target = qobject_cast<QObject*>(map->rootObject());
    QString functionName = "clearMap";
    QMetaObject::invokeMethod(target, functionName.toUtf8(), Qt::AutoConnection);

    return true;
}
