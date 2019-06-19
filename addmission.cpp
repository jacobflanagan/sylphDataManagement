#include "addmission.h"
#include "ui_addmission.h"

addmission::addmission(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::addmission)
{
    ui->setupUi(this);
}

addmission::~addmission()
{
    delete ui;
}
