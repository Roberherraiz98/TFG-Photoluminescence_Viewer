#include "retardtime.h"
#include "ui_retardtime.h"

RetardTime::RetardTime(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RetardTime)
{
    ui->setupUi(this);
    setWindowTitle("Configure Retard Time");
}

RetardTime::~RetardTime()
{
    delete ui;
}

void RetardTime::on_boton_ok_clicked()
{
    QString valor_texto = ui->valor->text();//Grants access to the value the user has just entered
    int valor = valor_texto.toInt();//transform a QString variable into a int variable
    AsynchronousGrab a;
    ui->texto_resultado->setText(QString::fromStdString(a.SetRetardTime(valor)));

}
