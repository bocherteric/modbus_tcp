#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QString ip, QWidget *parent) :
    QWidget(parent),_ip(ip),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client = new Client(_ip);


    //timer = new QElapsedTimer();

    connect(ui->pushButton1,&QPushButton::clicked,this, &MainWindow::readLVvoltge);
    connect(ui->pushButton2, &QPushButton::clicked, this, &MainWindow::setPowerSP);
    connect(ui->pushButton4, &QPushButton::clicked, this, &MainWindow::anyRequest);
    connect(client, &Client::readResponse, this, &MainWindow::readResponse);
    connect(client, &Client::writeResponse, this, &MainWindow::writeResponse);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//1_LV_Voltage
//
void MainWindow::readLVvoltge(){
    LVclicked =true;
    client->genericRead(2);
}

//2_Power_Setpoint
//
void MainWindow::setPowerSP(){
    //qDebug()<< "hiu";
    QString text = ui->text2->toPlainText();
    _power =text.toFloat();

    timer->start();
    client->genericWrite(4,_power);
}

//3_Control
//
// missing
// probably timers of some sorts would need to be implemented
//

//4_Any_Request
//
void MainWindow::anyRequest(){
    QString text = ui->text41->toPlainText();
    quint8 request =text.toInt();
    qDebug() << "anyRequest: request =" << request;
    if(request== 1 || request== 2 || request== 3 || request== 5){
        client->genericRead(request);
    }else if (request == 4) {
        QString text = ui->text42->toPlainText();
        _power =text.toFloat();
        client->genericWrite(request,_power);
    }else {
        qDebug() << "anyRequest: wrong request!";
    }
}

//writeResponse
//
void MainWindow::writeResponse(quint8 transID, bool status){
    switch (transID) {
    case 4:
        if(status == true){
            /*for time measurment
             *
            client->genericRead(1);*/
            ui->browserEmergency->append("set SP true");
        }else {
            ui->browserEmergency->append("set SP false");
        }
        break;
    }
}

//readResponse
//
//there needs to be a means of printing the correct values to the correct windows. Neue Übergabevariable?
//
void MainWindow::readResponse(quint8 transID, float data){
    QString output = QString::number(data);
    ui->browser4->append(output);

    /*for time measurment
     *
    switch(transID){
        case 1:
        if(data==_power){
            qDebug() << "hiuhiu" ;
            int time =timer->elapsed();
            ui->timer2->append(QString::number(time));
        }else {
            ui->browserSP->append(QString::number(data));
            client->genericRead(1);
        }
        break;

    case 2:
        if(LVclicked == true){
            QString output = QString::number(data);
            ui->browser1->append(output);
            LVclicked =false;
        }else {
            qDebug() << "case2 in requestResponse";
        }
        break;
    case 3:
        qDebug() << "case3 in requestResponse";
        break;
    default:
        qDebug() << "wrong transID in requestResponse";

    }*/
}




