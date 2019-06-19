#ifndef PROJECTWIDGET_H
#define PROJECTWIDGET_H

#pragma once

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QDir>
#include <QMessageBox>
#include <QListWidget>
#include <QTreeWidget>
#include <QQuickWidget>
#include <QTextBrowser>

#include "p4dproject.h"
#include "missionwidget.h"
//#include "mainwindow.h"

namespace Ui {
    class ProjectWidget;
}

//UI elements ProjectWidget class needs to control, inherits from mission control
class ProjectControl : public MissionControl
{
public:
    QQuickWidget *projectMap;
    QTextBrowser *pDescription;
    QTreeWidget *missionList;

    //Other elements
    QLabel *pName;
    QLabel *pMissionCount;
    QLabel *pDate;
    QLabel *pTime;
    QLabel *pSensors;

    MissionWidget *loadedMission = nullptr;
};

class ProjectWidget : public QWidget
{
    Q_OBJECT

public:
    ProjectWidget(QDir *directory = nullptr, ProjectControl *pc = nullptr, QWidget *parent = nullptr);
    ~ProjectWidget();

    //Write the "Set" functions here

    QLabel* name;
    QLabel* missionCount;
    QLabel* date;
    QLabel* time;
    QLabel* sensors;

    QString cMethod;
    QString projType;

    ProjectStatistics *ps;

    QDir* directory;
    QWidget* parent;
    ProjectControl *pc;

    //Dirty Stuff
    MissionWidget* parentLoaded; //monitor the loaded mission
    QWidget* MissionTab;

    void setMissionCount(int count, double seconds);
    void setCreationDateTime(QDateTime *dt);
    void setSensors(QStringList sList);

    QString setProjType(QDir d);
    void mission_info(QTreeWidgetItem *w, int n);
    
private slots:
    void load_missions();

private:
    Ui::ProjectWidget *ui;
};

#endif // PROJECTWIDGET_H
