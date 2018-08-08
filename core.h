#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QCoreApplication>
#include <QFileInfo>
#include <QSettings>
#include <QDateTime>
#include <QDebug>
#include <QTimer>
#include <QSerialPortInfo>
#include <QCryptographicHash>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QSignalBlocker>

#include "global.h"
#include "Service/Report/report.h"
#include "Service/DB/database.h"
#include "Service/measurementworker.h"
#include "Service/monitorworker.h"
#include "Service/Analyzer/analyzer.h"
#include "Service/HV/powersupplyhv.h"

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core(QObject *parent = 0);
    void init();
    QSettings *regSet;
    QString language();
    void setObject(MeasurementWorker *meas, MonitorWorker *mon, PowerSupplyHV *hv, Analyzer *ana);

signals:
    void acceptedLoginUser();
    void rejectedLoginuser();

    //MainWindow
    void setSettingsMainWindow(MAINWINDOWSETTINGS settings);
    void setDisplayDateTime(DISPLAYDATETIME dateTime);
    void setDisplayMeasurementStatistics(DISPLAYSTATISTICS statistics);
    void setMeasurementRegisterTable(QList<QStringList> records);
    void setLastStandardizationTable(QList<QStringList> records);

    //SettingsDialog
    void setSettingsSettingsDialog(SETTINGSDIALOGSETTINGS settings);
    void availableNewLogin(bool available);
    void setSettingsUser(USERDATA data);
    void confirmedPassword(bool confirmed);


    //NewMeasurementDialog
    void setSettingsNewMeasurementDialog(NEWMEASUREMENTDIALOGSETTINGS settings);
    void showNewMeasurementDialog(MEASUREMENTDATA data);

    //SeriesSourcesDialog
    void setSettingsSeriesSourcesDialog(SERIESSOURCESDIALOGSETTINGS settings);
    void setDataSeriesSourceDialog(SERIESSOURCESDATA data);

    //ParametersDialog
    void setSettingsParametersDialog(PARAMETERSDIALOGSETTINGS settings);
    void setDataParametersDialog(PARAMETERSDATA data);

    //MeasurementDialog
    void showMeasurementDialog(MEASUREMENTDIALOGSETTINGS settings);
    void showFinishedBoxMeasurementDialog(QString text, QString time);

    //MeasurementWorker
    void setPowerSupplyHV(MODULEDATA data, int delayedStart);
    void setAnalyzer(MODULEDATA data);
    void setMeasurement(MEASUREMENTDATA meas, SERIESSOURCESDATA series, PARAMETERSDATA param, QString location);
    void startMeasurement();
    void testPowerSupply();
    void testAnalyzer();
    void finishedMeasThread();

    //MonitorDialog
    void showMonitorDialog();

    //MonitorWorker
    void startMonitor();

public slots:
    //MainWindow
    void searchMeasurementMainWindow(QString arg);
    void savePositionMainWindow(QPoint p);
    void saveSizemainWindow(QSize s);
    void saveMaxmizedMainWindow(bool m);
    void repeatMeasurementMainWindow(QString id);
    void deleteReportMainWindow(QStringList ids);
    void openReportLocationMainWindow(QString id);
    void clickedMonitorButtonMainWindow();

    //LoginDialog
    void setLoginUser(QString login, QString password);

    //SettingsDialog
    void saveSettingsDialog(SETTINGSDIALOGSETTINGS settings);
    void testConnection(MODULEDATA data);
    void checkAvailableNewLogin(QString login);
    void saveUserData(USERDATA data);
    void setUsername(QString login);
    void removeUserData(QString login);
    void confirmPassword(QString login, QString password);
    void changePasswordUser(QString login, QString password);

    //NewMeasurementDialog
    void initMeasurement(MEASUREMENTDATA data);
    void changedNuclideNewMeasurementDialog(QString nuclide);

    //SeriesSourceDialog
    void getDataSeriesSources(QString name);
    void removeSeriesSources(QString name);
    void saveSeriesSources(SERIESSOURCESDATA data);

    //ParametersDialog
    void getDataParameters(QString name);
    void removeParameters(QString name);
    void saveParameters(PARAMETERSDATA data);

    //MeasurementWorker
    void finishedStatus(QString idMeas, int status, QString lastError = QString());

    //MonitorDialog
    void saveToFileMonitorData(MONITORDIALOGDATA data);
    void finishedMonitor();

private slots:
    void timeoutHalfSecondTimer();

private:

    Database *db;
    QTimer *halfSecondTimer;
    QTime *timeMeasurement;
    MeasurementWorker *measObj;
    MonitorWorker *monObj;
    PowerSupplyHV *hvObj;
    Analyzer *anaObj;

    USERDATA loginUser;
    MAINWINDOWSETTINGS settingsMainWindow;
    DISPLAYDATETIME currentDateTime;
    DISPLAYSTATISTICS measurementStatistics;
    SETTINGSDIALOGSETTINGS settingsSettingsDialog;
    NEWMEASUREMENTDIALOGSETTINGS settingsNewMeasDialog;
    SERIESSOURCESDIALOGSETTINGS settingsSeriesSourcesDialog;
    PARAMETERSDIALOGSETTINGS settingsParametersDialog;
    MEASUREMENTDIALOGSETTINGS settingsMeasurementDialog;

    inline QString lastModificationDateTimeBy();
    inline QString generatorCryptographicHash(QString text) {
        return QString(QCryptographicHash::hash(text.toUtf8(),QCryptographicHash::Sha3_512).toHex());
    }

    QString generatorStrTotalTime(qint64 msecs) {
       QDateTime dateTime;
       dateTime.setDate(QDate(2000,1,1));
       dateTime.setTime(QTime(0,0,0));
       dateTime = dateTime.addMSecs(msecs);
       return QString(QString::number(dateTime.date().year()-2000)+"y "+QString::number(QDate(dateTime.date().year(),1,1).daysTo(dateTime.date()))+"d "+QString::number(dateTime.time().hour())+"h "+QString::number(dateTime.time().minute())+"m "+QString::number(dateTime.time().second())+"s");
    }
    QSignalBlocker *signalsBlock;
    void signalStatusDisabler(bool block);
};

#endif // CORE_H
