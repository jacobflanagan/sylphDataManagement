#include "missionwidget.h"
#include "ui_missionwidget.h"

#include <QFileDialog>
#include <QMessageBox>

#include "exif.h"

//#include <exiv2/exiv2.hpp>
#include <libexif/exif-data.h>

MissionWidget::MissionWidget(Mission *mission, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MissionWidget)
{
    ui->setupUi(this);

    pThread = new PairThread(QDir("C:/"), ui->progressBar, ui->pairButton); //dummy thread setup, a given directory maybe problematic across different OSs

    QString name = mission->name;
    ui->name->setText(mission->name);
    this->name = ui->name; //Important
    this->mission = mission;
    setFlightTime(MissionStatistics::flightTime(mission->flightDetails));
    setAltitude(MissionStatistics::altitude(mission->flightDetails));
    setDateTime(MissionStatistics::executedAt(mission->flightDetails));
    setCaptureDevice(MissionStatistics::captureDevice(mission->flightDetails));

    ui->progressBar->setValue(this->mission->imageSyncRatio() * 100);
    
    //set the default color here for referncing later
    this->defaultPairButton = ui->pairButton->palette();

    //Connect ui button to load function
    QObject::connect( ui->syncButton, SIGNAL(clicked()),
                     this, SLOT(sync_data()) );
    //Connect ui button to function
    QObject::connect( ui->pairButton, SIGNAL(clicked()),
                     this, SLOT(pair_data()) );

    //Connect ui button to function
    QObject::connect( ui->deleteButton, SIGNAL(clicked()),
                     this, SLOT(delete_self()) );
}

MissionWidget::~MissionWidget()
{
    delete this->mission;
    delete ui;
}

void MissionWidget::setFlightTime(double seconds)
{
    QTime fd(0,0,0);
    fd = fd.addMSecs(static_cast<int>(seconds*1000));
    ui->label_flightTime->setText( QString().sprintf("%d' %d\"", fd.hour()*60+fd.minute(), fd.second()) );
}

void MissionWidget::setDateTime(QDateTime dt)
{
    QString tz = dt.timeZone().displayName(dt,QTimeZone::OffsetName);
    ui->label_dateTime->setText(dt.toString("<b>yyyy.MM.dd</b> HH:mm:ss"));
    //ui->label_time->setText("<b>Time:</b> " + dt.toString("HH:mm:ss"));
}

void MissionWidget::setAltitude(double altitude)
{
    ui->label_altitude->setText( QString().sprintf("%.0fm AGL", altitude ) );
}

void MissionWidget::setCaptureDevice(QString sensor)
{
    ui->label_sensors->setText("" + sensor);
}

bool MissionWidget::clearMissionTab()
{
    return false;
}

//Assuming the time
qint64 image2epoch(QString path, int tz_offset){

    FILE *fp = fopen(path.toUtf8(), "rb");
    if (!fp) {
        qInfo("Can't open file.");
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    unsigned long fsize = ftell(fp);
    rewind(fp);
    unsigned char *buf = new unsigned char[fsize];
    if (fread(buf, 1, fsize, fp) != fsize) {
        printf("Can't read file.\n");
        delete[] buf;
        return 0;
    }
    fclose(fp);

    // Parse EXIF
    easyexif::EXIFInfo result;
    int code = result.parseFrom(buf, fsize);
    delete[] buf;
    if (code) {
        qInfo("Error parsing EXIF: code %d\n", code);
        return 0;
    }

    //convert string time to QDateTime, then to epoch
    //qInfo(result.DateTime.c_str());
    QStringList dt_str = QString(result.DateTime.c_str()).split(" "); //Date and time split
    QStringList d_str = dt_str[0].split(":");
    QStringList t_str = dt_str[1].split(":");
    QDateTime dt;
    dt.setOffsetFromUtc(tz_offset);
    dt = QDateTime::fromString(result.DateTime.c_str(),"yyyy:MM:dd HH:mm:ss");

    //QDateTime dt( QDate(d_str[0].toInt(),d_str[1].toInt(),d_str[2].toInt()), QTime(t_str[0].toInt(),t_str[1].toInt(),t_str[2].toInt()), QTimeZone(tz_offset) );

    //qInfo("DT: " + dt.toString("yyyy-MM-dd HH:mm:ss t").toUtf8());
    return dt.toSecsSinceEpoch();

}

//setting max_depth <= 0 will prevent recursion
QStringList MissionWidget::glob(QString directory, QStringList filters, int max_depth=0, int depth=0, qint64 start_time=0, qint64 end_time=0, int tz_offset = 0){

    QStringList files;

    QDir dir(directory);

    QFileInfoList fil = dir.entryInfoList(filters, QDir::NoDotAndDotDot|QDir::AllDirs|QDir::Files);

    for (int i = 0; i < fil.size(); i++)
    {
        QFileInfo fi = fil.at(i);
        if (fi.isDir() and (depth < max_depth)){
            files += glob( fi.absoluteFilePath(), filters, max_depth, depth+1, start_time, end_time);
        }
        if (fi.isFile()){
            if (start_time < end_time){
                //test if photo falls within the time
                qint64 et;
                if (fi.absoluteFilePath().contains( "TIF", Qt::CaseSensitivity::CaseInsensitive )){

                    //skip tif for now
                    continue;

                    et = image2epoch(fi.absoluteFilePath(), 0); //UTC for RedEdge testing
                    qInfo ("Found TIF, ET: %d", et);
                }
                else
                    et = image2epoch(fi.absoluteFilePath(), tz_offset);

                if (et < end_time and et > start_time)
                    files.append(fi.absoluteFilePath());
            }
            else{ //case where time isn't considered
                files.append(fi.absoluteFilePath());
            }
        }
    }

    return files;

}


/* Remove spaces on the right of the string */
static void trim_spaces(char *buf)
{
    char *s = buf-1;
    for (; *buf; ++buf) {
        if (*buf != ' ')
            s = buf;
    }
    *++s = 0; /* nul terminate the string on the first of the final spaces */
}

/* Show the tag name and contents if the tag exists */
static void show_tag(ExifData *d, ExifIfd ifd, ExifTag tag)
{
    /* See if this tag exists */
    ExifEntry *entry = exif_content_get_entry(d->ifd[ifd],tag);
    if (entry) {
        char buf[1024];

        /* Get the contents of the tag in human-readable form */
        exif_entry_get_value(entry, buf, sizeof(buf));

        /* Don't bother printing it if it's entirely blank */
        trim_spaces(buf);
        if (*buf) {
            qInfo("Tag Info - %s: %s\n", exif_tag_get_name_in_ifd(tag,ifd), buf);
        }
        else
            qInfo("Buffer Issue");
    }
    else
        qInfo("entry Issue");
}

void MissionWidget::sync_data(){

    ExifData *ed;
    //ed = exif_data_new_from_file("C:/Projects/CHI/Image Dump/20191303_Sensor/DJI_0001.jpg");
    ed = exif_data_new_from_file("C:/Projects/CHI/Image Dump/20191303_Altum/0002SET/001/IMG_0200_1.tif");

    if (ed)
        show_tag(ed, EXIF_IFD_0, EXIF_TAG_DATE_TIME);
    EXIF_DATA_OPTION_FOLLOW_SPECIFICATION;
    EXIF_DATA_OPTION_IGNORE_UNKNOWN_TAGS;

    //Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open("C:/Projects/CHI/Image Dump/20191303_Sensor/DJI_0001.jpg");


//    QString col = "List:";

//    QMessageBox msgBox;
//    msgBox.setText(col);
//    msgBox.exec();
//    return;

    //1. Look in a common directory (i.e., SD Card)
    //2. Look in data dump directory (set in settings)
    //3. Ask user for specific directory

    //3.
    QString dirname = QFileDialog::getExistingDirectory(this, tr("Manually Locate Data"),
                     "C:/",
                     QFileDialog::ShowDirsOnly
                     | QFileDialog::DontResolveSymlinks);

    QDir directory(dirname);

    //Filters
    QStringList filters;
    filters << "*.JPG";
    filters << "*.jpg";
    filters << "*.TIF";
    filters << "*.tif";

    auto bdt = MissionStatistics::executedAt(mission->flightDetails);
    qint64 btime = bdt.toSecsSinceEpoch();

    QStringList imgs = glob( dirname, filters, 5, 0, btime, btime + MissionStatistics::flightTime(mission->flightDetails), bdt.offsetFromUtc() ); //max depth of 5
    //from my understanding of QMap, it's probably "bestest" to iterate the QStringList and map it instead of writing a recursive glob map function
    //Issue with case, so coverting all keys to uppercase (make sure to search in this same way)
    QMap<QString, QString> img_map;
    qInfo("TZ Offset from UTC:  %d",bdt.offsetFromUtc());
    qInfo("Outputting Files:");
    for (auto item: imgs){

        item.replace("\\","/");
        img_map[item.split("/").back().toUpper().toUtf8()] = item;

    }

    qInfo("Size of List: %d", imgs.size() );

    for (auto item: img_map.keys()){
        if (item.contains("TIF"))
            qInfo("Key: " + item.toUtf8() + " Value: " + img_map.value(item).toUtf8() );
    }

    for (auto item: this->mission->images->keys()){
        qInfo("Image Name: " + item.toUtf8() + " with state: %d", this->mission->images->value(item)->state());
        if (this->mission->images->value(item)->state() == Image::STATE::PROCESSED && img_map.contains(item.toUpper())) {
            qInfo( "Copying: " + img_map.value(item.toUpper()).toUtf8() + " to " + this->mission->images->value(item)->directory.absoluteFilePath(item).toUtf8());
            qInfo("Epoch Time: %d", image2epoch(img_map.value(item.toUpper()),0));

            QFile::copy(img_map.value(item.toUpper()),this->mission->images->value(item)->directory.absoluteFilePath(item));

            //Useful for making sure file exists in destination
            FILE *fp = fopen(this->mission->images->value(item)->directory.absoluteFilePath(item).toUtf8(), "rb");
            qInfo("Image path: " + this->mission->images->value(item)->directory.absoluteFilePath(item).toUtf8());
            if (!fp) {
                qInfo("Can't open file.");
                continue;
            }

            fseek(fp, 0, SEEK_END);
            unsigned long fsize = ftell(fp);
            rewind(fp);
            unsigned char *buf = new unsigned char[fsize];
            if (fread(buf, 1, fsize, fp) != fsize) {
                printf("Can't read file.\n");
                delete[] buf;
                continue;
            }
            fclose(fp);

            // Parse EXIF
            easyexif::EXIFInfo result;
            int code = result.parseFrom(buf, fsize);
            delete[] buf;
            if (code) {
                qInfo("Error parsing EXIF: code %d\n", code);
                continue;
            }

            qInfo("Camera make          : %s", result.Make.c_str());
            qInfo("Camera Model         : %s", result.Model.c_str());
            qInfo("Image date/time      : %s", result.DateTime.c_str());
            qInfo("Original date/time   : %s", result.DateTimeOriginal.c_str());
            qInfo("Digitize date/time   : %s", result.DateTimeDigitized.c_str());


            //QFile::copy(this->mission->images->value(item)->directory.absoluteFilePath());
        }
    }

    this->mission->p4d;

    return;

//    for (auto k: this->mission->images->keys()) {
//        qInfo( this->mission->images->value(k)->name.toUtf8() );
//        nameFilter.append(this->mission->images->value(k)->name);
//        //directory.en
//    }

}

double try_stripping(QString str){

    if (str.contains("[") and str.contains("]") and str.contains("/")){
        QStringList progressParts = str.split("[").back().split("]").front().split("/");
        return progressParts[0].toDouble()/progressParts[1].toDouble();
    }

    return -1;
}

QStringList PairThread::exiftool_exe(QDir directory, QProgressBar *pb = nullptr, QString executable = "C:/Exiftool/exiftool(-k).exe"){

    QProcess *exiftool = new QProcess(); //this); //add this to a parent function (won't work from main)

    QStringList arguments;

    //recursively find images (-r paramenter)
    arguments << "-progress" << "-r" << "-p" << "$directory/${filename},${createdate},$make,$model" << directory.absolutePath();
    //arguments << "-progress" << "-f" << "-r" << "-p" << "'$directory/${filename},${createdate},$make,$model'" << d.absolutePath() << "-csv" << "-o" << d.absolutePath()+"/sylph.csv";
    //qInfo(arguments.join(" ").toUtf8());

    QStringList result;
    exiftool->start(executable, arguments);

    if (exiftool->waitForStarted()){

        double p = 0;
        while(exiftool->waitForReadyRead(-1)) {

            //qInfo(exiftool->readAll());

            QString O = exiftool->readAllStandardOutput();
            QString O2 = exiftool->readAllStandardError();

            if (O.length() > 0)
                result.append( O.split("\n") );

            p = try_stripping( O2 );
            if (p >= 0){
                qInfo("%.2f",p*100);
                if (pb and false)
                    pb->setValue(p*100);
                emit progressBar(p*100);
            }

            //kill if requested to stop this thread
            if ( this->stop ){
                exiftool->kill();
                return result;
            }
        }
    }

    if (!exiftool->waitForFinished())
        qInfo("Didn't wait for FINISH.");

    result.append( exiftool->readAll() );
    //QStringList r_split = result.split("\n");

    //QByteArray result = exiftool->readAll();

    //qInfo( "Result: " + result[result.length()-1].toUtf8() );

    return result;
}



PairThread::PairThread(QDir directory, QProgressBar *pb, QToolButton *b)
{
    this->pb = pb;
    this->b = b;
    this->directory = directory;
    this->stop = true;
}

void PairThread::run() {
    QString result;
    /* ... here is the expensive or blocking operation ... */
    //b->setEnabled(false);
    this->stop = false;
    exiftool_exe( this->directory.absolutePath(), pb ); //QDir("C:/Projects/CHI/Image Dump/20191303_Altum/0002SET/000"),pb);
    emit resultReady(result);
    //b->setEnabled(true);
}

void PairThread::quit() {
    this->stop = true;
}

bool PairThread::isStopped(){
    return this->stop;
}

void MissionWidget::pair_data(){

    if (!pThread->isStopped()){
        this->pThread->quit();
        return;
    }

    QString dirname = QFileDialog::getExistingDirectory(this, tr("Manually Locate Data"),
                     "C:/",
                     QFileDialog::ShowDirsOnly
                     | QFileDialog::DontResolveSymlinks);

    if (dirname.isEmpty())
        return;

    QDir directory(dirname);

    //exiftool_exe(QDir("C:/Projects/CHI/Image Dump/20191303_Altum/0002SET/000"),ui->progressBar);
    //ui->pairButton->setDisabled(true); Disabled this becasue the pair button is now a "stop" button if this thread is running
    auto pal = this->defaultPairButton;
    //pal.setColor(QPalette::Button, QColor(Qt::blue));
    //ui->syncButton->setAutoFillBackground(true);
    //ui->syncButton->setPalette(pal);
    //ui->syncButton->update();
    
    //ui->syncButton->setDisabled(true);
    this->pThread = new PairThread(directory, ui->progressBar, ui->pairButton);
    connect(pThread, &PairThread::resultReady, this, &MissionWidget::enablePairButton);
    connect(pThread, &PairThread::progressBar, this, &MissionWidget::updateProgressBar);
    connect(pThread, &PairThread::finished, pThread, &QObject::deleteLater);
    this->pThread->start();

}

void MissionWidget::updateProgressBar(int p){
    ui->progressBar->setValue(p);
    this->update();
}

void MissionWidget::enablePairButton(QString s){
    ui->pairButton->setEnabled(true);
    this->update();
}

bool removeDir(const QString & dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists()) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = QDir().rmdir(dirName);
    }
    return result;
}

void MissionWidget::delete_self(){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete Mission", "Are you sure you would like to delete this mission?",
                                    QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        qDebug() << "Yes was clicked";
        //delete directory and contents
        //removeDir(this->mission->directory.absolutePath());
    } else {
        qDebug() << "Yes was *not* clicked";
    }
}
