#include "bad2.h"

BAD2::BAD2(QObject *parent) : QObject(parent)
{
    isConnect = false;
    currSetTime = QVector<int>(5);
}

void BAD2::set(QString _serialName)
{
    serialNumber = strdup(_serialName.toStdString().c_str());
    emit debug("BAD2:set() - Parameters set");
}

bool BAD2::init()
{
    emit debug("BAD2:init() - Initiation started");
    FT_STATUS ftStatus;

    if(isConnect) {
        deinit();
    }
    ftStatus = FT_OpenEx(serialNumber,FT_OPEN_BY_SERIAL_NUMBER,&ftHandle);
    if(ftStatus == FT_OK) {
        isConnect = true;
        emit debug("BAD2:init() - Succeeded");

        ftStatus = FT_Purge(ftHandle, FT_PURGE_RX | FT_PURGE_TX); // Purge both Rx and Tx buffers
        if (ftStatus == FT_OK) {
            emit debug("BAD2:init() - Purge OK");
        }
        else {
            emit error("BAD2:init() - Purge failed");
        }
        ftStatus = FT_SetTimeouts(ftHandle,1000,1000);
        if (ftStatus == FT_OK) {
            emit debug("BAD2:init() - SetTimeouts OK");
        }else {
            emit error("BAD2:init() - SetTimeouts failed");
        }
    }else {
        isConnect = false;
        emit error("BAD2:init() - Failed - Error: "+errorString(ftStatus));
    }
    return isConnect;
}

void BAD2::deinit()
{
    emit debug("BAD2:deinit() - Deinitiation");
    isConnect = false;
    FT_Close(ftHandle);
}

bool BAD2::setTime(int value, int ch)
{
    unsigned char cmd[3];
    int timeClk = int((float)value/7.5);
    if(timeClk > 8190) {
        timeClk = 8190;
    };
    value = timeClk*7.5;
    int b1 = timeClk/256;
    int b0 = timeClk - b1*256;

    cmd[0] = (unsigned char)ch; // COMMAND
    cmd[1] = (unsigned char)b0;
    cmd[2] = (unsigned char)b1;

    if(write(cmd)) {
        switch (ch) {
        case R1: {
            currSetTime[0] = value;
            emit debug(QString("BAD2:setTime() - Succeeded")+QString(" R1: ")+QString::number(value)+QString("ns"));
            break;
        }
        case R2: {
            currSetTime[1] = value;
            emit debug(QString("BAD2:setTime() - Succeeded")+QString(" R2: ")+QString::number(value)+QString("ns"));
            break;
        }
        case DT1: {
            currSetTime[2] = value;
            emit debug(QString("BAD2:setTime() - Succeeded")+QString(" DT1: ")+QString::number(value)+QString("ns"));
            break;
        }
        case DT2: {
            currSetTime[3] = value;
            emit debug(QString("BAD2:setTime() - Succeeded")+QString(" DT2: ")+QString::number(value)+QString("ns"));
            break;
        }
        case DLB: {
            currSetTime[4] = value;
            emit debug(QString("BAD2:setTime() - Succeeded")+QString(" DLB: ")+QString::number(value)+QString("ns"));
            break;
        }
        default:
            emit error("BAD2:setTime() - Error: Incorrect command");
            break;
        }
        return true;
    }else {
        emit error("BAD2:setTime() - Failed");
        return false;
    }
}

bool BAD2::setCommand(int com)
{
    unsigned char cmd[3];
    cmd[0] = (unsigned char)com;
    cmd[1] = 0;
    cmd[2] = 0;
    if(write(cmd)) {
        emit debug("BAD2:setCommand() - Succeeded");
        return true;
    }else {
        emit error("BAD2:setCommand() - Failed");
        return false;
    }
}

QVector<unsigned long> BAD2::getData()
{
    QVector<unsigned long> counters;
    if(setCommand(126)) {
        FT_STATUS ftStatus;
        int maxBytes = 228;
        DWORD BytesReceived = 0;
        unsigned char tmp[maxBytes];

        //emit debug("BAD2:getData() - Started reading");
        ftStatus = FT_Read(ftHandle,tmp,maxBytes,&BytesReceived);
        if (ftStatus == FT_OK) {
            if ((int)BytesReceived == maxBytes) {
                // FT_Read OK
                //emit debug("BAD2:getData() - Succeeded");
                for(int i=0;i<(int)(maxBytes/4);i++)
                {
                    unsigned long b3 = (int)tmp[i*4];
                    unsigned long b2 = (int)tmp[i*4 + 1];
                    unsigned long b1 = (int)tmp[i*4 + 2];
                    unsigned long b0 = (int)tmp[i*4 + 3];

                    unsigned long value = b0 + b1*(unsigned long)(256) + b2*(unsigned long)(256)*(unsigned long)(256) + b3*(unsigned long)(256)*(unsigned long)(256)*(unsigned long)(256);

                    counters << value;
                }
               // emit debug("BAD2:getData() - Received "+QString::number(counters.count())+" counters");

            }else {
                // FT_Read Timeout
                emit error("BAD2:readData() - Failed (Timeout) - Bytes received: "+QString::number(BytesReceived)+" - Error: "+errorString(ftStatus));
            }
        }else {
            // FT_Read Failed
            emit error("BAD2:readData() -  Failed - Error: "+errorString(ftStatus));
        }

    }
    return counters;
}

bool BAD2::write(unsigned char *cmd)
{
    DWORD BytesWritten;
    FT_STATUS ftStatus;

    ftStatus = FT_Write(ftHandle,cmd,sizeof(cmd),&BytesWritten);
    if(ftStatus == FT_OK) {
        if(BytesWritten == sizeof(cmd)) {
            //emit debug("BAD2:write() OK");
            return true;
        }else {
            emit error("BAD2:write() - Failed (Timeout) - Error: "+errorString(ftStatus));
            return false;
        }
    }else {
        emit error("BAD2:write() - Failed - Error: "+errorString(ftStatus));
        return false;
    }
}

QString BAD2::errorString(FT_STATUS status)
{
    switch(status) {
    case FT_OK:
    {
        return tr("No error");
    }
    case FT_INVALID_HANDLE:
    {
        return tr("Invalid handle");
    }
    case FT_DEVICE_NOT_FOUND:
    {
        return tr("Device not found");
    }
    case FT_DEVICE_NOT_OPENED:
    {
        return tr("Device not opened");
    }
    case FT_IO_ERROR:
    {
        return tr("IO error");
    }
    case FT_INSUFFICIENT_RESOURCES:
    {
        return tr("Insufficient resources");
    }
    case FT_INVALID_PARAMETER:
    {
        return tr("FT_INVALID_PARAMETER");
    }
    case FT_INVALID_BAUD_RATE:
    {
        return tr("FT_INVALID_BAUD_RATE");
    }
    case FT_DEVICE_NOT_OPENED_FOR_ERASE:
    {
        return tr("FT_DEVICE_NOT_OPENED_FOR_ERASE");
    }
    case FT_DEVICE_NOT_OPENED_FOR_WRITE:
    {
        return tr("FT_DEVICE_NOT_OPENED_FOR_WRITE");
    }
    case FT_FAILED_TO_WRITE_DEVICE:
    {
        return tr("FT_FAILED_TO_WRITE_DEVICE");
    }
    case FT_EEPROM_READ_FAILED:
    {
        return tr("FT_EEPROM_READ_FAILED");
    }
    case FT_EEPROM_WRITE_FAILED:
    {
        return tr("FT_EEPROM_WRITE_FAILED");
    }
    case FT_EEPROM_ERASE_FAILED:
    {
        return tr("FT_EEPROM_ERASE_FAILED");
    }
    case FT_EEPROM_NOT_PRESENT:
    {
        return tr("FT_EEPROM_NOT_PRESENT");
    }
    case FT_EEPROM_NOT_PROGRAMMED:
    {
        return tr("FT_EEPROM_NOT_PROGRAMMED");
    }
    case FT_INVALID_ARGS:
    {
        return tr("FT_INVALID_ARGS");
    }
    case FT_NOT_SUPPORTED:
    {
        return tr("FT_NOT_SUPPORTED");
    }
    case FT_OTHER_ERROR:
    {
        return tr("FT_OTHER_ERROR");
    }
    case FT_DEVICE_LIST_NOT_READY:
    {
        return tr("FT_DEVICE_LIST_NOT_READY");
    }
    default:
        return tr("Number of error: ")+QString::number(status);
    }
}
