#include "changefeatureswindow.h"
#include "ui_changefeatureswindow.h"

ChangeFeaturesWindow::ChangeFeaturesWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeFeaturesWindow)
{
    ui->setupUi(this);
    setWindowTitle("Configure Exposure Time");

    ui->comboBox->addItem("ms");
    ui->comboBox->addItem("us");
}

ChangeFeaturesWindow::~ChangeFeaturesWindow()
{
    delete ui;
}

void ChangeFeaturesWindow::on_boton_ok_clicked()
{
    QString valor_texto = ui->valor->text();//Grants access to the value the user has just entered
    double valor = valor_texto.toDouble();//transform a QString variable into a float variable


    if(variableunidad == 1){ //if user selects microseconds

         ui->texto_resultado->setText(QString::fromStdString(AVT::VmbAPI::Examples::ChangeFeatures::Print("", "ExposureTime", valor)));
    }
    else{ //if user selects miliseconds

        ui->texto_resultado->setText(QString::fromStdString(AVT::VmbAPI::Examples::ChangeFeatures::Print("", "ExposureTime", valor*1000)));
    }
}


void ChangeFeaturesWindow::on_comboBox_activated(const QString &arg1)
{
    if(arg1 == QString::fromStdString("us")){ //if user selects microseconds

        variableunidad = 1;
    }
    else { //if user selects miliseconds

        variableunidad = 0;
    }
}
