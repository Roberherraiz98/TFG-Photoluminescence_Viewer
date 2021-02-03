#ifndef PWMDURATION_H
#define PWMDURATION_H

#include <QDialog>
#include <wiringPi.h>
#include <softPwm.h>
#include "asynchronousgrab.h"

namespace Ui {
class pwmduration;
}

class pwmduration : public QDialog
{
    Q_OBJECT

public:
    explicit pwmduration(QWidget *parent = 0);
    ~pwmduration();

private slots:
    void on_boton_ok_clicked();

private:
    Ui::pwmduration *ui;
};

#endif // PWMDURATION_H
