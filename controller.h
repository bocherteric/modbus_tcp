#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "client.h"


class Controller : public QObject
{
    QString _ip;
    const int _SP1;
    const int _SP2;
    const int _SP3;
    const int _SP4;

    Client* client = nullptr;
    void errorState();

public:
    Controller(QString _ip, int _SP1, int _SP2, int _SP3, int _SP4, QObject *parent = nullptr);


public slots:
    void controlLoop();

signals:
    void loopRepeat();

};

#endif // CONTROLLER_H