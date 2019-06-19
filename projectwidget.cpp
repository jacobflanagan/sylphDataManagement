#include "igisproject.h"
#include "maptools.h"
#include "projectwidget.h"
#include "ui_projectwidget.h"

//RESOURCE for creating a new QWidget: https://stackoverflow.com/questions/9696317/how-to-use-ui-file-for-making-a-simple-widget
//TIPS:
//Add ui_....h as above
//Make sure Form is properly named for ui_... to get set up correctly (Name QWidget and window title name)
//Add destructor! (duh)

//NOTES: get rid of missionList, this is already in pc (project control)
ProjectWidget::ProjectWidget(QDir *directory, ProjectControl *pc, QWidget *parent)
   : QWidget(parent),
    ui(new Ui::ProjectWidget)
{
    ui->setupUi(this);

    //shortcuts
    this->directory = directory;
    this->parent = parent;

    //link UI elements
    this->name = ui->name;
    this->missionCount = ui->label_missionCount;
    this->date = ui->label_date;
    this->time = ui->label_time;
    this->sensors = ui->label_sensors;

    this->pc = pc;

    if (directory != nullptr)
    {
        QDir d(*this->directory);

        ps = new ProjectStatistics(*this->directory);
        qInfo("Mission COunt: %f",ps->missionCount);
        setMissionCount(ps->missionCount, ps->totalTime);
        setCreationDateTime(&ps->cDateTime);
        this->sensors->setText("M+100"); //CHANGE!!!
        setSensors(ps->sensors);
        this->cMethod = ps->cMethod;
        //ui->label_time->setText(QString::number(ps->totalTime));
    }

    //Connect ui button to load function
    QObject::connect(ui->loadButton, SIGNAL(clicked()),
                     this, SLOT(load_missions()));

}

ProjectWidget::~ProjectWidget()
{
    delete ui;
}

void ProjectWidget::setMissionCount(int count, double seconds)
{
    QTime fd(0,0,0);
    fd = fd.addMSecs(static_cast<int>(seconds*1000));
    ui->label_missionCount->setText( QString().sprintf("%d Missions, %d' %d\"", count, (fd.hour()*60+fd.minute()), fd.second()) );
}

void ProjectWidget::setCreationDateTime( QDateTime *dt)
{
    //int t = dt->offsetFromUtc()/(60*60);
    //QString sign = "-";
    //if (t > 0) sign = "+";
    //this->time->setText( dt->toString("HH:mm:ss (") + sign + QString( "%1:00)" ).arg( abs(t), 2, 10, QChar('0')) );

    QString tz = dt->timeZone().displayName(*dt,QTimeZone::OffsetName);
    this->time->setText( dt->toString("HH:mm:ss ") + "<small>" + tz + "</small>" );
    this->date->setText( dt->toString("<b>yyyy.MM.dd</b>") );
}

void ProjectWidget::setSensors(QStringList sList)
{
    this->sensors->setText(sList.join(", "));
}

/*void ProjectWidget::mission_info(QTreeWidgetItem *w, int n)
{

    //Save any info back to mission
    if(this->pc->loadedMission)
        this->pc->loadedMission->mission->notes = ui->textEdit_missionNotes->toPlainText();

    //Report to Text Browser
    MissionWidget *qw = static_cast<MissionWidget*>(ui->treeWidget_missions->itemWidget(w,n));

    this->pc->loadedMission = qw;
    QString notes = this->pc->loadedMission->mission->notes;

    //Load the mission notes to UI
    pc->missionNotes->setText(notes);

    //QML Stuff here for Map
    //MapTools::centerMap(&this->pc->loadedMission->mission->centerCoordinate, this->pc->missionMap);
    //Clear Map
    MapTools::clearMap(this->pc->missionMap);

    //Add Polygon
    if (qw->mission->footPrint.length() > 0)
        MapTools::addPolygon(&qw->mission->footPrint, this->pc->missionMap, QColor(183,222,255,150));
    //MapTools::addPolygon(&l_coords, ui->quickWidget_projectMap);

    //Add Photo points
    for (auto i: qw->mission->images->keys()) {
        auto image = qw->mission->images->value(i);
        //qInfo("mission info: %f", qw->mission->images->value(i)->state());
        if (qw->mission->images->value(i)->state() >= Image::STATE::SYNCED)
            MapTools::addMarker(image->name,image->position,this->pc->missionMap, QColor(0,255,0));
        else
            MapTools::addMarker(image->name,image->position,this->pc->missionMap, QColor(255,0,0));
    }

}*/

void ProjectWidget::load_missions()
{

    P4dProject* p4d = new P4dProject(*this->directory);

    qInfo("creation Method is " + this->cMethod.toUtf8());

    //determine creation method
    if (this->cMethod == "Manual"){
        auto p4d = new IGISProject(*this->directory);

        //Setup banner (new way, instead of using missionBanner)
        this->pc->pName->setText(this->name->text());
        this->pc->pMissionCount->setText(this->missionCount->text());
        this->pc->pDate->setText(this->date->text());
        this->pc->pTime->setText(this->time->text());
        this->pc->pSensors->setText(this->sensors->text());


        //Clean up loaded missions
        if (this->pc->loadedMission)
            this->pc->loadedMission->mission->notes = this->pc->missionNotes->toPlainText();
        this->pc->loadedMission = nullptr;
        this->pc->missionNotes->clear();
        QTreeWidgetItemIterator it(this->pc->missionList);
        while (*it) {
            delete (*it);
        }
        this->pc->missionList->clear();

        for (Mission *m: *p4d->missions) {

            MissionWidget* mw = new MissionWidget(m, this->parent);

            QTreeWidgetItem* qtw = new QTreeWidgetItem;
            this->pc->missionList->addTopLevelItem(qtw);
            this->pc->missionList->setItemWidget(qtw,0,mw);

        }

    }
    else{
        //Create a project object
        auto p4d = new P4dProject(*this->directory);


        //Should never get an oppurtunity to do this...
        if (!p4d->exists) {
            QMessageBox msgBox;
            msgBox.setText("Not a p4d Project.\n"+this->name->text());
            msgBox.exec();
            return;
        }

        //Setup banner (new way, instead of using missionBanner)
        this->pc->pName->setText(this->name->text());
        this->pc->pMissionCount->setText(this->missionCount->text());
        this->pc->pDate->setText(this->date->text());
        this->pc->pTime->setText(this->time->text());
        this->pc->pSensors->setText(this->sensors->text());


        //Clean up loaded missions
        if (this->pc->loadedMission)
            this->pc->loadedMission->mission->notes = this->pc->missionNotes->toPlainText();
        this->pc->loadedMission = nullptr;
        this->pc->missionNotes->clear();
        QTreeWidgetItemIterator it(this->pc->missionList);
        while (*it) {
            delete (*it);
        }
        this->pc->missionList->clear();

        for (Mission *m: *p4d->missions) {

            MissionWidget* mw = new MissionWidget(m, this->parent);

            QTreeWidgetItem* qtw = new QTreeWidgetItem;
            this->pc->missionList->addTopLevelItem(qtw);
            this->pc->missionList->setItemWidget(qtw,0,mw);

        }
    }
    
    //pre-load first mission widget _DO LATER!
    //QTreeWidgetItem* qtw = this->pc->missionList->topLevelItem(0);
    //mission_info(qtw, 0); //why the 0 column? Eliminate?

}
