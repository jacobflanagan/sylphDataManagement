#ifndef NEWMISSIONDIALOG_H
#define NEWMISSIONDIALOG_H

#include <QDialog>

#include "missionwidget.h"

class MissionProperties {
public:
    QStringList images;

    QDateTime start;
    QDateTime end;
    double seconds;
    QString sensor;
    double Altitude;

    QList<double> lats;
    QList<double> lngs;
    QList<double> altitudes;
    QList<double> times;

    double location_min[2];
    double location_max[2];

    QString name;

};

namespace Ui {
class NewMissionDialog;
}

class NewMissionDialog : public QDialog
{
    Q_OBJECT

public:
    MissionProperties *data;
    MissionProperties* getData();

    explicit NewMissionDialog(QWidget *parent = nullptr);
    ~NewMissionDialog();

    QStringList glob(QString directory, QStringList filters, int max_depth = 0, int depth = 0, qint64 start_time = 0, qint64 end_time = 0, int tz_offset = 0);

private slots:
    void dirSelect();
    void imageSearch();
    void removeSelected();
    void clearImageList();

private:
    Ui::NewMissionDialog *ui;
};

#endif // NEWMISSIONDIALOG_H
