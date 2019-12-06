#include <QCoreApplication>
#include "controller.h"


int main(int argc, char *argv[])
{
    //set IP-Address
    //another silly git test:)
    //oops, already staged but forgot this important comment
    QString ip = "127.0.0.1";

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


