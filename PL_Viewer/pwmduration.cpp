#include "pwmduration.h"
#include "ui_pwmduration.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include <thread>
#include <iostream>
#include <string>

#include <wiringPi.h>
#include <softPwm.h>

pwmduration::pwmduration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::pwmduration)
{
    ui->setupUi(this);
    setWindowTitle("Configure PWM duration");

}

pwmduration::~pwmduration()
{
    delete ui;
}

void task1(std::string msg, float num){

    delay(num);
    std::cout << msg << std::endl;
    delay(num);
}

void pwmduration::on_boton_ok_clicked()
{
    QString valor_texto = ui->valor->text();//Grants access to the value the user has just entered
    int valor = valor_texto.toInt();//transform a QString variable into a int variable
    AsynchronousGrab a;
    ui->texto_resultado->setText(QString::fromStdString(a.SetPWMDuration(valor)));

}


