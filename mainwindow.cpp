#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QString ip, QWidget *parent) :
    QWidget(parent),_ip(ip),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client = new Client(_ip);
    timer = new QTimer(this);

    //needs to be member too, also needs a connect
    //QElapsedTimer * timer2 = new QElapsedTimer();

    //1_LV_Voltage
    connect(ui->pushButton1,&QPushButton::clicked,this, &MainWindow::readLVvoltge);
    //2_Power_Setpoint
    connect(ui->pushButton2, &QPushButton::clicked, this, &MainWindow::setPowerSP);
    //4_Any_Request
    connect(ui->pushButton4, &QPushButton::clicked, this, &MainWindow::anyRequest);
    //5_Response_Management
    connect(client, &Client::readResponse, this, &MainWindow::readResponse);
    connect(client, &Client::writeResponse, this, &MainWindow::writeResponse);

    //3_Control
    connect(ui->pushButtonSP, &QPushButton::clicked,this, &MainWindow::setSP);
    connect(ui->pushButtonStart, &QPushButton::clicked,this, &MainWindow::startTimer);
    //connect(ui->pushButtonStop, &QPushButton::clicked,this, &MainWindow::setSP);
    //implementation of stop_Button missing
    connect(timer, &QTimer::timeout, this, &MainWindow::controlLoop);

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
    QString text = ui->text2->toPlainText();
    _power =text.toFloat();

    timer->start();
    client->genericWrite(4,2,_power);
}

//3_Control
//
// incomplete
//
void MainWindow::setSP(){
    //need to check if every field is full and if the values in the fields are making sense
    QString text1 = ui->textSP1->toPlainText();
    QString text2 = ui->textSP2->toPlainText();
    QString text3 = ui->textSP3->toPlainText();
    QString text4 = ui->textSP4->toPlainText();
    _SP1 = text1.toInt();
    _SP2 = text2.toInt();
    _SP3 = text3.toInt();
    _SP4 = text4.toInt();
    //dont forget to disable button during running of control loop
    //activate start button

}

void MainWindow::startTimer(){
    controller = new Controller(_SP1, _SP2, _SP3, _SP4);
    //disable pushButtonSP,pushButtonStart

    timer->start(200);
}

void MainWindow::controlLoop(){
    client->genericRead(2,3);
}

void MainWindow::stopTimer(){

}

//4_Any_Request
//
void MainWindow::anyRequest(){
    QString text = ui->text41->toPlainText();
    quint8 request =text.toInt();
    qDebug() << "anyRequest: request =" << request;
    if(request== 1 || request== 2 || request== 3 || request== 5){
        client->genericRead(request,4);
    }else if (request == 4) {
        QString text = ui->text42->toPlainText();
        _power =text.toFloat();
        client->genericWrite(request,4,_power);
    }else {
        qDebug() << "anyRequest: wrong request!";
    }
}

//5_Response_Management
//
//writeResponse
//
void MainWindow::writeResponse(quint8 TID1, quint8 TID2, bool status){

    switch(TID2){
    case 2:
        if(status == true){
            /*for time measurment
             *
            client->genericRead(1);*/
            ui->browserEmergency->append("set SP true");
        }else {
            ui->browserEmergency->append("set SP false");
        }
        break;
    case 3:
       /*
        * missing:
        * part here
        * exit control loop: stop button pressed + errors in communication with Inverter
        * controller
        * SP comparison
        * UI
        */
        break;
    case 4:
        break;
    default:
        qDebug("MainWindow::writeResponse: wrong TID2");
    }

}

//readResponse
//
//there needs to be a means of printing the correct values to the correct windows. Neue Übergabevariable?
//
void MainWindow::readResponse(quint8 TID1, quint8 TID2, float data){
    QString output = QString::number(data);

    switch(TID2){
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
         *initiate timer2 object in constructor
         *
        switch(TID1){
            case 1:
            if(data==_power){
                qDebug() << "hiuhiu" ;
                int time =timer2->elapsed();
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
            qDebug() << "wrong TID1 in requestResponse";

        }*/
        break;
    case 3:
        if(TID1 == 2){
            //could implement the comparison of old and new value here
            client->genericWrite(4,3,controller->controlLoop(data));
        }else{
            qDebug("MainWindow::writeResponse(TID2 = 3): wrong TID1");
        }
        break;
    case 4:
        break;
    default:
        qDebug("MainWindow::writeResponse: wrong TID2");
    }

}




