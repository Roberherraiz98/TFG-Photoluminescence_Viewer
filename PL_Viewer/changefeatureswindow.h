#ifndef CHANGEFEATURESWINDOW_H
#define CHANGEFEATURESWINDOW_H

#include <QDialog>
#include "changefeatures.h"



namespace Ui {
class ChangeFeaturesWindow;
}

class ChangeFeaturesWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeFeaturesWindow(QWidget *parent = 0);
    ~ChangeFeaturesWindow();
    bool variableunidad =0;

private slots:
    void on_boton_ok_clicked();

    void on_comboBox_activated(const QString &arg1);

private:
    Ui::ChangeFeaturesWindow *ui;
};

#endif // CHANGEFEATURESWINDOW_H
