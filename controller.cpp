#include "controller.h"




Controller::Controller(float SP1, float SP2, float SP3, float SP4, QObject* parent)
    :QObject(parent),_SP1(SP1),_SP2(SP2),_SP3(SP3),_SP4(SP4)
{
    _m1= (-400)/(_SP2 - _SP1);
    _m2= (-400)/(_SP4 - _SP3);
    _t1= (-_m1)*_SP2;
    _t2= (-_m2)*_SP3;


}

qint16 Controller::controlLoop(float voltage){
    qint16 power = 0;


    if(voltage <= _SP1){
        power=400;
    }else if(_SP1 < voltage && voltage < _SP2){
        power=static_cast<qint16>(_m1*voltage+_t1);
    }else if(_SP3 < voltage && voltage < _SP4){
        power=static_cast<qint16>(_m2*voltage+_t2);
    }else if(voltage >= _SP4){
        power=-400;
    }

    return power;
}

