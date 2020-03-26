#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>


class Client : public QObject
{
    Q_OBJECT
    const QString _ip;
    const quint16 _port = 502;
    QDataStream _stream;
    QTcpSocket* _socket = nullptr;
    bool _connectStatus = false;
    float _power=0;
    bool _flag=false;


    float requestResponse();
    bool changePowerResponse();
    void genericReadResponse(quint8 pups);
    void genericWriteResponse(quint8 transID);

public:
    explicit Client(QObject *parent = nullptr);
    Client(QString _ip, QObject *parent = nullptr);

    float checkLvVoltage();
    bool changeAcPower(float power);
    bool connectStatus();

    void genericRead(quint8 TID1, quint8 TID2);
    void genericWrite(quint8 TID1, quint8 TID2, float data);

    void printBitWise(qint8 byte);
    void printBitWise(qint16 bytes);


signals:
    void readResponse(quint8 TID1, quint8 TID2, float data);
    void writeResponse(quint8 TID1, quint8 TID2, bool status);

public slots:
    void signIn();
    void serverDisconnected();
    void responseParsing();

};

#endif // CLIENT_H
