#include "mainwindow.h"
#include "newmissiondialog.h"
#include "newprojectdialog.h"
#include "p4dproject.h"
#include "ui_mainwindow.h"

#include <QtQml/QQmlEngine>
#include <QtPositioning>
#include <QtLocation>
#include <QIcon>
#include <QString>
#include <QQmlComponent>
#include <QStyledItemDelegate>
#include <QTreeWidget>

#include <QComboBox>
#include <QQuickItem>
#include <QQuickView>
//#include <QMetaObject>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    readSettings();
    ui->setupUi(this);
    this->setWindowTitle("IGIS Drone Data Notebook");
    this->setWindowIcon(QIcon(":/IGIS.png"));
    this->showMaximized();

    //add items to project configuration area
    combo_project_type = new QComboBox(this);
    combo_project_type->addItems(QStringList() << "Merged Flight Blocks" << "Time Series / Seperate Flight Blocks");
    //auto it = ui->treeWidget_projectConfig->topLevelItem(0);
    auto itlist = ui->treeWidget_projectConfig->findItems("Project Type",Qt::MatchExactly,0);
    foreach(auto it, itlist)
        ui->treeWidget_projectConfig->setItemWidget(it,1,combo_project_type);
    //resize contents to all 0 column items
    for(int i = 0; i < ui->treeWidget_projectConfig->topLevelItemCount(); i++)
        ui->treeWidget_projectConfig->resizeColumnToContents(i);

    //Try and connecto to combo box here
    //connect(static_cast<QComboBox>ui->treeWidget_projectConfig.,SIGNAL(currentIndexChanged(const QString&)),
    //        this,SLOT(switchcall(const QString&)));

    this->pc = new ProjectControl();

    //Load Map (CHANGE!!! THIS TO WORK RELATIVE!!!)
    QString gMapURL = "England"; // this is where you want to point
    gMapURL = "http://maps.google.com.sg/maps?q="+gMapURL+"&oe=utf-8&rls=org.mozilla:en-US:official&client=firefox-a&um=1&ie=UTF-8&hl=en&sa=N&tab=wl";
    //ui->quickWidget_projectMap->setSource(QUrl::("https://www.google.com/maps/"));

    //QGeoPolygon

    //ui->quickWidget_missionMap->setSource(QUrl::fromLocalFile("C:\\Users\\flana\\Documents\\Qt Creator\\SylphDataManagement\\map.qml"));
    ui->quickWidget_missionMap->setSource(QUrl::fromLocalFile("C:\\Users\\flana\\Documents\\Qt Creator\\SylphDataManagement\\mapview2.qml"));
    //ui->quickWidget_missionMap->setSource(QUrl::fromLocalFile("mapview2.qml"));


    //ui->quickWidget_projectMap->setSource(QUrl::fromLocalFile("C:\\Users\\flana\\Documents\\Qt Creator\\SylphDataManagement\\map2.qml"));
    ui->quickWidget_projectMap->setSource(QUrl::fromLocalFile("C:\\Users\\flana\\Documents\\Qt Creator\\SylphDataManagement\\mapview.qml"));
    //ui->quickWidget_projectMap->setSource(QUrl::fromLocalFile("mapview.qml"));

//    QObject* target = qobject_cast<QObject*>(ui->quickWidget_projectMap->rootObject());
//    QString functionName = "addPolygon";

//    QVariantList l_coords;
//    l_coords << QVariant::fromValue(QGeoCoordinate (40.7324281, -73.97602));
//    l_coords << QVariant::fromValue(QGeoCoordinate (40.7396432, -73.98666));
//    l_coords << QVariant::fromValue(QGeoCoordinate (40.7273266, -73.99835));
//    l_coords << QVariant::fromValue(QGeoCoordinate (40.7264281, -73.98602));

//    //QMetaObject::invokeMethod(target, functionName.toUtf8(), Qt::AutoConnection, Q_ARG(QVariant, l_coords), Q_ARG(QVariant, QVariant::fromValue(QColor(100,100,100,100))), Q_ARG(QVariant, 1));
//    //QMetaObject::invokeMethod(target, "fitAll", Qt::AutoConnection);

//    QObject *obj = ui->quickWidget_missionMap->rootObject();
//    QObject *mapPoly = obj->findChild<QObject*>("mapPoly");
//    QQuickItem *mainMap = obj->findChild<QQuickItem*>("mainMap");

//    QQmlComponent MP(ui->quickWidget_missionMap->engine(),mainMap);
//    mainMap->setProperty("center", QVariant::fromValue(QGeoCoordinate(-27,153.0)) ); //This works...

//    const QByteArray qmlString = "import QtQuick 2.0; MapPolygon { objectName: \"test\" color: 'green' path: [ { latitude: -27, longitude: 153.0 }, { latitude: -27, longitude: 154.1 }, { latitude: -28, longitude: 153.5 } ]}";
//    MP.setData(qmlString, ui->quickWidget_missionMap->source());
//    QQuickItem *mapInstance = qobject_cast<QQuickItem *>(MP.create());
//    ui->quickWidget_missionMap->engine()->setObjectOwnership(mapInstance,QQmlEngine::JavaScriptOwnership);
//    if(mapInstance)
//                mapInstance->setParentItem(mainMap);

//    //std::list<QGeoCoordinate> listOfCoords;
//    QGeoCoordinate coord(-27,153.0);

//    QList<QVariant> listOfCoords;
//    //listOfCoords << QVariant::fromValue(coord);

//    listOfCoords << QVariant::fromValue(QGeoCoordinate(-27,153.0));
//    listOfCoords << QVariant::fromValue(QGeoCoordinate(-27,154.1));
//    listOfCoords << QVariant::fromValue(QGeoCoordinate(-27,153.5));


//    if (mapPoly) {
//        mapPoly->setProperty("path", listOfCoords); //[ { latitude: -27, longitude: 153.0 },{ latitude: -27, longitude: 154.1 },{ latitude: -28, longitude: 153.5 }]);
//        mapPoly->setProperty("color","#8ca3cfee");
//        mapPoly->setProperty("border.width", 2);        //WHY ISN'T BORDER WORKING???
//        mapPoly->setProperty("border.color", "black");  //WHY???
//    }

    //mapPoly->setProperty("path", "[ { latitude: -27, longitude: 153.0 },{ latitude: -27, longitude: 154.1 },{ latitude: -28, longitude: 153.5 }]");

    //Correct way to use custom widget as qwidget
    //QWidget* q = new QWidget();
    //ProjectWidget* p = new ProjectWidget(q);


    QObject::connect(ui->pushButton_syncDevice, SIGNAL(clicked()),
                     this, SLOT( syncButtonClicked() ));

    //Connect ui button to open function
    QObject::connect(ui->pushButton, SIGNAL(clicked()),
                     this, SLOT(load_projects()));

    //Connect ui Project Tree Widget to info function
    QObject::connect(ui->button_newProject, SIGNAL(clicked()),
                     this, SLOT(new_project()));

    //Connect ui Project Tree Widget to info function
    QObject::connect(ui->button_newMission, SIGNAL(clicked()),
                     this, SLOT(new_mission()));

    //Connect ui Project Tree Widget to info function
    QObject::connect(ui->treeWidget_projects, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
                     this, SLOT(project_info(QTreeWidgetItem*, int)));

    //Connect ui Project Tree Widget to info function
    QObject::connect(ui->treeWidget_missions, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
                     this, SLOT(mission_info(QTreeWidgetItem*, int)));

}

void MainWindow::load_projects()
{

    QString dirname = QFileDialog::getExistingDirectory(this, tr("Open Projects Directory"),
                                                 ui->text_projectDirectory->text(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if (dirname.isEmpty()){ return; } //User cancelled

    ui->progressBar_project->setFormat("Loading Projects: %p% ");
    dirname = QDir::toNativeSeparators(dirname); //convert slashes to os native
    ui->text_projectDirectory->setText(dirname); //Update Project dir field

    //Get this list of could-be projects (sub-directories)
    QDir directory(dirname);
    QStringList projectList = directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot); //List of sub-directories, should be projects
    projectList.sort(); //FIGURE OUT SORTING HERE

    //Confirm project type (p4d, something else or nieh) and initialize a list of widgets

    //If user thought to pick the actual project directory, check it instead
    if (projectList.isEmpty() && QFileInfo::exists(directory.absoluteFilePath("project_details.json")) && QFileInfo(directory.absoluteFilePath("project_details.json")).isFile())
        projectList.append(directory.absolutePath());
        //projectWidgets.append(new QTreeWidgetItem((QTreeWidget*)0));

    int i = 0; 
    
    ui->treeWidget_projects->clear();
    //also clear the mission widgets
    ui->treeWidget_missions->clear();
    
    for (auto project: projectList) {

        //qInfo(project.toUtf8());

        QDir *d = new QDir(directory.absoluteFilePath(project));

        //read project details HERE - determine creation method

        //Determine if it's a project and append
        if (P4dProject::isProject( *d ))
        {

            ProjectWidget* p = new ProjectWidget(d, this->pc, this);
            p->name->setText(project);
            p->cMethod = "Pix4D Capture";

            //Set the creation method
            auto it = ui->treeWidget_projectConfig->findItems("Creation Method", Qt::MatchExactly)[0];
            it->setText(1,p->cMethod);

            //Setup UI control
            p->pc->pName = ui->label_projectName;
            p->pc->pMissionCount = ui->label_missionCount;
            p->pc->pDate = ui->label_pDate;
            p->pc->pTime = ui->label_pTime;
            p->pc->missionList = ui->treeWidget_missions;
            p->pc->projectMap = ui->quickWidget_projectMap;
            p->pc->missionMap = ui->quickWidget_missionMap;
            p->pc->missionNotes = ui->textEdit_missionNotes;
            //p->pc->pDescription = ui->textBrowser_projectDescription;
            p->pc->pSensors = ui->label_pSensors;

            QTreeWidgetItem* qtw = new QTreeWidgetItem;
            ui->treeWidget_projects->addTopLevelItem(qtw);
            ui->treeWidget_projects->setItemWidget(qtw,0,p);

            ui->progressBar_project->setValue(static_cast<float>(++i)/static_cast<float>(projectList.length())*100);
        }
        //IGIS Type Project
        else if (IGISProject::isProject( *d )){
            //qInfo("Found IGIS Project");

            ProjectWidget* p = new ProjectWidget(d, this->pc, this);
            p->name->setText(project);

            //Setup UI control
            p->pc->pName = ui->label_projectName;
            p->pc->pMissionCount = ui->label_missionCount;
            p->pc->pDate = ui->label_pDate;
            p->pc->pTime = ui->label_pTime;
            p->pc->missionList = ui->treeWidget_missions;
            p->pc->projectMap = ui->quickWidget_projectMap;
            p->pc->missionMap = ui->quickWidget_missionMap;
            p->pc->missionNotes = ui->textEdit_missionNotes;
            //p->pc->pDescription = ui->textBrowser_projectDescription;
            p->pc->pSensors = ui->label_pSensors;

            QTreeWidgetItem* qtw = new QTreeWidgetItem;
            ui->treeWidget_projects->addTopLevelItem(qtw);
            ui->treeWidget_projects->setItemWidget(qtw,0,p);

            ui->progressBar_project->setValue(static_cast<float>(++i)/static_cast<float>(projectList.length())*100);

        }

    }

    //pre-load first mission widget
    //if (ui->treeWidget_projects->topLevelItemCount() > 0){
    //    QTreeWidgetItem* qtw = ui->treeWidget_projects->topLevelItem(0);
    //    project_info(qtw, 0); //why the 0 column? Eliminate?
    //}

    ui->progressBar_project->setValue(0);
    ui->progressBar_project->setFormat(" %p% ");

}

void MainWindow::load_missions()
{
    return;
}

void MainWindow::project_info(QTreeWidgetItem *w, int n = 0)
{
    //Report to Text Browser
    ProjectWidget *pw = static_cast<ProjectWidget*>(ui->treeWidget_projects->itemWidget(w,n));
    QString name = pw->name->text();

    //QML Stuff here for Map
    QVariantList l_coords;
    l_coords << QVariant::fromValue(QGeoCoordinate (40.7324281, -73.97602));
    l_coords << QVariant::fromValue(QGeoCoordinate (40.7396432, -73.98666));
    l_coords << QVariant::fromValue(QGeoCoordinate (40.7273266, -73.99835));
    l_coords << QVariant::fromValue(QGeoCoordinate (40.7264281, -73.98602));

    //Clear Map
    MapTools::clearMap(ui->quickWidget_projectMap);

    //Add Polygons
    for (auto polygon: *pw->ps->polygons){
        //qInfo("Polygon Length is %d",polygon.length());
        if (polygon.length() > 0)
            MapTools::addPolygon(&polygon, ui->quickWidget_projectMap, QColor(0,0,0,175));
    }
    //MapTools::addPolygon(&l_coords, ui->quickWidget_projectMap);

    //set up configuration information
    auto it = ui->treeWidget_projectConfig->findItems("Creation Method", Qt::MatchExactly)[0];
    it->setText(1,pw->cMethod);

    auto pt = ui->treeWidget_projectConfig->findItems("Project Type", Qt::MatchExactly)[0];

    //NOT WORKING
    QComboBox* box = (QComboBox*)ui->treeWidget_projectConfig->itemWidget(pt,1);
    if(pw->projType == "Time Series / Seperate Flight Blocks")
        box->setCurrentIndex(0);
    else {
        box->setCurrentIndex(1);
    }
}

void MainWindow::mission_info(QTreeWidgetItem *w, int n)
{

    //Save any info back to mission
    if(this->pc->loadedMission)
        this->pc->loadedMission->mission->notes = ui->textEdit_missionNotes->toPlainText();

    //Report to Text Browser
    MissionWidget *qw = static_cast<MissionWidget*>(ui->treeWidget_missions->itemWidget(w,n));

    this->pc->loadedMission = qw;
    QString notes = this->pc->loadedMission->mission->notes;

    //Load the mission notes to UI
    ui->textEdit_missionNotes->setText(notes);

    //QML Stuff here for Map
    //MapTools::centerMap(&this->pc->loadedMission->mission->centerCoordinate, this->pc->missionMap);
    //Clear Map
    MapTools::clearMap(ui->quickWidget_missionMap);

    //Add Polygon
    if (qw->mission->footPrint.length() > 0)
        MapTools::addPolygon(&qw->mission->footPrint, ui->quickWidget_missionMap, QColor(183,222,255,150));
    //MapTools::addPolygon(&l_coords, ui->quickWidget_projectMap);

    //Add Photo points
    for (auto i: qw->mission->images->keys()) {
        auto image = qw->mission->images->value(i);
        //qInfo("mission info: %f", qw->mission->images->value(i)->state());
        if (qw->mission->images->value(i)->state() >= Image::STATE::SYNCED) {
            MapTools::addMarker(image->name,image->position,ui->quickWidget_missionMap, QColor(0,255,0));
            //MapTools::addCircle(image->position,2,ui->quickWidget_missionMap,QColor(0,255,0));
        }
        else {
            MapTools::addMarker(image->name,image->position,ui->quickWidget_missionMap, QColor(255,0,0));
            //MapTools::addCircle(image->position,2,ui->quickWidget_missionMap,QColor(255,0,0));
        }
    }
    
    //focus on first mission

//    for (auto image: *qw->mission->images){
//        qInfo("mission info: %f", image->currentState);
//        if (image->currentState == Image::STATE::SYNCED)
//            MapTools::addMarker(image->name,image->position,ui->quickWidget_missionMap, QColor(0,255,0));
//        else {
//            MapTools::addMarker(image->name,image->position,ui->quickWidget_missionMap, QColor(255,0,0));
//        }
//    }

}

void MainWindow::syncButtonClicked()
{
    qInfo("Sync Button Pressed");

    QObject* target = qobject_cast<QObject*>(ui->quickWidget_projectMap->rootObject());
    QString functionName = "fitAll";

    QMetaObject::invokeMethod(target, functionName.toUtf8(), Qt::AutoConnection);

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //optional check if the user really want to quit
    // and/or if the user want to save settings
    writeSettings();
    event->accept();
}

void MainWindow::writeSettings()
{
    QSettings settings("reaffer Soft", "reafferApp");

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings("reaffer Soft", "reafferApp");

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(400, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();
}

MainWindow::~MainWindow()
{
    //Save any info back to mission
    if(this->loaded)
        this->loaded->mission->notes = ui->textEdit_missionNotes->toPlainText();

    delete ui;
}

void MainWindow::init_IGISXML(QDir absolutePath, QString state)
{
    QFile file(absolutePath.absoluteFilePath("IGIS.xml"));
    file.open(QIODevice::WriteOnly);
    file.close();
}

void MainWindow::new_project()
{
    NewProjectDialog *dlg = new NewProjectDialog(this);
    dlg->setWindowTitle("New Project");

    if (  dlg->exec() == QDialog::Accepted  )  {
        QString name = dlg->getData(); // data could be anything, int, string etc.
        qInfo("data " + name.toUtf8() );
        // Ok was pressed, create a new project and add to list
        //new directory
        QDir projDir = QDir(QDir(ui->text_projectDirectory->text()).absoluteFilePath(name));
        if (projDir.exists()){
            qInfo("Directory/Project already exists");
        }
        else {
            //create a new directory for the project
            QDir().mkdir(projDir.absolutePath());
            QFile file(projDir.absoluteFilePath("project_details.json")); //FIX, make name const like in P4DProject!!
            file.open(QIODevice::WriteOnly);

            QJsonDocument *pd = new QJsonDocument();
            QJsonObject jo;
            auto cTime = QDateTime::currentMSecsSinceEpoch();
            jo.insert("creationTime", cTime);
            jo.insert("creationType", "Manual");
            jo.insert("projectType", "Merged Flight Blocks");
            pd->setObject(jo);
            //pd->object();
            file.write(pd->toJson());
            file.close();

            QDir *d = new QDir(projDir);
            ProjectWidget* p = new ProjectWidget(d, this->pc, this);
            p->name->setText(name);
            p->setMissionCount(0,0);

            auto *dt_cTime = new QDateTime(QDateTime::fromMSecsSinceEpoch(cTime));
            p->setCreationDateTime(dt_cTime);

            //p->setProjType()
            p->setSensors(QStringList("None"));
            p->cMethod = "Manual";

            //Setup UI control
            p->pc->pName = ui->label_projectName;
            p->pc->pMissionCount = ui->label_missionCount;
            p->pc->pDate = ui->label_pDate;
            p->pc->pTime = ui->label_pTime;
            p->pc->missionList = ui->treeWidget_missions;
            p->pc->projectMap = ui->quickWidget_projectMap;
            p->pc->missionMap = ui->quickWidget_missionMap;
            p->pc->missionNotes = ui->textEdit_missionNotes;
            //p->pc->pDescription = ui->textBrowser_projectDescription;
            p->pc->pSensors = ui->label_pSensors;

            QTreeWidgetItem* qtw = new QTreeWidgetItem;
            ui->treeWidget_projects->addTopLevelItem(qtw);
            ui->treeWidget_projects->setItemWidget(qtw,0,p);

        }

    }



    return;
}

void MainWindow::new_mission()
{
    auto projName = ui->label_projectName->text();
    if (projName == "Load a Project"){

        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Project not loaded.");
        msgBox.exec();

        return;
    }

    NewMissionDialog *dlg = new NewMissionDialog(this);
    dlg->setWindowTitle("New Mission");

    if (  dlg->exec() == QDialog::Accepted  )  {
        MissionProperties *mp = dlg->getData();
        qInfo("Here %f", mp->location_max[0]);
        if (mp->images.isEmpty() || mp->name.isEmpty())
            return; // same as cancel. We can warn in the future.

        //Set is flown flag
        QDir missionDir = QDir(QDir(ui->text_projectDirectory->text()).absoluteFilePath(projName)).absoluteFilePath(mp->name);
        if (missionDir.exists()){
            qInfo("Directory already exists");
        }
        else {
            qInfo("Creating Directory: "+missionDir.absolutePath().toUtf8());
            QDir().mkdir( missionDir.absolutePath() );

            QDir().mkdir( missionDir.absoluteFilePath("data")); //create data directory to copy images into

            QJsonObject json;

            //altitude
            QJsonObject actual;
            QJsonValue flightTime;
            flightTime = mp->start.secsTo(mp->end); //keep in seconds
            actual.insert("flightTime",flightTime);
            QJsonArray photos;
            //need to add photos here
            for (int i = 0; i < mp->images.length(); i++){
                QJsonObject p;
                QJsonObject mediaDescriptor;
                QJsonValue mediaId = QFileInfo(mp->images[i]).fileName(); //mp->images[i];
                mediaDescriptor.insert("mediaId",mediaId);
                p.insert("mediaDescriptor",mediaDescriptor);
                QJsonObject pose;
                QJsonArray location;
                QJsonValue lat = mp->lats[i];
                QJsonValue lng = mp->lngs[i];
                QJsonValue alt = mp->altitudes[i];
                location.insert(0,lng); location.insert(1,lat); location.insert(2,alt); //Pro-tip - these must go in order (0,1,2...)
                pose.insert("location",location);
                QJsonValue time = mp->times[i];
                pose.insert("time",time);
                p.insert("pose", pose);
                photos.insert(i,p);
            }
            actual.insert("photos",photos);
            json.insert("actual",actual);

            QJsonObject missionPlan;
            QJsonValue altitude = mp->Altitude;
            missionPlan.insert("altitude",altitude);
            QJsonObject surveyRegion;
            QJsonValue regionType = "Polygon";
            surveyRegion.insert("regionType",regionType);
            QJsonArray outlineCoordinates;

            QJsonArray c0; c0.insert(0, mp->location_min[1]); c0.insert(1, mp->location_min[0]);
            QJsonArray c1; c1.insert(0, mp->location_max[1]); c1.insert(1, mp->location_min[0]);
            QJsonArray c2; c2.insert(0, mp->location_max[1]); c2.insert(1, mp->location_max[0]);
            QJsonArray c3; c3.insert(0, mp->location_min[1]); c3.insert(1, mp->location_max[0]);
            outlineCoordinates.insert(0,c0); outlineCoordinates.insert(1,c1); outlineCoordinates.insert(2,c2); outlineCoordinates.insert(3,c3);
            surveyRegion.insert("outlineCoordinates",outlineCoordinates);

            QJsonArray centerCoordinate;
            centerCoordinate.insert(0,(mp->location_max[1]+mp->location_max[1])/2.0); centerCoordinate.insert(1,(mp->location_max[0]+mp->location_max[0])/2.0);
            surveyRegion.insert("centerCoordinate",centerCoordinate);

            missionPlan.insert("surveyRegion",surveyRegion);
            json.insert("missionPlan",missionPlan);

            //Capture Device
            QJsonObject captureDevice;
            QJsonObject camera;
            QJsonValue name = mp->sensor;
            camera.insert("name",name);
            QJsonObject parameters;
            QJsonValue name2 = mp->sensor;
            parameters.insert("name",name2);
            camera.insert("parameters",parameters);
            captureDevice.insert("camera",camera);
            json.insert("captureDevice",captureDevice);

            //Meta data - execution time
            QJsonObject metaData;
            QJsonValue executedAt = mp->start.toString("yyyy-MM-ddTHH:mm:ss.zzzZ");
            metaData.insert("executedAt", executedAt);
            json.insert("metaData",metaData);

            //home coordinate
            //double lat = mp->location_max[0]+mp->location_min[0]/2;
            double lat = mp->location_min[0];
            //double lon = mp->location_max[1]+mp->location_min[1]/2;
            double lon = mp->location_min[1];
            QJsonArray hc;
            hc.insert(0,lat);
            hc.insert(1,lon);
            json.insert("homeCoordinate", hc);

            QJsonDocument mDeets;
            mDeets.setObject(json);

            QFile file(missionDir.absoluteFilePath("mission_details.json"));
            file.open(QIODevice::WriteOnly);
            file.write(mDeets.toJson());
            file.close();

            //auto coords = json["missionPlan"].toObject()["surveyRegion"].toObject()["outlineCoordinates"];
            //auto j = json.insert("missionPlan", QJsonValue::Object);
            //QJsonArray oc;

            //copy mission template to new mission directory
            QString p4dpath = missionDir.absoluteFilePath(mp->name+".p4d");
            QFile::copy(ui->lineEdit_p4dTemplate->text(), p4dpath);
            auto doc = Mission::getP4d(p4dpath);

            //nav to pix4dmapper node
            auto n = doc->firstChild();
            while(n.nodeName() != "pix4dmapper" && !n.isNull()){
                n = n.nextSibling();
            }

            //nav to inputs node
            auto nn = n.firstChild();
            while(nn.nodeName() != "inputs" && !nn.isNull()){
                nn = nn.nextSibling();
            }

            //nav to images node
            auto nnn = nn.firstChild();
            QDomNode imgs;
            QDomNode pn;
            while(!nnn.isNull()){
                if (nnn.nodeName() == "images")
                    imgs = nnn;
                else if (nnn.nodeName() == "processingArea")
                    pn = nnn;
                nnn = nnn.nextSibling();
            }

            //nav to processing area node
//            auto pn = nn.firstChild();
//            while(pn.nodeName() != "processingArea" && pn.isNull()){
//                pn = pn.nextSibling();
//            }

            //fill the images
            for (auto i: mp->images){
                auto img = doc->createElement("image");
                QFileInfo i_file(i);
                img.setAttribute("path",i_file.fileName());
                imgs.appendChild(img);
            }

            //fill the processing area
            QDomElement pa;
            pa = doc->createElement("geoCoord2D");
            pa.setAttribute("lat",mp->location_min[0]);
            pa.setAttribute("lng",mp->location_min[1]);
            pn.appendChild(pa);
            pa = doc->createElement("geoCoord2D");
            pa.setAttribute("lat",mp->location_min[0]);
            pa.setAttribute("lng",mp->location_max[1]);
            pn.appendChild(pa);
            pa = doc->createElement("geoCoord2D");
            pa.setAttribute("lat",mp->location_max[0]);
            pa.setAttribute("lng",mp->location_max[1]);
            pn.appendChild(pa);
            pa = doc->createElement("geoCoord2D");
            pa.setAttribute("lat",mp->location_max[0]);
            pa.setAttribute("lng",mp->location_min[1]);
            pn.appendChild(pa);

            QFile p4dfile(p4dpath);
            p4dfile.open(QIODevice::WriteOnly);
            p4dfile.write(doc->toString().toUtf8());
            p4dfile.close();

            //create sync_state FLOWN file
            QFile ss_file(missionDir.absoluteFilePath("sync_state.json"));
            ss_file.open(QIODevice::WriteOnly);
            ss_file.write("\"FLOWN\"");
            ss_file.close();

            //Add mission to mission tree widget
            Mission *m = new Mission(missionDir,p4dpath,"data");
            MissionWidget* mw = new MissionWidget(m, this);

            QTreeWidgetItem* qtw = new QTreeWidgetItem;
            this->pc->missionList->addTopLevelItem(qtw);
            this->pc->missionList->setItemWidget(qtw,0,mw);

        }
    }

    return;
}
