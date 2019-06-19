#include "igisproject.h"
#include "p4dproject.h"

IGISProject::IGISProject(QDir directory)
{

    //create the IGISProject
    QString dirString = QDir::toNativeSeparators(directory.absolutePath());

    //Check if not a IGIS project, otherwise detele this (make sure to test for null)
    if (this->isProject(directory)) {

        this->exists = true;
        this->missions = new QMap<QString, Mission*>;

        this->directory = directory;
        this->projectDetails = P4dProject::getProjectDetails(directory);

        //Get this list of could-be missions (sub-directories)
        QStringList missionList = directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

        for(QString mission : missionList){

            qInfo("Looking at mission");

            Mission *m = new Mission(QDir(directory.absoluteFilePath(mission)));

            if (m->state == Mission::STATE::FLOWN)
                (*this->missions)[mission] = m;

        }

        qInfo("Number of projects: %d",missionList.length());
    }

    else //Not a project
    {
        this->exists = false;
    }

}

QDateTime *IGISProject::DateTime()
{
    QJsonObject json = this->projectDetails->object();
    auto epochTime = json["creationTime"].toDouble();

    QDateTime *dt = new QDateTime(QDateTime::fromMSecsSinceEpoch(epochTime));
    return dt;
}

bool IGISProject::writeProjectDetails(QDir directory)
{
    QFile pd_file(directory.absoluteFilePath(project_details_filename));
    if (!pd_file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    pd_file.write(projectDetails->toJson());
    return true;
}

bool IGISProject::isProject(QDir directory)
{

    if(QFile::exists(directory.absoluteFilePath("project_details.json"))){

        //check if a true P4dProject by looking for the selectedTab entry - if this exists, its assumed this was created using pix4d capture
        QJsonDocument *pd = P4dProject::getProjectDetails(directory);
        if (pd->object().contains("creationType"))
            if (pd->object()["creationType"].toString() == "Manual"){
                qInfo("IGIS Project Confirmed.");
                return true;
            }
    }

    return false;

}
