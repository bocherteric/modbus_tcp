#include <QCoreApplication>
#include <QApplication>
#include "controller.h"
#include "mainwindow.h"

//need to implement connected status, reading and writing may only be possible if the connected status is true

int main(int argc, char *argv[])
{
    //set IP-Address
    //port is set automatically
    /*
     * CHANGE BACK
     *
    QString ip = "Venus.local";
    *
    * */
    QString ip = "127.0.0.1";

    QApplication app(argc, argv);
    MainWindow w(ip);
    w.show();

    return app.exec();
}

/*TO_DO
 *
 *Tesing
 *GUI
 *2_power set points
 */


