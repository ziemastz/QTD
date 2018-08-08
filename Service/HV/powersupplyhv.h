#ifndef POWERSUPPLYHV_H
#define POWERSUPPLYHV_H

#define ANODEBETA 0
#define FIRSTDYNODEBETA 1
#define FOCUSINGDYNODEBETA 2
#define ANODEGAMMA 3

#include <QObject>
#include <QVector>
#include <QTimer>
#include <QThread>
#include "global.h"
#include "Service/HV/n1470hv.h"


class PowerSupplyHV : public QObject
{
    Q_OBJECT
public:
    explicit PowerSupplyHV(QObject *parent = 0);
    ~PowerSupplyHV();
signals:
    void started();
    void finished();
    void stop();
    void debug(QString text);
    void error(QString text);
    void status(QVector<QString> statCh, QVector<float> voltCh, QVector<float> ampCh);
    void isCharged(bool charged);
    void tested(bool res);

public slots:
    void init();
    void set(MODULEDATA data, int delayedStart);
    void quit();

    void setTurnOn(bool turned);
    void setVoltage(float value, int ch);

    void test();

private slots:
    //N1470 
    void checkStatus();

private:
    QString name;
    N1470HV n1470;
    QTimer *timer;
    bool isConnect, isTurned;
    bool charged;
    int delay;
    QVector<float> setVolt;
    QVector<float> monVoltage();
    QVector<float> monAmpere();
    QVector<QString> monStatus();

};

#endif // POWERSUPPLYHV_H
