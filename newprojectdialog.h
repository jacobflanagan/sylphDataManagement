#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>
#include <QLineEdit>

namespace Ui {
class NewProjectDialog;
}

class NewProjectDialog : public QDialog
{
    Q_OBJECT

public:
    QString getData();

    explicit NewProjectDialog(QWidget *parent = nullptr);
    ~NewProjectDialog();

private:
    Ui::NewProjectDialog *ui;
};

#endif // NEWPROJECTDIALOG_H
