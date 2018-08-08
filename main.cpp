#include "mainwindow.h"
#include "core.h"
#include "Dialog/logindialog.h"
#include "Service/measurementworker.h"
#include "Service/monitorworker.h"
#include "Service/HV/powersupplyhv.h"
#include "Service/Analyzer/analyzer.h"
#include <QtGlobal>
#include <QApplication>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QDesktopServices>
#include <QDebug>
#include <QTranslator>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
  {
      QByteArray localMsg = msg.toLocal8Bit();
      QString txt;
      QDateTime date;

      QFile outFile(QDir::currentPath() + "/log.txt");
      if(outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
      {
          QTextStream out(&outFile);
          switch (type) {
          case QtDebugMsg:
              txt.append(QString("Debug: %1").arg(localMsg.constData()));
              break;
          case QtInfoMsg:
              txt.append(QString("Info: %1").arg(localMsg.constData()));
              break;
          case QtWarningMsg:
              txt.append(QString("Warning: %1").arg(localMsg.constData()));
              break;
          case QtCriticalMsg:
              txt.append(QString("Critical: %1").arg(localMsg.constData()));
              break;
          case QtFatalMsg:
              txt.append(QString("Fatal: %1").arg(localMsg.constData()));
              abort();
          }
          txt.prepend(" - ");
          txt.prepend(date.currentDateTime().toString("dd-MM-yyyy hh:mm:ss.zzz"));
          txt.append("\n");
          out << txt;
          outFile.close();
      }
  }
int main(int argc, char *argv[])
{
    QFile outFile(QDir::currentPath() + "/log.txt");
    outFile.remove();
    qInstallMessageHandler(myMessageOutput);
    qDebug() << "main(): Run app";
    QApplication a(argc, argv);

    qRegisterMetaType<MAINWINDOWSETTINGS>("MAINWINDOWSETTINGS");
    qRegisterMetaType<DISPLAYDATETIME>("DISPLAYDATETIME");
    qRegisterMetaType<DISPLAYSTATISTICS>("DISPLAYSTATISTICS");
    qRegisterMetaType<SETTINGSDIALOGSETTINGS>("SETTINGSDIALOGSETTINGS");
    qRegisterMetaType<USERDATA>("USERDATA");
    qRegisterMetaType<NEWMEASUREMENTDIALOGSETTINGS>("NEWMEASUREMENTDIALOGSETTINGS");
    qRegisterMetaType<MEASUREMENTDATA>("MEASUREMENTDATA");
    qRegisterMetaType<SERIESSOURCESDIALOGSETTINGS>("SERIESSOURCESDIALOGSETTINGS");
    qRegisterMetaType<SERIESSOURCESDATA>("SERIESSOURCESDATA");
    qRegisterMetaType<PARAMETERSDIALOGSETTINGS>("PARAMETERSDIALOGSETTINGS");
    qRegisterMetaType<PARAMETERSDATA>("PARAMETERSDATA");
    qRegisterMetaType<MEASUREMENTDIALOGSETTINGS>("MEASUREMENTDIALOGSETTINGS");
    qRegisterMetaType<MODULEDATA>("MODULEDATA");
    qRegisterMetaType<STATUSPROCESSMEASUREMENTDIALOG>("STATUSPROCESSMEASUREMENTDIALOG");
    qRegisterMetaType<STATUSPARAMETERSMEASUREMENTDIALOG>("STATUSPARAMETERSMEASUREMENTDIALOG");
    qRegisterMetaType<STATUSTRACKMEASUREMENTDIALOG>("STATUSTRACKMEASUREMENTDIALOG");
    qRegisterMetaType<MONITORDIALOGDATA>("MONITORDIALOGDATA");

    qRegisterMetaType<QVector<QString> >("QVector<QString>");
    qRegisterMetaType<QVector<int> >("QVector<int>");
    qRegisterMetaType<QVector<unsigned long> >("QVector<unsigned long>");
    qRegisterMetaType<QVector<float> >("QVector<float>");
    qRegisterMetaType<QList<QStringList> >("QList<QStringList>");
    QFile f(":Image/style.qss");
    if (!f.exists())
    {
        qWarning() << ("main(): Unable to set stylesheet, file not found: ")+f.fileName();
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }

    Core c;
    QTranslator translator;
    const QString lang = c.language();

    if(translator.load(":Translations/Translations/qtd_"+lang)){
        a.installTranslator(&translator);
        qDebug() << "main(): Install translation: "+lang;
    }else {
        qDebug() << "main(): Not install translation: "+lang;
    }

    MainWindow w;
    LoginDialog l;
    a.connect(&l,SIGNAL(setLoginUser(QString,QString)),&c,SLOT(setLoginUser(QString,QString)));
    a.connect(&c,SIGNAL(acceptedLoginUser()),&l,SLOT(accept()));
    a.connect(&c,SIGNAL(rejectedLoginuser()),&l,SLOT(rejectedLoginuser()));
    if(l.exec() != QDialog::Accepted) {
        qDebug() << "main(): Canceled login";
        return 0;
    }


    //MainWindow
    a.connect(&c,SIGNAL(setSettingsMainWindow(MAINWINDOWSETTINGS)),&w,SLOT(setSettingsMainWindow(MAINWINDOWSETTINGS)));
    a.connect(&c,SIGNAL(setDisplayDateTime(DISPLAYDATETIME)),&w,SLOT(setDisplayDateTime(DISPLAYDATETIME)));
    a.connect(&c,SIGNAL(setDisplayMeasurementStatistics(DISPLAYSTATISTICS)),&w,SLOT(setDisplayMeasurementStatistics(DISPLAYSTATISTICS)));
    a.connect(&c,SIGNAL(setMeasurementRegisterTable(QList<QStringList>)),&w,SLOT(setMeasurementRegisterTable(QList<QStringList>)));
    a.connect(&c,SIGNAL(setLastStandardizationTable(QList<QStringList>)),&w,SLOT(setLastStandardizationTable(QList<QStringList>)));
    a.connect(&w,SIGNAL(searchMeasurement(QString)),&c,SLOT(searchMeasurementMainWindow(QString)));
    a.connect(&w,SIGNAL(positionWindow(QPoint)),&c,SLOT(savePositionMainWindow(QPoint)));
    a.connect(&w,SIGNAL(sizeWindow(QSize)),&c,SLOT(saveSizemainWindow(QSize)));
    a.connect(&w,SIGNAL(maximizedWindow(bool)),&c,SLOT(saveMaxmizedMainWindow(bool)));
    a.connect(&w,SIGNAL(repeatMeasurement(QString)),&c,SLOT(repeatMeasurementMainWindow(QString)));
    a.connect(&c,SIGNAL(showNewMeasurementDialog(MEASUREMENTDATA)),&w,SLOT(showNewMeasurementDialog(MEASUREMENTDATA)));
    a.connect(&w,SIGNAL(deleteReport(QStringList)),&c,SLOT(deleteReportMainWindow(QStringList)));
    a.connect(&w,SIGNAL(openReportLocation(QString)),&c,SLOT(openReportLocationMainWindow(QString)));
    a.connect(&w,SIGNAL(clickedMonitorButton()),&c,SLOT(clickedMonitorButtonMainWindow()));

    //SettingsDialog
    a.connect(&c,SIGNAL(setSettingsSettingsDialog(SETTINGSDIALOGSETTINGS)),&w.settingsDialog,SLOT(setSettings(SETTINGSDIALOGSETTINGS)));
    a.connect(&w.settingsDialog,SIGNAL(settings(SETTINGSDIALOGSETTINGS)),&c,SLOT(saveSettingsDialog(SETTINGSDIALOGSETTINGS)));
    a.connect(&w.settingsDialog,SIGNAL(testConnection(MODULEDATA)),&c,SLOT(testConnection(MODULEDATA)));
    a.connect(&w.settingsDialog,SIGNAL(newLogin(QString)),&c,SLOT(checkAvailableNewLogin(QString)));
    a.connect(&c,SIGNAL(availableNewLogin(bool)),&w.settingsDialog,SIGNAL(availableNewLogin(bool)));
    a.connect(&w.settingsDialog,SIGNAL(saveUserData(USERDATA)),&c,SLOT(saveUserData(USERDATA)));
    a.connect(&w.settingsDialog,SIGNAL(removeUserData(QString)),&c,SLOT(removeUserData(QString)));
    a.connect(&w.settingsDialog,SIGNAL(confirmPassword(QString,QString)),&c,SLOT(confirmPassword(QString,QString)));
    a.connect(&c,SIGNAL(confirmedPassword(bool)),&w.settingsDialog,SIGNAL(confirmedPassword(bool)));
    a.connect(&w.settingsDialog,SIGNAL(changePasswordUser(QString,QString)),&c,SLOT(changePasswordUser(QString,QString)));
    a.connect(&c,SIGNAL(setSettingsUser(USERDATA)),&w.settingsDialog,SLOT(setSettingsUser(USERDATA)));
    a.connect(&w.settingsDialog,SIGNAL(setUsername(QString)),&c,SLOT(setUsername(QString)));

    //NewMeasurementDialog
    a.connect(&c,SIGNAL(setSettingsNewMeasurementDialog(NEWMEASUREMENTDIALOGSETTINGS)),&w.newMeasDialog,SLOT(setSettings(NEWMEASUREMENTDIALOGSETTINGS)));
    a.connect(&w.newMeasDialog,SIGNAL(startMeasurement(MEASUREMENTDATA)),&c,SLOT(initMeasurement(MEASUREMENTDATA)));
    a.connect(&w.newMeasDialog,SIGNAL(changedNuclide(QString)),&c,SLOT(changedNuclideNewMeasurementDialog(QString)));
    //SeriesSourcesDialog
    a.connect(&c,SIGNAL(setSettingsSeriesSourcesDialog(SERIESSOURCESDIALOGSETTINGS)),&w.newMeasDialog.seriesDialog,SLOT(setSettings(SERIESSOURCESDIALOGSETTINGS)));
    a.connect(&w.newMeasDialog.seriesDialog,SIGNAL(changedName(QString)),&c,SLOT(getDataSeriesSources(QString)));
    a.connect(&c,SIGNAL(setDataSeriesSourceDialog(SERIESSOURCESDATA)),&w.newMeasDialog.seriesDialog,SLOT(setData(SERIESSOURCESDATA)));
    a.connect(&w.newMeasDialog.seriesDialog,SIGNAL(remove(QString)),&c,SLOT(removeSeriesSources(QString)));
    a.connect(&w.newMeasDialog.seriesDialog,SIGNAL(save(SERIESSOURCESDATA)),&c,SLOT(saveSeriesSources(SERIESSOURCESDATA)));

    //ParametersDialog
    a.connect(&c,SIGNAL(setSettingsParametersDialog(PARAMETERSDIALOGSETTINGS)),&w.newMeasDialog.paramDialog,SLOT(setSettings(PARAMETERSDIALOGSETTINGS)));
    a.connect(&w.newMeasDialog.paramDialog,SIGNAL(changedName(QString)),&c,SLOT(getDataParameters(QString)));
    a.connect(&c,SIGNAL(setDataParametersDialog(PARAMETERSDATA)),&w.newMeasDialog.paramDialog,SLOT(setData(PARAMETERSDATA)));
    a.connect(&w.newMeasDialog.paramDialog,SIGNAL(remove(QString)),&c,SLOT(removeParameters(QString)));
    a.connect(&w.newMeasDialog.paramDialog,SIGNAL(save(PARAMETERSDATA)),&c,SLOT(saveParameters(PARAMETERSDATA)));

    //MeasurementDialog
    a.connect(&c,SIGNAL(showMeasurementDialog(MEASUREMENTDIALOGSETTINGS)),&w,SLOT(showMeasurementDialog(MEASUREMENTDIALOGSETTINGS)),Qt::QueuedConnection);
    a.connect(&c,SIGNAL(showFinishedBoxMeasurementDialog(QString,QString)),&w.measDialog,SLOT(showFinishedBox(QString,QString)),Qt::QueuedConnection);


    //MeasurementWorker
    MeasurementWorker *meas = new MeasurementWorker();
    QThread *measThread = new QThread();
    PowerSupplyHV *hv = new PowerSupplyHV();
    QThread *hvThread = new QThread();
    Analyzer *analyzer = new Analyzer();
    QThread *analyzerThread = new QThread();

    meas->moveToThread(measThread);
    hv->moveToThread(hvThread);
    analyzer->moveToThread(analyzerThread);
    hvThread->start();
    analyzerThread->start();
    a.connect(&c,SIGNAL(setPowerSupplyHV(MODULEDATA,int)),hv,SLOT(set(MODULEDATA,int)),Qt::DirectConnection);
    a.connect(&c,SIGNAL(setAnalyzer(MODULEDATA)),analyzer,SLOT(set(MODULEDATA)),Qt::DirectConnection);
    a.connect(&c,SIGNAL(setMeasurement(MEASUREMENTDATA,SERIESSOURCESDATA,PARAMETERSDATA,QString)),meas,SLOT(set(MEASUREMENTDATA,SERIESSOURCESDATA,PARAMETERSDATA,QString)),Qt::DirectConnection);
    a.connect(&c,SIGNAL(startMeasurement()),measThread,SLOT(start()));
    a.connect(measThread,SIGNAL(started()),meas,SLOT(init()));
    a.connect(meas,SIGNAL(initHV()),hv,SLOT(init()));
    a.connect(meas,SIGNAL(initAnalyzer()),analyzer,SLOT(init()));

    a.connect(meas,SIGNAL(finished(QString,int,QString)),&c,SLOT(finishedStatus(QString,int,QString)));
    a.connect(meas,SIGNAL(finished(QString,int,QString)),hv,SLOT(quit()),Qt::DirectConnection);
    a.connect(meas,SIGNAL(finished(QString,int,QString)),analyzer,SLOT(quit()),Qt::DirectConnection);
    a.connect(&c,SIGNAL(finishedMeasThread()),measThread,SLOT(quit()));

    a.connect(&w.measDialog,SIGNAL(stopMeasurement()),meas,SLOT(quit()),Qt::DirectConnection);
    a.connect(meas,SIGNAL(insertBlankVial()),&w.measDialog,SLOT(insertBlankVial()));
    a.connect(&w.measDialog,SIGNAL(startBlankVial()),meas,SLOT(startBlankVial()),Qt::DirectConnection);
    a.connect(meas,SIGNAL(insertSourceVial(QString)),&w.measDialog,SLOT(insertSourceVial(QString)));
    a.connect(&w.measDialog,SIGNAL(startSourceVial()),meas,SLOT(startSourceVial()),Qt::DirectConnection);

    a.connect(hv,SIGNAL(stop()),meas,SLOT(finishedHV()));
    a.connect(hv,SIGNAL(started()),meas,SLOT(startedHV()));
    a.connect(analyzer,SIGNAL(stop()),meas,SLOT(finishedAnalyzer()));
    a.connect(analyzer,SIGNAL(started()),meas,SLOT(startedAnalyzer()));

    a.connect(meas,SIGNAL(setTurnOn(bool)),hv,SLOT(setTurnOn(bool)));
    a.connect(meas,SIGNAL(setVoltage(float,int)),hv,SLOT(setVoltage(float,int)),Qt::DirectConnection);
    a.connect(hv,SIGNAL(status(QVector<QString>,QVector<float>,QVector<float>)),meas,SLOT(statusPowerSupplyHV(QVector<QString>,QVector<float>,QVector<float>)));
    a.connect(hv,SIGNAL(isCharged(bool)),meas,SLOT(setChargedHV(bool)),Qt::DirectConnection);

    a.connect(meas,SIGNAL(setCommand(int)),analyzer,SLOT(setCommand(int)),Qt::DirectConnection);
    a.connect(meas,SIGNAL(setTime(int,int)),analyzer,SLOT(setTime(int,int)));
    a.connect(analyzer,SIGNAL(status(QString,QVector<int>,QVector<unsigned long>)),meas,SLOT(statusAnalyzer(QString,QVector<int>,QVector<unsigned long>)));

    a.connect(&c,SIGNAL(testPowerSupply()),hv,SLOT(test()));
    a.connect(hv,SIGNAL(tested(bool)),&w.settingsDialog,SLOT(connectionTested(bool)));
    a.connect(&c,SIGNAL(testAnalyzer()),analyzer,SLOT(test()));
    a.connect(analyzer,SIGNAL(tested(bool)),&w.settingsDialog,SLOT(connectionTested(bool)));

    //MeasurementDialog
    a.connect(meas,SIGNAL(setStatusProcessMeasurementDialog(STATUSPROCESSMEASUREMENTDIALOG)),&w.measDialog,SLOT(setStatusProcess(STATUSPROCESSMEASUREMENTDIALOG)));
    a.connect(meas,SIGNAL(setStatusParametersMeasurementDialog(STATUSPARAMETERSMEASUREMENTDIALOG)),&w.measDialog,SLOT(setStatusParameters(STATUSPARAMETERSMEASUREMENTDIALOG)));
    a.connect(meas,SIGNAL(setStatusTrackMeasurementDialog(STATUSTRACKMEASUREMENTDIALOG)),&w.measDialog,SLOT(setStatusTrack(STATUSTRACKMEASUREMENTDIALOG)));
    a.connect(meas,SIGNAL(insertRowTableMeasurementDialog()),&w.measDialog,SLOT(insertRowTable()));
    a.connect(meas,SIGNAL(setRTTableMeasurementDialog(QStringList)),&w.measDialog,SLOT(setRTTable(QStringList)));
    a.connect(meas,SIGNAL(setPMTableMeasurementDialog(QStringList)),&w.measDialog,SLOT(setPMTable(QStringList)));

    a.connect(hv,SIGNAL(debug(QString)),meas,SIGNAL(debug(QString)));
    a.connect(hv,SIGNAL(error(QString)),meas,SIGNAL(error(QString)));
    a.connect(analyzer,SIGNAL(debug(QString)),meas,SIGNAL(debug(QString)));
    a.connect(analyzer,SIGNAL(error(QString)),meas,SIGNAL(error(QString)));


   // a.connect(measThread,SIGNAL(finished()),measThread,SLOT(deleteLater()));

    //Monitor
    QThread *monitorThread = new QThread();
    MonitorWorker *monitorWorker = new MonitorWorker();

    monitorWorker->moveToThread(monitorThread);

    a.connect(&c,SIGNAL(showMonitorDialog()),&w.monDialog,SLOT(exec()),Qt::QueuedConnection);
    a.connect(&c,SIGNAL(showMonitorDialog()),&w.monDialog,SLOT(clear()),Qt::QueuedConnection);
    a.connect(&c,SIGNAL(startMonitor()),monitorThread,SLOT(start()));
    a.connect(monitorThread,SIGNAL(started()),monitorWorker,SLOT(start()));
    a.connect(monitorWorker,SIGNAL(finished()),&c,SLOT(finishedMonitor()));
    a.connect(monitorWorker,SIGNAL(finished()),monitorThread,SLOT(quit()));
    a.connect(monitorWorker,SIGNAL(finished()),hv,SLOT(quit()));
    a.connect(monitorWorker,SIGNAL(finished()),analyzer,SLOT(quit()));
    a.connect(&w.monDialog,SIGNAL(stop()),monitorWorker,SLOT(quit()));
    a.connect(monitorThread,SIGNAL(finished()),&w.monDialog,SLOT(accept()));

    a.connect(monitorWorker,SIGNAL(initAnalyzer()),analyzer,SLOT(init()));
    a.connect(monitorWorker,SIGNAL(initHV()),hv,SLOT(init()));

    a.connect(hv,SIGNAL(stop()),monitorWorker,SLOT(finishedHV()));
    a.connect(hv,SIGNAL(started()),monitorWorker,SLOT(startedHV()));
    a.connect(analyzer,SIGNAL(stop()),monitorWorker,SLOT(finishedAnalyzer()));
    a.connect(analyzer,SIGNAL(started()),monitorWorker,SLOT(startedAnalyzer()));

    a.connect(hv,SIGNAL(status(QVector<QString>,QVector<float>,QVector<float>)),monitorWorker,SLOT(statusPowerSupplyHV(QVector<QString>,QVector<float>,QVector<float>)));
    a.connect(analyzer,SIGNAL(status(QString,QVector<int>,QVector<ulong>)),monitorWorker,SLOT(statusAnalyzer(QString,QVector<int>,QVector<ulong>)));

    a.connect(hv,SIGNAL(debug(QString)),monitorWorker,SLOT(debug(QString)));
    a.connect(hv,SIGNAL(error(QString)),monitorWorker,SLOT(error(QString)));
    a.connect(analyzer,SIGNAL(debug(QString)),monitorWorker,SLOT(debug(QString)));
    a.connect(analyzer,SIGNAL(error(QString)),monitorWorker,SLOT(error(QString)));

    a.connect(monitorWorker,SIGNAL(setStatusParametersMonitorDialog(STATUSPARAMETERSMEASUREMENTDIALOG)),&w.monDialog,SLOT(setStatusParameters(STATUSPARAMETERSMEASUREMENTDIALOG)),Qt::QueuedConnection);
    a.connect(monitorWorker,SIGNAL(setStatusTrackMonitorDialog(STATUSTRACKMEASUREMENTDIALOG)),&w.monDialog,SLOT(setStatusTrack(STATUSTRACKMEASUREMENTDIALOG)),Qt::QueuedConnection);
    a.connect(monitorWorker,SIGNAL(insertRowTableMonitorDialog()),&w.monDialog,SLOT(insertRowTable()));
    a.connect(monitorWorker,SIGNAL(setRTTableMonitorDialog(QStringList)),&w.monDialog,SLOT(setRTTable(QStringList)),Qt::QueuedConnection);
    a.connect(monitorWorker,SIGNAL(setPMTableMonitorDialog(QStringList)),&w.monDialog,SLOT(setPMTable(QStringList)),Qt::QueuedConnection);

    a.connect(monitorWorker,SIGNAL(setTurnOn(bool)),hv,SLOT(setTurnOn(bool)));
    a.connect(monitorWorker,SIGNAL(setHV(float,int)),hv,SLOT(setVoltage(float,int)));

    a.connect(monitorWorker,SIGNAL(setCommand(int)),analyzer,SLOT(setCommand(int)));

    a.connect(&w.monDialog,SIGNAL(setHV(float,int)),hv,SLOT(setVoltage(float,int)));
    a.connect(&w.monDialog,SIGNAL(setAnalyzer(int,int)),analyzer,SLOT(setTime(int,int)));

    a.connect(&w.monDialog,SIGNAL(startMeasMonitor(int)),monitorWorker,SLOT(startMeas(int)));
    a.connect(&w.monDialog,SIGNAL(stopMeasMonitor()),monitorWorker,SLOT(stopMeas()));
    a.connect(&w.monDialog,SIGNAL(resetMeasMonitor()),monitorWorker,SLOT(resetMeas()));
    a.connect(monitorWorker,SIGNAL(stoppedMeas()),&w.monDialog,SLOT(stopped()));

    a.connect(&w.monDialog,SIGNAL(saveToFileMonitorData(MONITORDIALOGDATA)),&c,SLOT(saveToFileMonitorData(MONITORDIALOGDATA)));

    c.setObject(meas,monitorWorker,hv,analyzer);
    c.init();
    w.show();
    int ret = a.exec();
    qDebug() << "main(): Quit app";
    hvThread->quit();
    analyzerThread->quit();
    qDebug() << "main(): Quit thread";
    return ret;
}
