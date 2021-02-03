#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <synchronousgrab.h>
#include <changefeatureswindow.h>
#include <changefeatures.h>
#include <asynchronousgrab.h>
#include <retardtime.h>
#include <pwmduration.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_boton_clicked();

    void on_botonsynchro_clicked();

    void on_botonasynchro_clicked();

    void on_botonexposuretime_clicked();

    void on_botonretard_clicked();

    void on_botonpwm_clicked();

protected:
    Ui::MainWindow *ui;

    SynchronousGrab *ventanasynchronousgrab;
	AsynchronousGrab *ventanaasynchronousgrab;
	ChangeFeaturesWindow *ventanachangefeatures;
	RetardTime *ventanaretardtime;
	pwmduration *ventanapwmduration;

};

#endif // MAINWINDOW_H
