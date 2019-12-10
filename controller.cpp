#include "controller.h"




Controller::Controller(QString ip, int SP1, int SP2, int SP3, int SP4, QObject* parent)
    :QObject(parent),_ip(ip),_SP1(SP1),_SP2(SP2),_SP3(SP3),_SP4(SP4)
{

    client = new Client(ip);
    //connect(this, &Controller::loopRepeat, this, &Controller::controlLoop);
    //controlLoop();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Controller::testLoop);
    timer->start(1000);

}

void Controller::testLoop(){

    if(client->connectStatus()==true){
        float LV = client->checkLvVoltage();
        static bool test = true;
        if(test){
            qDebug() << "LV value recieved by Controller";
            test =false;
        }
        qDebug() << LV;
        if (LV == 999){
            errorState();
        }
        //timer->stop();
    }
}

void Controller::controlLoop(){

    float power = 0;
    float LV = client->checkLvVoltage();
    bool test;

    //checking for correctness
    if (LV == 999){
        errorState();
    }

    if(LV <= _SP1){
        power=400;
    }else if(_SP1 < LV < _SP2){
        power=(4000-400*LV);
    }else if(_SP3 < LV < _SP4){
        power=(4800-400*LV);
    }else if(LV >= _SP4){
        power=-400;
    }

    test = client->changeAcPower(power);

    //checking for correctness
    if (test == false){
        errorState();
    }

    // emit loopRepeat(); //QObject assertion mistake whilest compiling. Cannot triger slot through signal coming from said slot?


}

void Controller::errorState(){
    //disconnect(this, &Controller::loopRepeat, this, &Controller::controlLoop);
    qDebug() << "errror State was entered";
}
