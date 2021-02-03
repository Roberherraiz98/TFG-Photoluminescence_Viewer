#ifndef RETARDTIME_H
#define RETARDTIME_H

#include <QDialog>
#include "asynchronousgrab.h"

namespace Ui {
class RetardTime;
}

class RetardTime : public QDialog
{
    Q_OBJECT

public:
    explicit RetardTime(QWidget *parent = 0);
    ~RetardTime();

private slots:
    void on_boton_ok_clicked();

private:
    Ui::RetardTime *ui;
};

#endif // RETARDTIME_H
