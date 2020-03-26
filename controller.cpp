#include "controller.h"




Controller::Controller(int SP1, int SP2, int SP3, int SP4, QObject* parent)
    :QObject(parent),_SP1(SP1),_SP2(SP2),_SP3(SP3),_SP4(SP4)
{

/*
    //connect(this, &Controller::loopRepeat, this, &Controller::controlLoop);
    //controlLoop();

    timervoltage = new QTimer(this);
   // connect(timervoltage, &QTimer::timeout, this, &Controller::readvoltageloop);
    timervoltage->start(10000);

    timerAC = new QTimer(this);
   // connect(timerAC, &QTimer::timeout, this, &Controller::chargeLoop);
    timerAC->start(30000);
    */

}

float Controller::controlLoop(float voltage){
    float power = 0;

    /*
     * missing: rounding power, control dependenet on SP
     *
    if(voltage <= _SP1){
        power=400;
    }else if(_SP1 < voltage < _SP2){
        power=(4000-400*voltage);
    }else if(_SP3 < voltage < _SP4){
        power=(4800-400*voltage);
    }else if(voltage >= _SP4){
        power=-400;
    }
    */
    return power;
}

