#ifndef P4DPROJECT_H
#define P4DPROJECT_H

#include <string>
#include <sys/stat.h>
#include <QDir>
#include <QStringList>
#include <QString>
#include <QTextStream>
#include <QMap>
#include <QtXml>
#include <QJsonDocument>
#include <QDateTime>
//#include <QGeoCoordinate>
#include <QtPositioning>

//Project Constants
const QString sync_state_filename = "sync_state.json";
const QString image_dir_basename = "data";  //Default capture directory, I believe pix4d looks for 'data' child folder by default
const QString mission_details_filename = "details.pix4dcapture-mission";
const QString project_details_filename = "project_details.json";

class Image
{
public:

    class STATE //state codes
    {
    public:
        static const int NONE = 0;
        static const int PROCESSED = 10;
        static const int SYNCED = 11;
        static const int ALL = 21;
    };

    explicit Image(QString name, int state = STATE::NONE, QString fileName = "");
    ~Image();

    /*
     * Image info
     *
     */

    QString name;
    QDir directory; //may not used
    QString fileName;
    int currentState;

    QGeoCoordinate *position;

    QDomDocument *p4d;

    int state();

};

class MissionStatistics
{
public:
    MissionStatistics();
    ~MissionStatistics();

    static double flightTime(QJsonDocument* flight_details);
    static double altitude(QJsonDocument* flight_details);
    static QDateTime executedAt(QJsonDocument* flight_details);
    static QString captureDevice(QJsonDocument* flight_details);
};

class Mission
{
public:

    class STATE //state codes
    {
    public:
        static const int FLOWN = 3;
        static const int PLANNED = 2;
        static const int UNKNOWN = 1;
        static const int ERR = 0;
    };

    explicit Mission(QDir directory, QString p4dFile = "", QString image_dir_basename = ::image_dir_basename);
    //explicit Mission(QDir directory);
    ~Mission();

    /*
     * Class unique functions
     */

    static bool isMission(QDir directory);
    static bool isFlown(QDir directory);
    static int getMissionState(QString state_file);

    QString loadNotes(QString notePath);
    bool saveNotes(QString notePath);

    double imageSyncRatio();

    bool getFootPrint();
    bool getCenterCoordinate();

    /*
     * Class Globals
     */

    QString cMethod;

    QString img_dir_basename;
    QString name;
    int state;
    QJsonDocument *flightDetails;
    QDomDocument *p4d;
    QDir directory;
    QMap<QString, Image*> *images; //Mission Images
    QString notes;

    QVariantList footPrint;
    QVariant centerCoordinate;

    QMap<QString, Image*> *getImageList();
    QJsonDocument *getFlightDetails(QDir directory, QString mission_details_filepath);
    static QDomDocument *getP4d(QString p4dFile);

};

class P4dProject
{
public:
    explicit P4dProject(QDir directory);
    ~P4dProject();

    QMap<QString, Mission *> *getFlownMissions();

    static bool isProject(QDir directory);

    /*
     * Class Globals
     */

    QDir directory;
    QJsonDocument *projectDetails;
    QMap<QString, Mission*> *missions; //Project Missions

    bool exists = false;

    //get stuff
    QDateTime *DateTime();

    bool writeProjectDetails(QDir directory);

    static QJsonDocument *getProjectDetails(QDir directory);

};

class ProjectStatistics
{
public:
    explicit ProjectStatistics(QDir directory);
    ~ProjectStatistics();

    int missionCount;
    double totalTime;
    QDateTime cDateTime;
    double altitude;
    QStringList sensors;
    QList<QVariantList> *polygons;

    QString projType;
    QString cMethod;

};

#endif // P4DPROJECT_H
