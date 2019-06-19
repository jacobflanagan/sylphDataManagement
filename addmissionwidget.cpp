#include "addmissionwidget.h"
#include "ui_addmissionwidget.h"

addmissionwidget::addmissionwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::addmissionwidget)
{
    ui->setupUi(this);

    this->name = ui->name;
}

addmissionwidget::~addmissionwidget()
{
    delete ui;
}
