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


    float requestResponse();
    bool changePowerResponse();

public:
    explicit Client(QObject *parent = nullptr);
    Client(QString _ip, QObject *parent = nullptr);

    float checkLvVoltage();
    bool changeAcPower(float power);
    bool connectStatus();

    void printBitWise(qint8 byte);
    void printBitWise(qint16 bytes);


signals:

public slots:
    void signIn();
    void serverDisconnected();
};

#endif // CLIENT_H
