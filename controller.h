#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "client.h"

#include <QTimer>


class Controller : public QObject
{

    const int _SP1;
    const int _SP2;
    const int _SP3;
    const int _SP4;

    QTimer* timerLV =nullptr;
    QTimer* timerAC =nullptr;

    void errorState();

public:
    Controller(int _SP1, int _SP2, int _SP3, int _SP4, QObject *parent = nullptr);
    float controlLoop(float voltage);
    float round(float power);

public slots:
    void controlLoop();
    void chargeLoop();
    void readLVloop();

};

#endif // CONTROLLER_H
