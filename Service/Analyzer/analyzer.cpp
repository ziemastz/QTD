#include "analyzer.h"

Analyzer::Analyzer(QObject *parent) : QObject(parent)
{
    connect(&bad1,SIGNAL(debug(QString)),this,SIGNAL(debug(QString)));
    connect(&bad1,SIGNAL(error(QString)),this,SIGNAL(error(QString)));
    connect(&bad2,SIGNAL(debug(QString)),this,SIGNAL(debug(QString)));
    connect(&bad2,SIGNAL(error(QString)),this,SIGNAL(error(QString)));
}

Analyzer::~Analyzer()
{
    delete timer;
}

void Analyzer::init()
{
    if(name == "BAD1") {
        if(bad1.init()) {
            emit started();
            emit debug("Analyzer:init() - Started");
            currentStatus = tr("Connected");
            checkStatus();
        }else {
            currentStatus = tr("Disconnect");
            emit stop();
            emit error("Analyzer:init() - Failed");
        }
    }else if(name == "BAD2") {
        if(bad2.init()) {
            emit started();
            emit debug("Analyzer:init() - Started");
            currentStatus = tr("Connected");
            checkStatus();
        }else {
            currentStatus = tr("Disconnect");
            emit stop();
            emit error("Analyzer:init() - Failed");
        }
    }else {
        currentStatus = tr("Disconnect");
        emit stop();
        emit error("Analyzer:init() - Failed");
    }
}

void Analyzer::setTime(int value, int ch)
{
    if(name == "BAD1") {
        switch (ch) {
        case CH_RESOLVINGTIMEBETA: {
            bad1.setTime(value,24);
            break;
        }
        case CH_RESOLVINGTIMEGAMMA: {
            bad1.setTime(value,36);
            break;
        }
        case CH_DEADTIMEBETA: {
            bad1.setTime(value,60);
            break;
        }
        case CH_DEADTIMEGAMMA: {
            bad1.setTime(value,165);
            break;
        }
        case CH_DELAYTIMEBETA: {
            bad1.setTime(value,90);
            break;
        }
        default:
            break;
        }
    }else if(name == "BAD2") {
        switch (ch) {
        case CH_RESOLVINGTIMEBETA: {
            bad2.setTime(value,24);
            break;
        }
        case CH_RESOLVINGTIMEGAMMA: {
            bad2.setTime(value,36);
            break;
        }
        case CH_DEADTIMEBETA: {
            bad2.setTime(value,60);
            break;
        }
        case CH_DEADTIMEGAMMA: {
            bad2.setTime(value,165);
            break;
        }
        case CH_DELAYTIMEBETA: {
            bad2.setTime(value,90);
            break;
        }
        default:
            break;
        }
    }
}

void Analyzer::test()
{
    if(name == "BAD1") {
        if(bad1.init()) {
            emit tested(true);
            emit debug("Analyzer:test() - Started");
            bad1.deinit();
        }else {
            emit tested(false);
            emit error("Analyzer:test() - Failed");
        }
    }else if(name == "BAD2") {
        if(bad2.init()) {
            emit tested(true);
            emit debug("Analyzer:test() - Started");
            bad2.deinit();
        }else {
            emit tested(false);
            emit error("Analyzer:test() - Failed");
        }
    }else {
        emit tested(false);
        emit error("Analyzer:test() - Failed");
    }
}

void Analyzer::set(MODULEDATA data)
{
    name = data.name;
    if(name == "BAD1"){
        bad1.set(data.serialNameUSB);
    }else if(name == "BAD2"){
        bad2.set(data.serialNameUSB);
    }
}

void Analyzer::quit()
{
    if(currentStatus == tr("Disconnect"))
        return;

    if(name == "BAD1") {
        bad1.setCommand(102);
        bad1.deinit();
    }else if(name == "BAD2") {
        bad2.setCommand(102);
        bad2.deinit();
    }
    currentStatus = tr("Disconnect");
    emit finished();
    emit debug("Analyzer:init() - Finished");
}

void Analyzer::setCommand(int command)
{
    if(name == "BAD1") {
        switch(command) {
        case C_START: {
            if(bad1.setCommand(66))
                currentStatus = tr("Started");
            break;
        }
        case C_STOP: {
            if(bad1.setCommand(102)) {
                currentStatus = tr("Stopped");
                emit status(monStatus(), monTime(), monCounters());
            }
            break;
        }
        case C_RESET: {
            if(bad1.setCommand(129)) {
                currentStatus = tr("Reset");
            }
            break;
        }
        case C_GETDATA: {
            bad1.setCommand(126);
            break;
        }
        }
    }else if(name == "BAD2") {
        switch(command) {
        case C_START: {
            if(bad2.setCommand(66))
                currentStatus = tr("Started");
            break;
        }
        case C_STOP: {
            if(bad2.setCommand(102)) {
                currentStatus = tr("Stopped");
                emit status(monStatus(), monTime(), monCounters());
            }
            break;
        }
        case C_RESET: {
            if(bad2.setCommand(129)) {
                currentStatus = tr("Reset");
            }
            break;
        }
        case C_GETDATA: {
            bad2.setCommand(126);
            break;
        }
        case C_ENAEXTCLK: {
            bad2.setCommand(70);
            break;
        }
        case C_DISEXTCLK: {
            bad2.setCommand(75);
            break;
        }
        }
    }


}

void Analyzer::checkStatus()
{
    if(currentStatus != tr("Disconnect")) {
        emit status(monStatus(), monTime(), monCounters());
        QTimer::singleShot(550, this, SLOT(checkStatus()));
    }
}

QString Analyzer::monStatus()
{
    return currentStatus;
}

QVector<int> Analyzer::monTime()
{
    if(name == "BAD1") {
        return bad1.currSetTime;
    }else if(name == "BAD2") {
        return bad2.currSetTime;
    }else {
        return QVector<int>(5);
    }
}

QVector<unsigned long> Analyzer::monCounters()
{
    if(name == "BAD1") {
        return bad1.getData();
    }else if(name == "BAD2") {
        return bad2.getData();
    }else {
        return QVector<unsigned long>();
    }
}
