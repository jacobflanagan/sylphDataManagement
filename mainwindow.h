#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once

#include <QMainWindow>
#include <QtQuickWidgets/QQuickWidget>
#include <QtQml/QQmlApplicationEngine>
#include <QtQuick/QQuickView>
#include <QtWidgets/QFileDialog>
#include <QtCore/QDir>
#include <QComboBox>
#include <QGeoCoordinate>

#include "projectwidget.h"
#include "missionwidget.h"
#include "p4dproject.h"
#include "igisproject.h"
#include "maptools.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //explicit MainWindow(QQuickView *view, QWidget *parent = nullptr);
    explicit MainWindow(QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *event);
    void writeSettings();
    void readSettings();
    ~MainWindow();

    QComboBox *combo_project_type;

    P4dProject *p4dProject;

    MissionWidget* loaded = nullptr;

    ProjectControl* pc = nullptr;
    MissionControl* mc = nullptr;

    void init_IGISXML(QDir absolutePath, QString state);

private slots:
    void new_project();
    void new_mission();
    void load_projects();
    void load_missions();
    void project_info(QTreeWidgetItem *w, int n);
    void mission_info(QTreeWidgetItem *w, int n);

    //void add_mission();

    void syncButtonClicked();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
