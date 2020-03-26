#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "client.h"
#include "controller.h"

#include <QWidget>
#include <QElapsedTimer>
#include <QTimer>
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT
    Client * client =nullptr;
    Controller * controller = nullptr;
    QTimer* timer =nullptr;

    const QString _ip;
    float _power;

    int _SP1;
    int _SP2;
    int _SP3;
    int _SP4;

   // float controlSP = 0;

    bool LVclicked = false;
    bool SPset = false;

public:
    explicit MainWindow(QString _ip,QWidget *parent = nullptr);
    ~MainWindow();

public slots:

    //1_LV_Voltage
    void readLVvoltge();

    //2_Power_Setpoint
    void setPowerSP();

    //3_Control
    void setSP();
    void startTimer();
    void stopTimer();
    void controlLoop();

    //4_Any_Request
    void anyRequest();

    //5_Response_Management
    void readResponse(quint8 TID1, quint8 TID2, float data);
    void writeResponse(quint8 TID1, quint8 TID2, bool status);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
