#include "newmissiondialog.h"
#include "ui_newmissiondialog.h"

#include "exif.h"
#include "missionwidget.h"

#include <qfiledialog.h>

NewMissionDialog::NewMissionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewMissionDialog)
{
    ui->setupUi(this);

    this->data = new MissionProperties();

    //Connect ui Project Tree Widget to info function
    QObject::connect(ui->toolButton_dirSelect, SIGNAL(clicked()),
                     this, SLOT(dirSelect()));

    //Connect ui Project Tree Widget to info function
    QObject::connect(ui->pushButton_searchImages, SIGNAL(clicked()),
                     this, SLOT(imageSearch()));

    //Connect ui Project Tree Widget to info function
    QObject::connect(ui->pushButton_removeSelected, SIGNAL(clicked()),
                     this, SLOT(removeSelected()));

    //Connect ui Project Tree Widget to info function
    QObject::connect(ui->pushButton_clear, SIGNAL(clicked()),
                     this, SLOT(clearImageList()));
}

NewMissionDialog::~NewMissionDialog()
{
    delete ui;
}

void NewMissionDialog::dirSelect(){

    QString dirname = QFileDialog::getExistingDirectory(this, tr("Select Image Directory"),
                                                 QDir::fromNativeSeparators(ui->text_searchDir->text()),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if (dirname.isEmpty()){ return; } //User cancelled

    ui->text_searchDir->setText(QDir::toNativeSeparators(dirname));

    return;

}

//look over later (2019-06-06)
easyexif::EXIFInfo readexif(QString path){

    easyexif::EXIFInfo result;

    FILE *fp = fopen(path.toUtf8(), "rb");
    if (!fp) {
        qInfo("Can't open file.");
        return result;
    }
    fseek(fp, 0, SEEK_END);
    unsigned long fsize = ftell(fp);
    rewind(fp);
    unsigned char *buf = new unsigned char[fsize];
    if (fread(buf, 1, fsize, fp) != fsize) {
        printf("Can't read file.\n");
        delete[] buf;
        return result;
    }
    fclose(fp);

    // Parse EXIF
    //easyexif::EXIFInfo result;
    int code = result.parseFrom(buf, fsize);
    delete[] buf;
    if (code) {
        qInfo("Error parsing EXIF: code %d\n", code);
        return result;
    }

    //convert string time to QDateTime, then to epoch
    //qInfo(result.DateTime.c_str());
    QStringList dt_str = QString(result.DateTime.c_str()).split(" "); //Date and time split
    QStringList d_str = dt_str[0].split(":");
    QStringList t_str = dt_str[1].split(":");
    QDateTime dt;

    //dt.setOffsetFromUtc(tz_offset); //implement offset later

    dt = QDateTime::fromString(result.DateTime.c_str(),"yyyy:MM:dd HH:mm:ss");

    //QDateTime dt( QDate(d_str[0].toInt(),d_str[1].toInt(),d_str[2].toInt()), QTime(t_str[0].toInt(),t_str[1].toInt(),t_str[2].toInt()), QTimeZone(tz_offset) );

    //qInfo("DT: " + dt.toString("yyyy-MM-dd HH:mm:ss t").toUtf8());
    return result;

}

void NewMissionDialog::imageSearch()
{
    QString rootDir = ui->text_searchDir->text();

    //Filters
    QStringList filters;
    filters << "*.JPG";
    filters << "*.jpg";
    filters << "*.TIF";
    filters << "*.tif";

    //depth, should change to use that in settings
    QStringList imgs = MissionWidget::glob( rootDir, filters, 5, 0, 0, 0, 0 ); //max depth of 5

    MissionProperties *mp = this->data; //new MissionProperties();

    //init date extents
    mp->start.setSecsSinceEpoch(UINT_MAX);
    mp->end.setSecsSinceEpoch(0);

    //init location extents
    mp->location_max[0] = -DBL_MAX;
    mp->location_max[1] = -DBL_MAX;
    mp->location_min[0] = DBL_MAX;
    mp->location_min[1] = DBL_MAX;

    mp->Altitude = 0;
    int i = 0;
    for (auto item: imgs){

        auto exif = readexif(item);
        //qInfo(exif.DateTime.c_str());

        auto iDateTime = QDateTime::fromString(exif.DateTime.c_str(),"yyyy:MM:dd HH:mm:ss");
        if (iDateTime < mp->start)
            mp->start = iDateTime;
        if (iDateTime > mp->end)
            mp->end = iDateTime;

        //determine max and min location (too simple)
        auto lat = exif.GeoLocation.Latitude;
        if (lat < mp->location_min[0]) mp->location_min[0] = lat;
        if (lat > mp->location_max[0]) mp->location_max[0] = lat;
        auto lon = exif.GeoLocation.Longitude;
        if (lon < mp->location_min[1]) mp->location_min[1] = lon;
        if (lon > mp->location_max[1]) mp->location_max[1] = lon;

        //sum for average altitude
        mp->Altitude += exif.GeoLocation.Altitude;

        //sensor - just grab last
        mp->sensor = QString::fromStdString(exif.Model);

        //add to mp - lats, lngs, altitude and time
        mp->altitudes.append(exif.GeoLocation.Altitude);
        mp->lats.append(exif.GeoLocation.Latitude);
        mp->lngs.append(exif.GeoLocation.Longitude);
        mp->times.append(QDateTime::fromString(QString::fromStdString(exif.DateTime),"yyyy:MM:dd HH:mm:ss").toSecsSinceEpoch());

        QTreeWidgetItem *tw = new QTreeWidgetItem(ui->treeWidget_imageList);
        tw->setText(2,item); //column 2
        tw->setText(0,exif.DateTime.c_str());
        tw->setText(1,QString(exif.Make.c_str()) + " " + QString(exif.Model.c_str()));
        tw->setText(3,QString::number(exif.GeoLocation.Latitude)+", "+QString::number(exif.GeoLocation.Longitude));
        i++;
    }
    //average altitude
    mp->Altitude = mp->Altitude/imgs.length();


    this->data->images.clear();
    this->data->images = imgs;

    //resize contents to all 0 column items
    for(int i = 0; i < ui->treeWidget_imageList->topLevelItemCount(); i++)
        ui->treeWidget_imageList->resizeColumnToContents(i);

    //mp->seconds = mp->start. - mp->end

}

void NewMissionDialog::removeSelected()
{
    //assuming "this" is the treeview
    while(!ui->treeWidget_imageList->selectedItems().isEmpty()) {
        auto idx = ui->treeWidget_imageList->selectedItems().first();
        //ui->treeWidget_imageList->removeItemWidget(idx,0);
        delete idx;
    }
}

void NewMissionDialog::clearImageList()
{
    ui->treeWidget_imageList->clear();
}

MissionProperties* NewMissionDialog::getData()
{
    data->name = ui->lineEdit->text();
    return this->data;
}
