#ifndef ADDMISSIONWIDGET_H
#define ADDMISSIONWIDGET_H

#include <QLabel>
#include <QWidget>

namespace Ui {
class addmissionwidget;
}

class addmissionwidget : public QWidget
{
    Q_OBJECT

public:

    QLabel *name;

    explicit addmissionwidget(QWidget *parent = nullptr);
    ~addmissionwidget();

private:
    Ui::addmissionwidget *ui;
};

#endif // ADDMISSIONWIDGET_H
