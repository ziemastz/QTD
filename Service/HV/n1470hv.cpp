#include "n1470hv.h"

N1470HV::N1470HV(QObject *parent) : QObject(parent)
{
    systemType = N1470;
    linkType = LINKTYPE_USB_VCP;
    isConnect = false;
    chList[0] = 0;
    chList[1] = 1;
    chList[2] = 2;
    chList[3] = 3;

}

void N1470HV::set(QString _port, QString _baudRate, QString _lBusAddress)
{
    port = _port;
    baudRate = _baudRate;
    lBusAddress = _lBusAddress;
    emit debug("N1470:set() - Parameters set");
}

bool N1470HV::init()
{
    emit debug("N1470:init() - Initiation started");
    if(isConnect) {
        deinit();
    }
    strcpy(arg,QString(port+"_"+baudRate+"_8BIT_NONE_1_"+lBusAddress).toLocal8Bit().data());
    strcpy(userName,"admin");
    strcpy(passwd,"admin");

    res = CAENHV_InitSystem(systemType,linkType,arg,userName,passwd,&handle);

    if(res == CAENHV_OK) {
        isConnect = true;
        emit debug("N1470:init() - Succeeded");
    }else {
        isConnect = false;
        emit error("N1470:init() - Failed - Error: "+QString(CAENHV_GetError(handle)));
    }

    return isConnect;
}

void N1470HV::deinit()
{
    isConnect = false;
    emit debug("N1470:deinit() - Deinitiation");
    res = CAENHV_DeinitSystem(handle);
    if(res != CAENHV_OK)
        emit error("N1470:deinit() - Error: "+QString(CAENHV_GetError(handle)));
}

bool N1470HV::turnOff()
{
    if(isConnect) {
        l[0] = 0;
        l[1] = 0;
        l[2] = 0;
        l[3] = 0;
        res = CAENHV_SetChParam(handle,0,"Pw",4,chList,l);
        if(res == CAENHV_OK) {
            emit debug("N1470:turnOff() - Succeeded");
            return true;
        }else {
            emit error("N1470:turnOff() - Failed - Error: "+QString(CAENHV_GetError(handle)));
            return false;
        }
    }else {
        emit error("N1470:turnOff() - Disconnected");
        return false;
    }
}

bool N1470HV::turnOn()
{
    if(isConnect) {
        l[0] = 1;
        l[1] = 1;
        l[2] = 1;
        l[3] = 1;
        res = CAENHV_SetChParam(handle,0,"Pw",4,chList,l);
        if(res == CAENHV_OK) {
            emit debug("N1470:turnOn() - Succeeded");
            return true;
        }else {
            emit error("N1470:turnOn() - Failed - Error: "+QString(CAENHV_GetError(handle)));
            return false;
        }
    }else {
        emit error("N1470:turnOn() - Disconnected");
        return false;
    }
}

bool N1470HV::setVolt(float value, int ch)
{
    if(isConnect)
    {
        res = CAENHV_SetChParam(handle,0,"VSet",1,&chList[ch],&value);

        if(res == CAENHV_OK){
            emit debug("N1470:setVolt() - Succeeded CH:"+QString::number(ch)+" V:"+QString::number(value));
            return true;
        }else {
            emit error("N1470:setVolt() - Failed - Error: "+QString(CAENHV_GetError(handle))+" CH:"+QString::number(ch)+" V:"+QString::number(value));
            return false;
        }
    }else {
        emit error("N1470:setVolt() - Disconnected CH:"+QString::number(ch)+" V:"+QString::number(value));
        return false;
    }
}


float N1470HV::monVolt(int ch)
{
    float ret = -1;
    if(isConnect) {
        res = CAENHV_GetChParam(handle,0,"VMon",1,&chList[ch],&ret);
        if(res != CAENHV_OK) {
            emit error("N1470:monVolt() - Failed - Error "+QString(CAENHV_GetError(handle))+" CH:"+QString::number(ch));
        }else {
            //emit debug("N1470:monVolt() - Succeeded CH:"+QString::number(ch)+" V:"+QString::number(ret));
        }
    }else {
        emit error("N1470:monVolt() - Disconnected CH:"+QString::number(ch));
    }

    return ret;
}

float N1470HV::monAmpere(int ch)
{
    float ret = -1;
    if(isConnect) {
        res = CAENHV_GetChParam(handle,0,"IMonH",1,&chList[ch],&ret);
        if(res != CAENHV_OK) {
            emit error("N1470:monAmpere() - Failed - Error "+QString(CAENHV_GetError(handle))+" CH:"+QString::number(ch));
        }else {
           // emit debug("N1470:monAmpere() - Succeeded CH:"+QString::number(ch)+" A:"+QString::number(ret));
        }
    }else {
        emit error("N1470:monAmpere() - Disconnected CH:"+QString::number(ch));
    }

    return ret;
}

QString N1470HV::monStatus(int ch)
{
    if(isConnect) {
        unsigned long value;
        res = CAENHV_GetChParam(handle,0,"ChStatus",1,&chList[ch],&value);
        if(res != CAENHV_OK) {
            emit error("N1470:monStatus() - Failed - Error "+QString(CAENHV_GetError(handle))+" CH:"+QString::number(ch));
            return QString();
        }

        bitset<32> bitfield = bitset<32>(value);
        QString status;

        if(bitfield[0]){
            status.append("ON");
        }else {
            status.append("OFF");
        }
        if(bitfield[1])
            status.append("|Ramping Up");
        if(bitfield[2])
            status.append("|Ramping Down");
        if(bitfield[3])
            status.append("|In Overcurrent");
        if(bitfield[4])
            status.append("|In Overvoltage");
        if(bitfield[5])
            status.append("|In Undervoltage");
        if(bitfield[6])
            status.append("|In External Trip");
        if(bitfield[7])
            status.append("|In Max V");
        if(bitfield[8])
            status.append("|In External Disable");
        if(bitfield[9])
            status.append("|In Internal Trip");
        if(bitfield[10])
            status.append("|In Calibration Error");
        if(bitfield[11])
            status.append("|Unplugged");
        if(bitfield[13])
            status.append("|In OverVoltage Protection");
        if(bitfield[14])
            status.append("|In Power Fail");
        if(bitfield[15])
            status.append("|In Temperature Error");
        //emit debug("N1470:monStatus() - Succeeded CH:"+QString::number(ch)+" Status:"+status);
        return status;
    }else {
        emit error("N1470:monStatus() - Disconnected CH:"+QString::number(ch));
        return QString();
    }

}
