#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

QString NewProjectDialog::getData()
{
    return ui->lineEdit->text();
}

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);
}

NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}
