#include "igisproject.h"
#include "p4dproject.h"

using namespace std;

P4dProject::P4dProject(QDir directory)
{
    QString dirString = QDir::toNativeSeparators(directory.absolutePath());

    //Check if not a p4d project, otherwise detele this (make sure to test for null)
    if (isProject(directory)) {

        this->exists = true;
        this->missions = new QMap<QString, Mission*>;

        this->directory = directory;
        this->projectDetails = getProjectDetails(directory);

        //Get this list of could-be missions (sub-directories)
        QStringList missionList = directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

        for(QString mission : missionList){

            Mission *m = new Mission(QDir(directory.absoluteFilePath(mission)));
            if (m->state == Mission::STATE::FLOWN)
                (*this->missions)[mission] = m;

        }
    }

    else //Not a project
    {
        this->exists = false;
    }
}

P4dProject::~P4dProject()
{

}

QMap<QString, Mission *>* P4dProject::getFlownMissions()
{
    QMap<QString, Mission *> *missionList = new QMap<QString, Mission *>; //only type that I can use the [] operator on...

    QMap<QString, Mission*> ml;

    //Create missions for those that were flown
    //Get this list of could-be missions (sub-directories)
    this->directory.setSorting(QDir::LocaleAware);
    QStringList projectList = this->directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot); //List of sub-directories, should be projects

    //attempting sorting using numberic mode
    //QCollator collator;
    //collator.setNumericMode(true);
    //projectList.sort(//projectList.begin(),projectList.end(),collator);

    for (QString project: projectList){
        QDir missionDir = QDir(this->directory.absoluteFilePath(project));
        if ( Mission::isFlown(missionDir) ) {
            Mission* m = new Mission(missionDir);
            (*missionList)[project] = m;
        }
    }

    return missionList;
}

bool P4dProject::isProject(QDir directory)
{

    if( QFileInfo::exists(directory.absoluteFilePath("project_details.json"))
            && QFileInfo(directory.absoluteFilePath("project_details.json")).isFile() ){

        //check if a true P4dProject by looking for the selectedTab entry - if this exists, its assumed this was created using pix4d capture
        QJsonDocument *pd = getProjectDetails(directory);
        if (pd->object().contains("selectedTab"))
            return true;
    }

    return false;
}

QJsonDocument *P4dProject::getProjectDetails(QDir directory)
{
    //read flight detail's file (json format)
    QFile pd_file(directory.absoluteFilePath(project_details_filename));
    if (!pd_file.open(QIODevice::ReadOnly | QIODevice::Text)) return nullptr; //problem reading file, maybe non-existant
    QString val = pd_file.readAll();
    pd_file.close();

    //set the document
    QJsonDocument *pd = new QJsonDocument();
    *pd = QJsonDocument::fromJson(val.toUtf8());

    //close file and return
    return pd;
}

bool P4dProject::writeProjectDetails(QDir directory)
{
    QFile pd_file(directory.absoluteFilePath(project_details_filename));
    if (!pd_file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    pd_file.write(projectDetails->toJson());
    return true;
}

QDateTime *P4dProject::DateTime()
{
    QJsonObject json = this->projectDetails->object();
    auto epochTime = json["creationTime"].toDouble();

    QDateTime *dt = new QDateTime(QDateTime::fromMSecsSinceEpoch(epochTime));
    return dt;
}



Mission::Mission(QDir directory, QString p4dPath, QString image_dir_basename){ //image_dir_basename is redundant, consider removing

    //determine and set mission type
    qInfo("Directory and file: " + directory.absoluteFilePath("details.pix4dcapture-mission").toUtf8());
    if (QFileInfo(directory.absoluteFilePath("details.pix4dcapture-mission")).exists()){
        this->cMethod = "Pix4D Capture";
    }
    else{
        this->cMethod = "Manual";
    }

    this->img_dir_basename = ::image_dir_basename;

    this->state = getMissionState(directory.absoluteFilePath(sync_state_filename));

    int state = this->state;

    if (state == Mission::STATE::FLOWN){ //sync file didn't exist, skip setup. Also had an issue considering "Planned", so ignore them too. Only considering state 3, flown


        //Initialize Mission
        this->name = QFileInfo(directory.absolutePath()).fileName();

        if (p4dPath.isEmpty()) p4dPath = directory.absoluteFilePath(this->name + ".p4d");
        this->p4d = getP4d( p4dPath );

        this->directory = directory;
        if (this->cMethod == "Manual")
            this->flightDetails = getFlightDetails(directory,"mission_details.json"); //Read flight details
        else
            this->flightDetails = getFlightDetails(directory,mission_details_filename);


        this->images = getImageList();

        //map info
        getFootPrint();
        getCenterCoordinate();

        QString notePath = directory.absoluteFilePath(this->name + ".notes");
        this->notes = loadNotes(notePath);


    }

}

//void init_mission(QDir directory){
//    //create a manual mission; create directory and initialize
//}

//Mission::Mission(QDir directory)
//{
//    if(!QFile(directory.absoluteFilePath("IGIS.xml")).exists())
//        init_mission(directory);

//    this->img_dir_basename = ::image_dir_basename;
//    this->state = getMissionState(directory.absoluteFilePath(sync_state_filename));
//}

Mission::~Mission(){
    QString notePath = directory.absoluteFilePath(this->name + ".notes");
    saveNotes(notePath);
}

bool Mission::isMission(QDir directory)
{
    QFile s(directory.absoluteFilePath(sync_state_filename));
    return s.open(QIODevice::ReadOnly | QIODevice::Text);
}

bool Mission::isFlown(QDir directory)
{
    if (getMissionState(directory.absoluteFilePath(sync_state_filename)) == STATE::FLOWN) return true;
    return false;
}

int Mission::getMissionState(QString state_file)
{
    int state = STATE::ERR;

    QFile sFile(state_file);
    if (sFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream contents(&sFile);

        QString state_string = contents.readLine();
        if ( state_string.contains("FLOWN") )
            state = STATE::FLOWN;
        else if ( state_string.contains("PLANNED") )
            state = STATE::PLANNED;
        else state = STATE::UNKNOWN;
    }
    return state;
}

QString Mission::loadNotes(QString notePath)
{

    QString notes = "";
    QFile noteFile(notePath);

    if (!noteFile.exists())
    {
        //File doesn't exist, create it
        if (noteFile.open(QFile::WriteOnly | QFile::Text))
        {
            QTextStream stream( &noteFile );
            stream << "" << endl;
        }
        else
        {
            noteFile.close();
            return nullptr;
        }
    }
    else if (noteFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //Read existing notes
        QTextStream in(&noteFile);
        notes = in.readAll();
    }
    else
    {
        //something went wrong
        noteFile.close(); //This may cause an error
        return nullptr;
    }

    noteFile.close();
    return notes;

}

bool Mission::saveNotes(QString notePath)
{
    if (notePath.isEmpty()) notePath = directory.absoluteFilePath(this->name + ".notes");

    QFile noteFile(notePath);

    if (noteFile.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream stream( &noteFile );
        stream << this->notes;
        noteFile.close();
        return true;
    }

    noteFile.close();
    return false;
}

bool Mission::getFootPrint()
{

    QJsonObject json = this->flightDetails->object();

    auto regionType = json["missionPlan"].toObject()["surveyRegion"].toObject()["regionType"].toString();

    if (regionType == "Rectangle"){

        auto center_coord = json["missionPlan"].toObject()["surveyRegion"].toObject()["centerCoordinate"].toArray();
        auto lon = center_coord[0].toDouble();
        auto lat = center_coord[1].toDouble();

        auto rot = json["missionPlan"].toObject()["surveyRegion"].toObject()["rotation"].toDouble();
        auto h = json["missionPlan"].toObject()["surveyRegion"].toObject()["size"].toObject()["height"].toDouble();
        auto w = json["missionPlan"].toObject()["surveyRegion"].toObject()["size"].toObject()["width"].toDouble();

        auto Ox = w/2;
        auto Oy = h/2;

        QGeoCoordinate center(lat, lon);
        auto c1 = center.atDistanceAndAzimuth(Oy, rot).atDistanceAndAzimuth(Ox, rot+90);
        auto c2 = center.atDistanceAndAzimuth(Oy, rot).atDistanceAndAzimuth(Ox, rot-90);
        auto c3 = center.atDistanceAndAzimuth(Oy, rot+180).atDistanceAndAzimuth(Ox, rot-90);
        auto c4 = center.atDistanceAndAzimuth(Oy, rot+180).atDistanceAndAzimuth(Ox, rot+90);

        qInfo("getFootPrint Corner 1: %f %f", c1.longitude(), c1.latitude());

        this->footPrint.append(QVariant::fromValue(c1));
        this->footPrint.append(QVariant::fromValue(c2));
        this->footPrint.append(QVariant::fromValue(c3));
        this->footPrint.append(QVariant::fromValue(c4));

        return true;
    }

    auto coords = json["missionPlan"].toObject()["surveyRegion"].toObject()["outlineCoordinates"].toArray();

    for (auto coord: coords){

        auto coordArr = coord.toArray();

        auto lon = coordArr[0].toDouble();
        auto lat = coordArr[1].toDouble();

        qInfo("getFootPrint: Coords: %d %d",lon,lat);

        this->footPrint.append(QVariant::fromValue(QGeoCoordinate(lat,lon)));

    }

    return true;
}

bool Mission::getCenterCoordinate()
{
    QJsonObject json = this->flightDetails->object();

    auto lon = json["missionPlan"].toObject()["surveyRegion"].toObject()["centerCoordinate"].toArray()[0].toDouble();
    auto lat = json["missionPlan"].toObject()["surveyRegion"].toObject()["centerCoordinate"].toArray()[1].toDouble();

    this->centerCoordinate = QVariant::fromValue(QGeoCoordinate(lat,lon));

    return true;
}

double Mission::imageSyncRatio()
{
    double count = 0;
    for (auto i: this->images->keys()) {
        if (this->images->value(i)->state() >= Image::STATE::SYNCED) count += 1;
    }
    return count / static_cast<double>(this->images->size());
}

QMap<QString, Image*> *Mission::getImageList()
{
    QMap<QString, Image*> *imageList = new QMap<QString, Image*>;

    QJsonObject json = this->flightDetails->object();
    auto photos = json["actual"].toObject()["photos"].toArray();

    for (auto p: photos) {

        QString name = p.toObject()["mediaDescriptor"].toObject()["mediaId"].toString();
        auto pos_arr = p.toObject()["pose"].toObject()["location"].toArray();
        double lon = pos_arr[0].toDouble();
        double lat = pos_arr[1].toDouble();
        double alt = pos_arr[2].toDouble();

        Image *image = new Image(name);
        image->name = name;
        image->directory = QDir(this->directory.absoluteFilePath(this->img_dir_basename));
        image->p4d = this->p4d;
        image->currentState = image->state(); //get initial image state
        image->position = new QGeoCoordinate(lat,lon,alt);;

        (*imageList)[name] = image;
    }

    return imageList;
}

QJsonDocument* Mission::getFlightDetails(QDir directory, QString mission_details_filepath = mission_details_filename)
{
    //read flight detail's file (json format)
    QFile fd_file(directory.absoluteFilePath(mission_details_filepath));
    if (!fd_file.open(QIODevice::ReadOnly | QIODevice::Text)) return nullptr; //problem reading file, maybe non-existant
    QString val = fd_file.readAll();
    fd_file.close();

    //set the document
    QJsonDocument *fd = new QJsonDocument();
    *fd = QJsonDocument::fromJson(val.toUtf8());

    //close file and return
    return fd;
}

QDomDocument *Mission::getP4d(QString p4dPath)
{
    QFile p4dFile(p4dPath);
    QDomDocument *p4dDoc = new QDomDocument();

    if (!p4dFile.open(QIODevice::ReadOnly))
        return nullptr;
    if (!p4dDoc->setContent(&p4dFile)) {
        p4dFile.close();
        return nullptr;
    }
    p4dFile.close();

    return p4dDoc;
}



Image::Image(QString name, int state, QString fileName)
{
    this->name = name;
    this->currentState = state;
    this->fileName = fileName;
}

Image::~Image()
{

}

int Image::state()
{
    int state = STATE::NONE;
    //check is synced
    QFile i( this->directory.absoluteFilePath(this->name) );
    if (i.exists()) state += STATE::SYNCED;

    //CHANGE!!! get processed state from p4d file
    //QDomElement root=this->p4d->documentElement();
    //if (i.)
    state += STATE::PROCESSED;

    this->currentState = state;
    return state;
}

MissionStatistics::MissionStatistics()
{

}

MissionStatistics::~MissionStatistics()
{

}

double MissionStatistics::flightTime(QJsonDocument* flight_details)
{
    QJsonObject json = flight_details->object();
    return json["actual"].toObject()["flightTime"].toDouble();
}

double MissionStatistics::altitude(QJsonDocument *flight_details)
{
    QJsonObject json = flight_details->object();
    return json["missionPlan"].toObject()["altitude"].toDouble();
}

QDateTime MissionStatistics::executedAt(QJsonDocument *flight_details)
{
    QJsonObject json = flight_details->object();
    QString dt_str = json["metaData"].toObject()["executedAt"].toString();

    QDateTime dt = QDateTime::fromString(dt_str, "yyyy-MM-ddTHH:mm:ss.zzzZ");

    return dt;
}

QString MissionStatistics::captureDevice(QJsonDocument *flight_details)
{
    QJsonObject json = flight_details->object();
    QString s = json["captureDevice"].toObject()["camera"].toObject()["name"].toString();
    return s;
}

ProjectStatistics::ProjectStatistics(QDir directory)
{
    totalTime = 0;

    if (P4dProject::isProject(directory))
    {
        P4dProject* p = new P4dProject(directory);
        //Initialize Polygon List
        this->polygons = new QList<QVariantList>();

        //mission stats
        this->totalTime = 0;
        this->missionCount = 0;

        for(Mission *m: *p->missions)
        {
            this->missionCount++;
            totalTime += MissionStatistics::flightTime(m->flightDetails);

            //collate sensors
            this->sensors.append( MissionStatistics::captureDevice( m->flightDetails ) );

            //add mission polygon
            if (m->state == Mission::STATE::FLOWN){
                qInfo("ProjStats: Footprint Length: %d", m->footPrint.length());
                this->polygons->append(m->footPrint); //May create seg fault, need to test
            }
        }

        this->sensors.removeDuplicates();
        if (this->sensors.length() == 0)
            this->sensors.append("None");

        //date, time stats
        QDateTime dt(*(p->DateTime()));
        this->cDateTime = dt;

        //Get project type (or default it)
        QJsonObject json = p->projectDetails->object();
        auto pType = json["projectType"].toString();
        if (pType.isEmpty()){ //initialize with default value
            json.insert("projectType", "Merged Flight Blocks");
            p->projectDetails->setObject(json);
            p->writeProjectDetails(p->directory);
            qInfo("Project Type: "+json["projectType"].toString().toUtf8());
        }
        this->cMethod = "Pix4d Capture";

        delete p;

    }
    else if (IGISProject::isProject(directory)){
        IGISProject* p = new IGISProject(directory);

        //Initialize Polygon List
        this->polygons = new QList<QVariantList>();

        //mission stats
        this->totalTime = 0;
        this->missionCount = 0;

        qInfo("Mission count in ps: %f",p->missions->count());

        for(Mission *m: *p->missions)
        {
            this->missionCount++;
            totalTime += MissionStatistics::flightTime(m->flightDetails);

            //collate sensors
            this->sensors.append( MissionStatistics::captureDevice( m->flightDetails ) );

            //add mission polygon
            if (m->state == Mission::STATE::FLOWN){
                qInfo("ProjStats: Footprint Length: %d", m->footPrint.length());
                this->polygons->append(m->footPrint); //May create seg fault, need to test
            }
        }

        this->sensors.removeDuplicates();
        if (this->sensors.length() == 0)
            this->sensors.append("None");

        //date, time stats
        QDateTime dt(*(p->DateTime()));
        this->cDateTime = dt;

        //Get project type (or default it)
        QJsonObject json = p->projectDetails->object();
        auto pType = json["projectType"].toString();
        if (pType.isEmpty()){ //initialize with default value
            json.insert("projectType", "Merged Flight Blocks");
            p->projectDetails->setObject(json);
            p->writeProjectDetails(p->directory);
            qInfo("Project Type: "+json["projectType"].toString().toUtf8());
        }
        this->cMethod = "Manual";

        delete p;
    }

}
