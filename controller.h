#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "client.h"

#include <QTimer>


class Controller : public QObject
{

    const float _SP1;
    const float _SP2;
    const float _SP3;
    const float _SP4;

    float _m1;
    float _m2;
    float _t1;
    float _t2;

public:
    Controller(float _SP1, float _SP2, float _SP3, float _SP4, QObject *parent = nullptr);
    qint16 controlLoop(float voltage);

public slots:


};

#endif // CONTROLLER_H
