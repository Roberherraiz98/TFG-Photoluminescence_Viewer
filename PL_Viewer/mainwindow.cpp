#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "VimbaCPP/Examples/ListCameras/Source/ListCameras.h"
#include "listfeatures.h"
#include "VimbaCPP/Include/VimbaCPP.h"
#include "VimbaCPP/Examples/Common/StreamSystemInfo.h"
#include "VimbaCPP/Examples/Common/ErrorCodeToMessage.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Photoluminescence Viewer");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_boton_clicked()//ListCameras
{
    ui->ventana->setText(QString::fromStdString(AVT::VmbAPI::Examples::ListCameras::Print()));
}

void MainWindow::on_botonsynchro_clicked()
{

    SynchronousGrab *ventanasynchronousgrab = new SynchronousGrab(this);
    ventanasynchronousgrab->show();
}

void MainWindow::on_botonasynchro_clicked()
{
    AsynchronousGrab *ventanaasynchronousgrab = new AsynchronousGrab(this);
    ventanaasynchronousgrab->show();
}

void MainWindow::on_botonexposuretime_clicked()
{
    ChangeFeaturesWindow *ventanachangefeatures = new ChangeFeaturesWindow();
    ventanachangefeatures->show();
}

void MainWindow::on_botonretard_clicked()
{
    RetardTime *ventanaretardtime = new RetardTime;
    ventanaretardtime->show();
}

void MainWindow::on_botonpwm_clicked()
{
    pwmduration *ventanapwmduration = new pwmduration;
    ventanapwmduration->show();
}
