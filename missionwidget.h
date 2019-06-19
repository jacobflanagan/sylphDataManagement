#ifndef MISSIONWIDGET_H
#define MISSIONWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QQuickWidget>
#include <QProgressBar>
#include <QToolButton>
#include "p4dproject.h"

namespace Ui {
class MissionWidget;
}

class PairThread : public QThread
{
    Q_OBJECT
private:
    bool stop;

public:
    PairThread(QDir directory, QProgressBar *pb, QToolButton *b);
    //~PairThread();

    QProgressBar *pb;
    QToolButton *b;
    QDir directory;

    bool isStopped();

    void run() override;
    void quit();

    QStringList exiftool_exe(QDir d, QProgressBar *pb, QString executable);

signals:
    void resultReady(const QString &s);
    void progressBar(int p);
};

//class SyncThread : public QThread
//{
//    Q_OBJECT
//private:
//    bool stop;

//public:
//    SyncThread(QProgressBar *pb, QToolButton *b);
//    //~PairThread();

//    QProgressBar *pb;
//    QToolButton *b;

//    bool isStopped();

//    void run() override;
//    void quit();

//    QStringList exiftool_exe(QDir d, QProgressBar *pb, QString executable);

//signals:
//    void resultReady(const QString &s);
//    void progressBar(int p);
//};

//UI elements MissionWidget class needs to control
class MissionControl
{
public:
    QQuickWidget *missionMap;
    QTextEdit *missionNotes;
};

class MissionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MissionWidget(Mission *mission, QWidget *parent = nullptr);
    ~MissionWidget();

    QLabel *name;
    Mission *mission;

    PairThread *pThread;
    QPalette defaultPairButton;

    void setFlightTime(double seconds);
    void setDateTime(QDateTime dt);
    void setAltitude(double altitude);
    void setCaptureDevice(QString sensor);

    bool clearMissionTab();
    static QStringList glob(QString directory, QStringList filter, int max_depth, int depth, qint64 start_time, qint64 end_time, int tz_offset);

    void enablePairButton(QString);
    void updateProgressBar(int p);

private slots:
    void sync_data();
    void pair_data();
    void delete_self();

private:
    Ui::MissionWidget *ui;
};

#endif // MISSIONWIDGET_H
