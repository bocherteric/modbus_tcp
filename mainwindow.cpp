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
    connect(ui->pushButton2_1, &QPushButton::clicked, this, &MainWindow::setPowerSPZero);
    connect(ui->pushButton2_2, &QPushButton::clicked, this, &MainWindow::clearBrowser);
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
    ui->browser1->clear();
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

void MainWindow::setPowerSPZero(){
    client->genericWrite(4,99,0);
}

void MainWindow::clearBrowser(){
    ui->browserTimer->clear();
    ui->browserP->clear();
}

//3_Control
//
//checks if power setpoints for controller inputed by user are making sense.
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
        qDebug("Power setpoints for controlLoop are set");
        //qDebug() <<_SP1 << ", " << _SP2 << ", " << _SP3 << ", " << _SP4;
        ui->pushButtonStart->setEnabled(true);
    }else{
        qDebug("0<SP1<SP2<SP3<SP4 not fulfilled");
        //qDebug() <<_SP1 << ", " << _SP2 << ", " << _SP3 << ", " << _SP4;
    }
}

// initiates controller object, starts timer that trigers control loop
void MainWindow::startTimer(){
    disablePowerSP(true);
    controller = new Controller(_SP1, _SP2, _SP3, _SP4);
    ui->browserU->clear();
    ui->browserSP->clear();
    ui->pushButtonStart->setEnabled(false);
    ui->pushButtonSP->setEnabled(false);
    ui->pushButtonStop->setEnabled(true);
    timer->start(2000);
}

void MainWindow::controlLoop(){
    client->genericRead(2,3);
}

void MainWindow::stopTimer(){
    timer->stop();
    disablePowerSP(false);
    ui->pushButtonSP->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);
    client->genericWrite(4,99,0);//sets power SP to 0
}

void MainWindow::disablePowerSP(bool flag){
    if(flag){
        ui->pushButton2->setEnabled(false);
        ui->pushButton2_1->setEnabled(false);
        ui->pushButton2_2->setEnabled(false);
    }else{
        ui->pushButton2->setEnabled(true);
        ui->pushButton2_1->setEnabled(true);
        ui->pushButton2_2->setEnabled(true);
    }
}

//4_Any_Request
//
void MainWindow::anyRequest(){
    //QString text = ui->text41->toPlainText();
    //QString text = "1";
    //quint16 t = text.toUShort();
    //quint8 request =static_cast<quint8>(t); // could cast directly to unsigned char
    int request = ui->text41->currentIndex();

    qDebug() << "anyRequest: request =" << request;
    if(request== 1 || request== 2 || request== 3 || request== 5){
        client->genericRead(request,4);
    }else if (request == 4) {
        QString text = ui->text42->toPlainText();
        ui->text42->clear();
        _powerSP4 =text.toShort();
        client->genericWrite(request,4,_powerSP4);
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
        QString output = "";
        switch(TID2){ //distiniguishes between the 4 possible GUI opertions
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
                output = QString::number(_powerSP3);
                ui->browserSP->append(output);
            }else {
                emit stopControlLoop();
                qDebug("MainWindow::writeResponse: controlLoop stopped");
            }

            break;
        case 4:
            if(status == true){
                 output = QString::number(_powerSP4);
                 ui->browser4->append("SP set to " + output);
            }else {
                qDebug("MainWindow::writeResponse: power setpoint was not set2");
            }

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
    qint16 dataTemp = static_cast<qint16>(data);

    switch(TID2){ //distiniguishes between the 4 possible GUI opertions
    case 1:
        if(TID1 == 2){
            ui->browser1->append(output);
        }else {
            qDebug("MainWindow::writeResponse(TID2 = 1): wrong TID1");
        }
        break;
    case 2:
        if(dataTemp ==_powerSP2){
            qint64 time =timer2->elapsed();
            QString timer = QString::number(time);
            QString powerSP = QString::number(_powerSP2);

            ui->browserTimer->append(powerSP +": " + timer);
            ui->browserP->append(QString::number(dataTemp));
            disableControlLoop(false);
        }else {
            ui->browserP->append(QString::number(dataTemp));
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
            qDebug("MainWindow::readResponse: controlLoop stopped");
        }
        break;
    case 4:
        switch(TID1){
        case 1:
            ui->browser4->append("Input Power[W]: " + output);
            break;
        case 2:
            ui->browser4->append("Battery Voltage[V]: " + output);
            break;
        case 3:
            ui->browser4->append("Battery Current[A]: " + output);
            break;
        case 5:
            ui->browser4->append("Power SP[W]: " + output);
            break;
        default:
            qDebug("MainWindow::writeResponse: wrong TID1");
        }

        break;
    default:
        qDebug("MainWindow::writeResponse: wrong TID2");
    }

}




