#include "client.h"

Client::Client(QString ip, QObject *parent)
    :QObject(parent), _ip(ip)
{
    _socket = new QTcpSocket(this);
    _socket->connectToHost(ip,_port);
    connect(_socket, &QAbstractSocket::connected, this, &Client::signIn);
    connect(_socket, &QAbstractSocket::disconnected, this, &Client::signIn);//?????????????????????????????????????????????????????????
    connect(_socket, &QAbstractSocket::readyRead, this, &Client::responseParsing);

    /*###############GUI_COMMAND########################
 * 1_LV_Voltage -> TID2: 1
 * 2_Power_Setpoint -> TID2: 2
 * 3_Control -> TID2: 3
 * 4_Any_Request -> TID2: 4
 */

}

//genericRead
//
void Client::genericRead(quint8 TID1, quint8 TID2){
    /*###############ADDRESSES_FOR_READ_REQUESTS##########################
 * Input power 1 -> TID1: 1 -> Register-address: 0x0c //Scalefactor: 0.1
 * Battery voltage -> TID1: 2 -> Register-address: 0x1a //Scalefactor: 100
 * Battery current -> TID1: 3 -> Register-address: 0x1b //Scalefactor: 10
 * ESS power setpoint phase -> TID1: 5 -> Register-address: 0x25 //Scalefactor: 1
 */

    std::vector<quint8> vector;
    quint8 addr;

    switch (TID1) {

    case 1:
        addr=0x0c;
        break;
    case 2:
        addr=0x1a;
        break;
    case 3:
        addr=0x1b;
        break;
    case 5:
        addr=0x25;
        break;
    default:
        qDebug() << "genericRead:Wrong TID1!!!";
        //emit Signal to warn MAinWindow that something went wrong!

    }
    //building correct modbus request-message
    vector.push_back(TID1); //transaction Id
    vector.push_back(TID2); //random transaction Id
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

        //##############DEBUGGING###############
        qDebug()<< x;
        //##############DEBUGGING###############
    }
}

//genericWrite
//
void Client::genericWrite(quint8 TID1, quint8 TID2, float data){
  /*###############ADDRESSES_FOR_READ_REQUESTS##########################
   * ESS power setpoint phase 1 -> TID1: 4 -> Register-address: 0x25 //Scalefactor: 1
   */

    std::vector<quint8> vector;
    qint16 power = data;
    quint8 addr;

    switch(TID1){
        case 4:
            addr=0x25;
            break;
    default:
        qDebug() << "genericRead:Wrong TID1!!!";
        //emit Signal to warn MAinWindow that something went wrong!

    }

    vector.push_back(TID1); //random transaction Id
    vector.push_back(TID2); //random transaction Id
    vector.push_back(0x00); //modbus specific protocol Id
    vector.push_back(0x00); //modbus specific protocol Id
    vector.push_back(0x00); //message length, 8 bytes follow
    vector.push_back(0x06); //message length, 8 bytes follow
    vector.push_back(0xf2); //Unit-Id for Venus Gx VE.Bus port
    vector.push_back(0x06); //F-code: write single register
    vector.push_back(0x00); //Register address for DC voltage
    vector.push_back(addr); //Register address for DC voltage

    qDebug() << "genericWrite request message sent:";
    for(auto &x : vector){
        _stream << x;

        //##############DEBUGGING###############
        qDebug() << x;
        //##############DEBUGGING###############
    }
    _stream << power;

    //##############DEBUGGING###############
    qDebug() << power;
    //##############DEBUGGING###############
}

//responseParsing
//
//distinguishes between Write- and Read-Response, calls correct method for either
//
void Client::responseParsing(){
    quint8 TID1;
    _stream >> TID1;

    if(TID1 <=3 || TID1 == 5){
        genericReadResponse(TID1);
    }else if(TID1==4){
        genericWriteResponse(TID1);
    }else {
        qDebug()<< "Error in responseParsing";
    }
}

//genericReadResponse
//
//extracts value from response message, multiplies value with according scalefactor and sends to MainWindow
//
void Client::genericReadResponse(quint8 TID1){

    std::vector<quint8> vector;
    quint8 TID2, PID1, PID2, length1, length2;
    _stream >> TID2 >> PID1 >> PID2 >> length1 >> length2;

        for(quint8 i=0; i<length2; i++){
            quint8 value;
            _stream >> value;
            vector.push_back(value);

        }


    //##############DEBUGGING###############
    qDebug() << "genericReadResponse - received message:";
    qDebug() << TID1 << TID2 << PID1 << PID2 << length1 << length2;

    for(auto &x :vector){
        qDebug() << x;
    }
    //##############DEBUGGING###############

    quint8 hexMsb= vector.at(3);
    quint8 hexLsb= vector.at(4);
    float data = (hexMsb << 8) | hexLsb;

    switch(TID1){

    case 1:
        data= (data*10);
        break;
    case 2:
        data= (data/100);
        break;
    case 3:
        data= (data/10);
        break;
    case 5:
        break;
    default:
        qDebug() << "wrong TID1 in genericReadResponse";
    }

    emit readResponse(TID1, TID2, data);
}

//genericWriteResponse
//
//checks if write request was sucessfull and sends status to MainWindow
void Client::genericWriteResponse(quint8 TID1){
    std::vector<quint8> vector;
    bool status = false;
    quint8 TID2, PID1, PID2, length1, length2;
    _stream >> TID2 >> PID1 >> PID2 >> length1 >> length2;

        for(quint8 i=0; i<length2; i++){
            quint8 value;
            _stream >> value;
            vector.push_back(value);
        }

    //##############DEBUGGING###############
    qDebug() << "genericWriteResponse - received message:";
    qDebug() << TID1 << TID2 << PID1 << PID2 << length1 << length2;

    for(auto &x :vector){
        qDebug() << x;
    }
    //##############DEBUGGING###############

    if(vector.at(1)== 0x06){
        status =true;
    }else {
        qDebug() << "Bug in changePowerResponse(): F-Code != 0x06";
        for(auto &x :vector){
            qDebug() <<x;
        }
    }
    emit writeResponse(TID1, TID2, status);

}

//##############THIS&THAT###############

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


