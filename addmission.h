#ifndef ADDMISSION_H
#define ADDMISSION_H

#include <QWidget>

namespace Ui {
class addmission;
}

class addmission : public QWidget
{
    Q_OBJECT

public:
    explicit addmission(QWidget *parent = nullptr);
    ~addmission();

private:
    Ui::addmission *ui;
};

#endif // ADDMISSION_H
