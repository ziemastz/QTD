#ifndef MONITORWORKER_H
#define MONITORWORKER_H

#include <QObject>
#include <QApplication>
#include <QDebug>
#include <QThread>
#include "global.h"

class MonitorWorker : public QObject
{
    Q_OBJECT
public:
    explicit MonitorWorker(QObject *parent = 0);

signals:
    void finished();
    void initHV();
    void initAnalyzer();

    void setTurnOn(bool turned);
    void setHV(float volt, int ch);

    void setCommand(int command);

    //MonitorDialog
    void setStatusParametersMonitorDialog(STATUSPARAMETERSMEASUREMENTDIALOG status);
    void setStatusTrackMonitorDialog(STATUSTRACKMEASUREMENTDIALOG status);
    void insertRowTableMonitorDialog();
    void setRTTableMonitorDialog(QStringList record);
    void setPMTableMonitorDialog(QStringList record);
    void stoppedMeas();

public slots:
    void start();
    void quit();

    void startMeas(int time);
    void stopMeas();
    void resetMeas();

    void startedHV();
    void startedAnalyzer();
    void finishedHV();
    void finishedAnalyzer();

    void statusAnalyzer(QString stat, QVector<int> timeCh, QVector<unsigned long> counters);
    void statusPowerSupplyHV(QVector<QString> statCh, QVector<float> voltCh, QVector<float> ampCh);

    void debug(QString text);
    void error(QString text);

private:
    void clear();

    bool isStartedHV, isStartedAnalyzer,stop,isStoppedCounting;
    QVector<unsigned long> countersStatus, beforCounterStatus, beforbeforCounterStatus;
    int timeMeas;

    STATUSPARAMETERSMEASUREMENTDIALOG statusParameters;
    STATUSTRACKMEASUREMENTDIALOG statusTrack;

    QStringList generatorRecordRT();
    QStringList generatorRecordPM();
};

#endif // MONITORWORKER_H
