#include <QCoreApplication>
#include <QApplication>
#include "controller.h"
#include "mainwindow.h"

//need to implement connected status, reading and writing may only be possible if the connected status is true

int main(int argc, char *argv[])
{
    //set IP-Address
    QString ip = "Venus.local";
    //port is set automatically

    QApplication app(argc, argv);
    MainWindow w(ip);
    w.show();

    return app.exec();
}


