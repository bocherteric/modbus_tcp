#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "client.h"
#include "controller.h"

#include <QWidget>
#include <QElapsedTimer>
#include <QTimer>


namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT
    Client * client =nullptr;
    Controller * controller = nullptr;
    QTimer* timer =nullptr;
    QElapsedTimer * timer2 = nullptr;

    const QString _ip;
    qint16 _powerSP2;
    qint16 _powerSP3;

    float _SP1;
    float _SP2;
    float _SP3;
    float _SP4;

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
    void disableControlLoop(bool flag);
    void setPoint();//better NAme!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    //3_Control
    void setSP();
    void startTimer();
    void stopTimer();
    void controlLoop();
    void disablePowerSP(bool flag);

    //4_Any_Request
    void anyRequest();

    //5_Response_Management
    void readResponse(quint8 TID1, quint8 TID2, float data);
    void writeResponse(quint8 TID1, quint8 TID2, bool status);

signals:
    void stopControlLoop();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
