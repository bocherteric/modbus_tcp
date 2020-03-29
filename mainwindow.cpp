#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QString ip, QWidget *parent) :
    QWidget(parent),_ip(ip),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client = new Client(_ip);
    timer = new QTimer(this);

    //does it need a connect?
    timer2 = new QElapsedTimer();
    //qDebug() << "system clock is monotonic and will not overflow: " << timer2->isMonotonic();

    //1_LV_Voltage
    connect(ui->pushButton1,&QPushButton::clicked,this, &MainWindow::readLVvoltge);
    //2_Power_Setpoint
    connect(ui->pushButton2, &QPushButton::clicked, this, &MainWindow::setPowerSP);
    connect(ui->pushButton2_1, &QPushButton::clicked, this, &MainWindow::setPoint);
    //4_Any_Request
    connect(ui->pushButton4, &QPushButton::clicked, this, &MainWindow::anyRequest);
    //5_Response_Management
    connect(client, &Client::readResponse, this, &MainWindow::readResponse);
    connect(client, &Client::writeResponse, this, &MainWindow::writeResponse);

    //3_Control
    connect(ui->pushButtonSP, &QPushButton::clicked,this, &MainWindow::setSP);
    connect(ui->pushButtonStart, &QPushButton::clicked,this, &MainWindow::startTimer);
    connect(ui->pushButtonStop, &QPushButton::clicked,this, &MainWindow::stopTimer);
    connect(this, &MainWindow::stopControlLoop,this, &MainWindow::stopTimer);
    connect(timer, &QTimer::timeout, this, &MainWindow::controlLoop);

    //GUI
    ui->pushButtonStart->setEnabled(false);
    ui->pushButtonStop->setEnabled(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//1_LV_Voltage
//
void MainWindow::readLVvoltge(){
    LVclicked =true;
    client->genericRead(2,1);
}

//2_Power_Setpoint
//
void MainWindow::setPowerSP(){
    disableControlLoop(true);
    QString text = ui->text2->toPlainText();
    _powerSP2 =text.toShort();
    //qDebug() << "powerSP2: " << _powerSP2;
    timer2->start();
    client->genericWrite(4,2,_powerSP2);
}

void MainWindow::disableControlLoop(bool flag){
    if(flag){
        ui->pushButtonStart->setEnabled(false);
        ui->pushButtonStop->setEnabled(false);
        ui->pushButtonSP->setEnabled(false);
    }else{
        ui->pushButtonStart->setEnabled(false);
        ui->pushButtonStop->setEnabled(false);
        ui->pushButtonSP->setEnabled(true);

    }
}

void MainWindow::setPoint(){
    client->genericWrite(4,99,0);
}

//3_Control
//
// incomplete
//
void MainWindow::setSP(){

    QString text1 = ui->textSP1->toPlainText();
    QString text2 = ui->textSP2->toPlainText();
    QString text3 = ui->textSP3->toPlainText();
    QString text4 = ui->textSP4->toPlainText();
    _SP1 = text1.toFloat();
    _SP2 = text2.toFloat();
    _SP3 = text3.toFloat();
    _SP4 = text4.toFloat();

    if(_SP1<_SP2 && _SP2<_SP3 && _SP3<_SP4 && _SP1 > 0){
        qDebug("Power setpoints are set");
        //qDebug() <<_SP1 << ", " << _SP2 << ", " << _SP3 << ", " << _SP4;
        ui->pushButtonStart->setEnabled(true);
    }else{
        qDebug("0<SP1<SP2<SP3<SP4 not fulfilled");
        //qDebug() <<_SP1 << ", " << _SP2 << ", " << _SP3 << ", " << _SP4;
    }
}

void MainWindow::startTimer(){
    disablePowerSP(true);
    controller = new Controller(_SP1, _SP2, _SP3, _SP4);
    ui->pushButtonStart->setEnabled(false);
    ui->pushButtonStop->setEnabled(false);
    timer->start(2000);
}

void MainWindow::controlLoop(){
    client->genericRead(2,3);
}

void MainWindow::stopTimer(){
    timer->stop();
    client->genericWrite(4,3,0);//sets power SP to 0
}

void MainWindow::disablePowerSP(bool flag){
    if(flag){
       ui->pushButton2->setEnabled(false);
       ui->pushButton2_1->setEnabled(false);
    }else{
       ui->pushButton2->setEnabled(true);
       ui->pushButton2_1->setEnabled(true);
    }
}

//4_Any_Request
//
void MainWindow::anyRequest(){
    QString text = ui->text41->toPlainText();
    quint16 t = text.toUShort();
    quint8 request =static_cast<quint8>(t); // could cast directly to unsigned char
    qDebug() << "anyRequest: request =" << request;
    if(request== 1 || request== 2 || request== 3 || request== 5){
        client->genericRead(request,4);
    }else if (request == 4) {
        QString text = ui->text42->toPlainText();
        _powerSP2 =text.toShort();
        client->genericWrite(request,4,_powerSP2);
    }else {
        qDebug() << "anyRequest: wrong request!";
    }
}

//5_Response_Management
//
//writeResponse
//
void MainWindow::writeResponse(quint8 TID1, quint8 TID2, bool status){

    if(TID1 == 4){

        switch(TID2){
        case 2:
            if(status == true){
            client->genericRead(1,2);
                qDebug("power setpoint was set correctly");
            }else {
                qDebug("MainWindow::writeResponse: power setpoint was not set");
            }
            break;
        case 3:
            if(status == true){
                QString output = QString::number(_powerSP3);
                ui->browserSP->append(output);
            }else {
                emit stopControlLoop();
                qDebug("MainWindow::writeResponse: controlLoop stopped");
            }

            break;
        case 4:
            break;
        case 99:
            if(!status){
                ui->browserEmergency->append("power setpoint was NOT reset correctly");
            }
            break;
        default:
            qDebug("MainWindow::writeResponse: wrong TID2");
        }
    }else{
        qDebug("MainWindow::writeResponse: wrong TID1");
    }

}

//readResponse
//
void MainWindow::readResponse(quint8 TID1, quint8 TID2, float data){
    QString output = QString::number(data);

    switch(TID2){ //distiniguishes between 4 possible GUI opertions
    case 1:
        if(TID1 == 2){
            ui->browser1->append(output);
        }else {
            qDebug("MainWindow::writeResponse(TID2 = 1): wrong TID1");
        }
        break;
    case 2:
        //check if everything is still correct after changes
        /*for measuring how much time passes before Power setpoint is actually set
         */

            if(static_cast<qint16>(data) ==_powerSP2){
                qint64 time =timer2->elapsed();
                ui->browserTimer->append(QString::number(time));
                ui->browserSP->append(QString::number(data));
                disableControlLoop(false);
            }else {
                ui->browserSP->append(QString::number(data));
                client->genericRead(1,2);
            }
        break;
    case 3:
        if(TID1 == 2){
            ui->browserU->append(output);
            _powerSP3 = controller->controlLoop(data);
            client->genericWrite(4,3,_powerSP3);
        }else{
            emit stopControlLoop();
            qDebug("MainWindow::writeResponse: controlLoop stopped");
        }
        break;
    case 4:
        break;
    default:
        qDebug("MainWindow::writeResponse: wrong TID2");
    }

}




