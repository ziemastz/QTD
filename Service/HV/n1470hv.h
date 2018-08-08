#ifndef N1470HV_H
#define N1470HV_H

#include <QObject>
#include "CAENHVWrapper.h"
#include <bitset>

using namespace std;
class N1470HV : public QObject
{
    Q_OBJECT
public:
    explicit N1470HV(QObject *parent = 0);
    void set(QString _port, QString _baudRate, QString _lBusAddress);
    bool init();
    void deinit();

    bool turnOff();
    bool turnOn();

    bool setVolt(float value, int ch);
    float monVolt(int ch);
    float monAmpere(int ch);

    QString monStatus(int ch);

signals:
    void error(QString text);
    void debug(QString text);

public slots:

private:
    QString port;
    QString baudRate;
    QString lBusAddress;
    bool isConnect;

    CAENHVRESULT res;
    CAENHV_SYSTEM_TYPE_t systemType;
    int linkType, handle;
    char arg[30], userName[20], passwd[20];

    unsigned short chList[4];
    float f[4];
    unsigned long l[4];
};

#endif // N1470HV_H
