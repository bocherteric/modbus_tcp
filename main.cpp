#include <QCoreApplication>
#include "controller.h"

//need to implement connected status, reading and writing may only be possible if the connected status is true

int main(int argc, char *argv[])
{
    //set IP-Address
    QString ip = "Venus.local";

    //set Controller Set-Points
    int SP1= 9;
    int SP2=10;
    int SP3=12;
    int SP4=13;


    QCoreApplication a(argc, argv);
    Controller controller(ip, SP1, SP2, SP3, SP4);

    //Client cl(ip);


/*
    while(1)
        if(cl.connectStatus() ==true){
            cl.checkLvVoltage();
        }*/

    return a.exec();
}


