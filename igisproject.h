#ifndef IGISPROJECT_H
#define IGISPROJECT_H

#include "p4dproject.h"

#include <QDir>
#include <QMap>


class IGISProject
{
public:
    IGISProject(QDir directory);

    static bool isProject(QDir directory);

    QDir directory;

    bool exists;
    QMap<QString, Mission *> *missions;
    QJsonDocument *projectDetails;
    QDateTime *DateTime();
    bool writeProjectDetails(QDir directory);
};

#endif // IGISPROJECT_H
