#include "client.h"

Client::Client(QObject *parent) : QObject(parent)
{

}


Client::Client(QString ip, QObject *parent)
    :QObject(parent), _ip(ip)
{
    _socket = new QTcpSocket(this);
    _socket->connectToHost(ip,_port);
    connect(_socket, &QAbstractSocket::connected, this, &Client::signIn);
    connect(_socket, &QAbstractSocket::disconnected, this, &Client::signIn);

}

float Client::genericRead(quint8 addr){
 std::vector<quint8> vector;
    //building correct modbus request-message
    vector.push_back(0x11); //random transaction Id
    vector.push_back(0x11); //random transaction Id
    vector.push_back(0x00); //modbus specific protocol Id
    vector.push_back(0x00); //modbus specific protocol Id
    vector.push_back(0x00); //message length, 6 bytes follow
    vector.push_back(0x06); //message length, 6 bytes follow
    vector.push_back(0xf2); //Unit-Id for Venus Gx VE.Bus port
    vector.push_back(0x04); //F-code
    vector.push_back(0x00); //Register address for DC voltage
    vector.push_back(addr); //Register address for DC voltage
    vector.push_back(0x00); //number of requested registers
    vector.push_back(0x01); //number of requested registers

    qDebug() << "genericRead request message sent:";
    for(auto &x : vector){
        _stream << x;
        qDebug()<< x;
    }

    while(_socket->waitForReadyRead(-1)==false){

    }
    //_flag = true;if you only want request response for genericRead printed
    return requestResponse();
}


float Client::checkLvVoltage(){
    std::vector<quint8> vector;

    //building correct modbus request-message
    vector.push_back(0x11); //random transaction Id
    vector.push_back(0x11); //random transaction Id
    vector.push_back(0x00); //modbus specific protocol Id
    vector.push_back(0x00); //modbus specific protocol Id
    vector.push_back(0x00); //message length, 6 bytes follow
    vector.push_back(0x06); //message length, 6 bytes follow
    vector.push_back(0xf2); //Unit-Id for Venus Gx VE.Bus port
    vector.push_back(0x04); //F-code
    vector.push_back(0x00); //Register address for DC voltage
    vector.push_back(0x1a); //Register address for DC voltage
    vector.push_back(0x00); //number of requested registers
    vector.push_back(0x01); //number of requested registers

    /*
    vector.push_back(0x11); //random transaction Id
    vector.push_back(0x11); //random transaction Id
    vector.push_back(0x00); //modbus specific protocol Id
    vector.push_back(0x00); //modbus specific protocol Id
    vector.push_back(0x00); //message length, 6 bytes follow
    vector.push_back(0x06); //message length, 6 bytes follow
    vector.push_back(0x01); //Address
    vector.push_back(0x03); //F-code 0x04 = Input register, 0x03 = Holding register
    vector.push_back(0x00); //Register address for DC voltage
    vector.push_back(0x04); //Register address for DC voltage
    vector.push_back(0x00); //number of requested register
    vector.push_back(0x01); //number of requested register*/

    //send checkLvVoltage request-message
    //qDebug() << "sent checkLvVoltage request-message:";
    for(auto &x : vector){
        _stream << x;
        //qDebug() << x;
    }

    //connect(socketC, &QAbstractSocket::readyRead, this, &Client::receive);
    while(_socket->waitForReadyRead(-1)==false){

    }
    return requestResponse();
}

float Client::requestResponse(){
    std::vector<quint8> vector;
    quint8 TID1,TID2, PID1, PID2, length1, length2;
    _stream >> TID1 >> TID2 >> PID1 >> PID2 >> length1 >> length2;

    if(length1 == 0){
        for(quint8 i=0; i<length2; i++){
            quint8 value;
            _stream >> value;
            vector.push_back(value);

        }
    }else {
        qDebug() << "Bug in requestResponse(): length1 != 0";
        return 999;
    }

    //##############DEBUGGING###############
    if(_flag == true){
    qDebug() << "received message:";
    qDebug() << TID1 << TID2 << PID1 << PID2 << length1 << length2;

    for(auto &x :vector){
        qDebug() << x;
    }
    _flag=false;
    }

    //checking correctness
    if(vector.at(1) != 0x04){
        qDebug() << "Bug in requestResponse(): F-Code != 0x04";
        return 999;
    }

    //extracting data
    quint8 hexMsb= vector.at(3);
    quint8 hexLsb= vector.at(4);

    float data = (hexMsb << 8) | hexLsb;
    data = (data/100); //Scalefactor for DC Volatge is 100

    return data;
}


bool Client::changeAcPower(float p){
    std::vector<quint8> vector;
    //qDebug() << "float p:"<< p;
    qint16 power = p;

    //##############DEBUGGING###############
    /*
    qDebug() << "qint16 power:" << power;
    qDebug() << "qint16 power bitwise:";
    printBitWise(power);*/

    /* //needed if stream works better with bytes only
    qint8 powerMsb = ((power >> 8) & 0xff);
    qDebug() << "qint8 powerMsb:" << powerMsb;
    qDebug() << "qint18 powerMsb bitwise:";
    printBitWise(powerMsb);


    qint8 powerLsb = (power & 0xff);
    qDebug() << "qint8 powerLsb:" << powerLsb;
    qDebug() << "qint18 powerLsb bitwise:";
    printBitWise(powerLsb);
    */

    vector.push_back(0x11); //random transaction Id
    vector.push_back(0x11); //random transaction Id
    vector.push_back(0x00); //modbus specific protocol Id
    vector.push_back(0x00); //modbus specific protocol Id
    vector.push_back(0x00); //message length, 8 bytes follow
    vector.push_back(0x06); //message length, 8 bytes follow
    vector.push_back(0xf2); //Unit-Id for Venus Gx VE.Bus port
    vector.push_back(0x06); //F-code: write single register
    vector.push_back(0x00); //Register address for DC voltage
    vector.push_back(0x25); //Register address for DC voltage

    /*vector.push_back(0x11); //random transaction Id
    vector.push_back(0x11); //random transaction Id
    vector.push_back(0x00); //modbus specific protocol Id
    vector.push_back(0x00); //modbus specific protocol Id
    vector.push_back(0x00); //message length, 8 bytes follow
    vector.push_back(0x06); //message length, 8 bytes follow
    vector.push_back(0x01); //Address
    vector.push_back(0x06); //F-code 0x04 = Input register, 0x03 = Holding register
    vector.push_back(0x00); //Register address
    vector.push_back(0x10); //Register address*/

    //send changeAcPower request
    //qDebug() << "sent changeAcPower request:";
    for(auto &x : vector){
        _stream << x;
        //qDebug() << x;
    }
    _stream << power;
    //qDebug() << power;

    while(_socket->waitForReadyRead(-1)==false){}

    return changePowerResponse();
}



bool Client::changePowerResponse(){
    std::vector<quint8> vector;
    quint8 TID1,TID2, PID1, PID2, length1, length2;
    _stream >> TID1 >> TID2 >> PID1 >> PID2 >> length1 >> length2;

    if(length1 == 0){
        for(quint8 i=0; i<length2; i++){
            quint8 value;
            _stream >> value;
            vector.push_back(value);

        }
    }else {
        qDebug() << "Bug in changePowerResponse(): length1 != 0";
        return false;
    }
    /*//##############DEBUGGING###############
    qDebug() << "received message:";
    qDebug() << TID1 << TID2 << PID1 << PID2 << length1 << length2;

    for(auto &x :vector){
        qDebug() << x;
    }*/

    if(vector.at(1)== 0x06){
        return true;
    }else {
        qDebug() << "Bug in changePowerResponse(): F-Code != 0x06";
        for(auto &x :vector){
            qDebug() <<x;
        }
        return false;
    }

}

void Client::signIn(){
    _stream.setDevice(_socket);
    _connectStatus =true;
    qDebug() << "Client connected";
}

void Client::serverDisconnected(){
    qDebug() << "Server disconnected";
}

bool Client::connectStatus(){
    return _connectStatus;
}


void Client::printBitWise(qint8 b){
    qint8 byte =b;
    for(qint8 i=7;i>=0;i--){
        qDebug() << ((byte >> i) & 1);
    }
}


void Client::printBitWise(qint16 bs){
    qint16 bytes =bs;
    for(qint8 i=15;i>=0;i--){
        qDebug() << ((bytes >> i) & 1);
    }
}


