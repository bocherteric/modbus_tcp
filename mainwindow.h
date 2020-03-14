#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "client.h"

#include <QWidget>
#include <QElapsedTimer>
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT
    Client * client =nullptr;
    QElapsedTimer * timer = nullptr;
    const QString _ip;
    float _power;
    bool LVclicked = false;
    bool SPset = false;

public:
    explicit MainWindow(QString _ip,QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void readLVvoltge();
    void setPowerSP();
    void readResponse(quint8 transID, float data);
    void writeResponse(quint8 transID, bool status);
    void anyRequest();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
