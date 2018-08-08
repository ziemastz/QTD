#include "powersupplyhv.h"

PowerSupplyHV::PowerSupplyHV(QObject *parent) : QObject(parent)
{
    setVolt = QVector<float>(4);
    setVolt[0] = -10;
    setVolt[1] = -10;
    setVolt[2] = -10;
    setVolt[3] = -10;
    connect(&n1470,SIGNAL(debug(QString)),this,SIGNAL(debug(QString)));
    connect(&n1470,SIGNAL(error(QString)),this,SIGNAL(error(QString)));
    charged = false;
    isConnect = false;
}

PowerSupplyHV::~PowerSupplyHV()
{
    delete timer;
}

void PowerSupplyHV::init()
{
    if(name == "N1470") {
        if(n1470.init()) {
            isConnect = true;
            emit started();
            emit debug("PowerSupplyHV:init() - Started");
            checkStatus();

        }else {
            isConnect = false;
            emit stop();
            emit error("PowerSupplyHV:init() - Failed");
        }
    }else {
        isConnect = true;
        emit started();
        emit debug("PowerSupplyHV:init() - Disable");
        checkStatus();
    }
}

void PowerSupplyHV::set(MODULEDATA data,int delayedStart)
{
    name = data.name;
    delay = delayedStart;
    if(name == "N1470")
        n1470.set(data.port, data.baudRate, QString::number(data.lBusAddress));

}

void PowerSupplyHV::quit()
{
    if(!isConnect) {
        return;
    }
    if(name == "N1470") {
        n1470.turnOff();
        n1470.deinit();
    }
    emit finished();
    emit debug("PowerSupplyHV:quit() - Finished");
}

void PowerSupplyHV::setTurnOn(bool turned)
{
    if(name == "N1470") {
        int cntError = 0;
        isTurned = turned;
        if(turned) {
            while(!n1470.turnOn()) {
                n1470.deinit();
                n1470.init();
                cntError++;
                if(cntError>10) {
                    n1470.turnOff();
                    n1470.deinit();
                    emit stop();
                    emit debug("PowerSupplyHV:setTurnOn(true) - Stopped");
                    break;
                }
            }

        }else {
            while(!n1470.turnOff()) {
                n1470.deinit();
                n1470.init();
                cntError++;
                if(cntError>10) {
                    n1470.turnOff();
                    n1470.deinit();
                    emit stop();
                    emit debug("PowerSupplyHV:setTurnOn(false) - Stopped");
                    break;
                }
            }
        }

    }else {
        isTurned = turned;
    }
}

void PowerSupplyHV::setVoltage(float value, int ch)
{
    setVolt[ch] = value;
    if(name == "N1470") {
        int cntError = 0;
        while(!n1470.setVolt(value,ch)) {
            n1470.deinit();
            n1470.init();
            cntError++;
            if(cntError>10) {
                emit stop();
                emit debug("PowerSupplyHV:setVoltage() - Stopped");
                break;
            }
        }
        charged = false;
        emit isCharged(charged);
    }
}

void PowerSupplyHV::test()
{
    if(name == "N1470") {
        if(n1470.init()) {
            emit tested(true);
            emit debug("PowerSupplyHV:test() - Started");
            n1470.deinit();
        }else {
            emit tested(false);
            emit error("PowerSupplyHV:test() - Failed");
        }
    }else {
        emit tested(true);
        emit error("PowerSupplyHV:test() - Disable");
    }
}

void PowerSupplyHV::checkStatus()
{
    if(isConnect) {
        emit status(monStatus(),monVoltage(),monAmpere());
        QTimer::singleShot(1000,this,SLOT(checkStatus()));
    }
}

QVector<float> PowerSupplyHV::monVoltage()
{
    QVector<float> ch(4);
    if(name == "N1470") {
        ch[ANODEBETA] = n1470.monVolt(0);
        ch[FIRSTDYNODEBETA] = n1470.monVolt(1);
        ch[FOCUSINGDYNODEBETA] = n1470.monVolt(2);
        ch[ANODEGAMMA] = n1470.monVolt(3);

        if((abs(ch[ANODEBETA] - setVolt[ANODEBETA]) < 5) && (abs(ch[FIRSTDYNODEBETA] - setVolt[FIRSTDYNODEBETA]) < 5) && (abs(ch[FOCUSINGDYNODEBETA] - setVolt[FOCUSINGDYNODEBETA]) < 5) && (abs(ch[ANODEGAMMA] - setVolt[ANODEGAMMA]) < 5)) {
            if(!charged) {
                emit isCharged(false);

                QThread::sleep(delay);

                charged = true;
                emit debug("PowerSupplyHV:monVoltage() - HV charged");
                emit isCharged(charged);
            }
        }else {
            if(charged) {
                charged = false;
                emit debug("PowerSupplyHV:monVoltage() - HV not charged");
                emit isCharged(charged);
            }
        }
    }else {
        ch[ANODEBETA] = setVolt[ANODEBETA];
        ch[FIRSTDYNODEBETA] = setVolt[FIRSTDYNODEBETA];
        ch[FOCUSINGDYNODEBETA] = setVolt[FOCUSINGDYNODEBETA];
        ch[ANODEGAMMA] = setVolt[ANODEGAMMA];
        if(!charged) {
            charged = true;
            emit debug("PowerSupplyHV:monVoltage() - HV charged");
            emit isCharged(charged);
        }
    }
    return ch;
}

QVector<float> PowerSupplyHV::monAmpere()
{
    QVector<float> ch(4);
    if(name == "N1470") {
        ch[ANODEBETA] = n1470.monAmpere(0);
        ch[FIRSTDYNODEBETA] = n1470.monAmpere(1);
        ch[FOCUSINGDYNODEBETA] = n1470.monAmpere(2);
        ch[ANODEGAMMA] = n1470.monAmpere(3);
    }
    return ch;
}

QVector<QString> PowerSupplyHV::monStatus()
{
    QVector<QString> ch(4);
    if(name == "N1470") {
        ch[ANODEBETA] = n1470.monStatus(0);
        ch[FIRSTDYNODEBETA] = n1470.monStatus(1);
        ch[FOCUSINGDYNODEBETA] = n1470.monStatus(2);
        ch[ANODEGAMMA] = n1470.monStatus(3);
        if(isTurned) {
            if(ch.contains("OFF")) {
                setTurnOn(true);
            }
        }else {
            if(ch.contains("ON")) {
                setTurnOn(false);
            }
        }
    }else {
        ch[ANODEBETA] = tr("Disable");
        ch[FIRSTDYNODEBETA] = tr("Disable");
        ch[FOCUSINGDYNODEBETA] = tr("Disable");
        ch[ANODEGAMMA] = tr("Disable");
        if(isTurned) {
            ch[ANODEBETA] = tr("ON");
            ch[FIRSTDYNODEBETA] = tr("ON");
            ch[FOCUSINGDYNODEBETA] = tr("ON");
            ch[ANODEGAMMA] = tr("ON");
        }else {
            ch[ANODEBETA] = tr("OFF");
            ch[FIRSTDYNODEBETA] = tr("OFF");
            ch[FOCUSINGDYNODEBETA] = tr("OFF");
            ch[ANODEGAMMA] = tr("OFF");
        }
    }
    return ch;
}
