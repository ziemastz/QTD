#ifndef MEASUREMENTWORKER_H
#define MEASUREMENTWORKER_H

#include <QObject>
#include <QApplication>
#include <QVariantList>
#include <QVector>
#include <QList>
#include <QThread>
#include "math.h"

#include "global.h"
#include "Service/Report/report.h"
#include "Service/rack.h"


template <typename T>
QVariantList toVariantList( const QList<T> &list )
{
    QVariantList newList;
    foreach( const T &item, list )
        newList << item;

    return newList;
}

class MeasurementWorker : public QObject
{
    Q_OBJECT
public:
    explicit MeasurementWorker(QObject *parent = 0);
    ~MeasurementWorker();

    QVariantList hvAnodeBeta, hvFirstDynodeBeta, hvFocusingDynodeBeta, hvAnodeGamma;
    QVariantList resolvintTimeBeta, deadTimeBeta, delayTimeBeta, resolvingTimeGamma, deadTimeGamma;

signals:
    void started();
    void finished(QString idMeas, int status, QString error);
    void initHV();
    void initAnalyzer();
    void insertBlankVial();
    void insertSourceVial(QString id);
    void setTurnOn(bool turned);
    void setVoltage(float value, int ch);
    void setCommand(int command);
    void setTime(int value, int ch);
    void debug(QString text);
    void error(QString text);

    //MeasurementDialog
    void setStatusProcessMeasurementDialog(STATUSPROCESSMEASUREMENTDIALOG status);
    void setStatusParametersMeasurementDialog(STATUSPARAMETERSMEASUREMENTDIALOG status);
    void setStatusTrackMeasurementDialog(STATUSTRACKMEASUREMENTDIALOG status);
    void insertRowTableMeasurementDialog();
    void setRTTableMeasurementDialog(QStringList record);
    void setPMTableMeasurementDialog(QStringList record);

public slots:
    void init();
    void set(MEASUREMENTDATA measData, SERIESSOURCESDATA seriesData, PARAMETERSDATA paramData, QString location);

    void startedHV();
    void startedAnalyzer();
    void finishedHV();
    void finishedAnalyzer();

    void statusAnalyzer(QString stat, QVector<int> timeCh, QVector<unsigned long> counters);
    void statusPowerSupplyHV(QVector<QString> statCh, QVector<float> voltCh, QVector<float> ampCh);
    void setChargedHV(bool charged);

    void measurementBackground();
    void startBlankVial();
    void measurementSource();
    void startSourceVial();

    void nextPoint();
    void nextReps();
    void waitForRecord();

    void quit();
    void addError(QString error);
private:
    Report *report;

    bool isStartedHV, isStartedAnalyzer, isTurnOnHV, isChargedHV, isStoppedCounting;
    bool endPoint, endReps;
    QString startedHour;
    QString lastError;
    QStringList generatorRecordRT();
    QStringList generatorRecordPM();


    QString id;
    int bgTime;
    int soTime;
    int reps;
    int leftTime;

    STATUSPARAMETERSMEASUREMENTDIALOG statusParameters;
    STATUSTRACKMEASUREMENTDIALOG statusTrack;
    STATUSPROCESSMEASUREMENTDIALOG statusProcess;

    bool finishedBox,answerBox,stop;

    QStringList idVials;
    QList<double> masses;

    int upperWindowGamma, lowerWindowGamma;
    TIMELIMITED timeLimited;
    CHANNELLIMITED channelLimited;
    int valueCountLimited;
    double valueUncertaintlyLimited;

    QVector<unsigned long> countersStatus;

    QVector<Rack *> axle;
    int currentPoint, maxPoint;
    int currentReps;
    int currentSource;
    QStringList tagPoints;

    void resetPoint();

    bool isChannelLimited();
};



#endif // MEASUREMENTWORKER_H
