#include "core.h"

Core::Core(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<QVector<QString> >("QVector<QString>");
    qRegisterMetaType<QVector<int> >("QVector<int>");
    qRegisterMetaType<QVector<unsigned long> >("QVector<unsigned long>");
    qRegisterMetaType<QVector<float> >("QVector<float>");
    qRegisterMetaType<QList<QStringList> >("QList<QStringList>");
    regSet = new QSettings("POLATOM","QTD",this);

    db = new Database(regSet->value("dbLocation").toString(),this);
    if(db->userPassword("admin").isEmpty()) {
        USERDATA a;
        a.login = "admin";
        a.name = "admin";
        a.password = generatorCryptographicHash("admin");
        a.lastModification = lastModificationDateTimeBy();
        if(!db->updateUser(a))
            qDebug() << "Core:Core(): Error creating admin user";
    }
}

void Core::init()
{
    //Set MainWindow
    settingsMainWindow.title = "QTD Control Software (rel. "+QFileInfo(QCoreApplication::applicationFilePath()).lastModified().date().toString("yyyy.M.d)");
    settingsMainWindow.position = regSet->value("posMainWindow").toPoint();
    settingsMainWindow.size = regSet->value("sizeMainWindow").toSize();
    settingsMainWindow.maximized = regSet->value("maximizedMainWindow").toBool();

    timeoutHalfSecondTimer();
    halfSecondTimer = new QTimer(this);
    connect(halfSecondTimer,SIGNAL(timeout()),this,SLOT(timeoutHalfSecondTimer()));
    halfSecondTimer->start(500);

    settingsMainWindow.headersLastStandardizationTable = lastStandardizationHeaders;
    settingsMainWindow.headersMeasurementRegisterTable = measurementHeaders;
    QList<QStringList> measurementRecords = db->registerMeasurement();
    QList<QStringList> lastStandardizationRecords;
    QStringList tmp;
    measurementStatistics.measurementCount = "0";
    for(int i=0;i<measurementRecords.count();i++){
        for(int j=0;j<measurementRecords.at(i).count();j++) {
            settingsMainWindow.completerMeasurementSearch << measurementRecords.at(i).at(j);
        }
        if(measurementRecords.at(i).at(M_STATUS).toInt() == STATUS_FINISHED && measurementRecords.at(i).at(M_MODE).toInt() == MODE_STANDARD) {
            SERIESSOURCESDATA series = db->seriesSources(measurementRecords.at(i).at(M_SERIESSOURCES));
            if(!tmp.contains(series.solution,Qt::CaseInsensitive) && !series.solution.isEmpty()) {
                tmp << series.solution;
                lastStandardizationRecords << (QStringList() << measurementRecords.at(i).at(M_NUCLIDE)
                                                            << series.solution
                                                            << measurementRecords.at(i).at(M_DATE));
            }

            measurementStatistics.measurementCount = QString::number(measurementStatistics.measurementCount.toInt()+1);
        }else if(measurementRecords.at(i).at(M_STATUS).toInt() == STATUS_FINISHED) {
            measurementStatistics.measurementCount = QString::number(measurementStatistics.measurementCount.toInt()+1);
        }
    } 
    settingsMainWindow.completerMeasurementSearch.removeDuplicates();
    emit setSettingsMainWindow(settingsMainWindow);
    emit setMeasurementRegisterTable(measurementRecords);
    emit setLastStandardizationTable(lastStandardizationRecords);

    //Set SettingsDialog
    settingsSettingsDialog.nameCompany = regSet->value("companyName").toString();
    settingsSettingsDialog.nameCounter = regSet->value("counterName").toString();
    settingsSettingsDialog.language = regSet->value("language").toString();
    settingsSettingsDialog.dbLocation = regSet->value("dbLocation").toString();
    if(settingsSettingsDialog.dbLocation.isEmpty())
        settingsSettingsDialog.dbLocation = QDir::currentPath();
    settingsSettingsDialog.reportLocation = regSet->value("reportLocation").toString();
    if(settingsSettingsDialog.reportLocation.isEmpty())
        settingsSettingsDialog.reportLocation = QDir::currentPath();

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
            settingsSettingsDialog.availablePort << info.portName();
        }

    settingsSettingsDialog.hv.name = regSet->value("namePowerSupplyHV").toString();
    settingsSettingsDialog.hv.port = regSet->value("port"+settingsSettingsDialog.hv.name).toString();
    settingsSettingsDialog.hv.baudRate = regSet->value("baudRate"+settingsSettingsDialog.hv.name).toString();
    settingsSettingsDialog.hv.lBusAddress = regSet->value("lBusAddress"+settingsSettingsDialog.hv.name).toInt();
    settingsSettingsDialog.hv.serialNameUSB = regSet->value("serialNameUSB"+settingsSettingsDialog.hv.name).toString();
    settingsSettingsDialog.hv.numberUSB = regSet->value("numberUSB"+settingsSettingsDialog.hv.name).toInt();

    settingsSettingsDialog.analyzer.name = regSet->value("nameAnalyzer").toString();
    settingsSettingsDialog.analyzer.port = regSet->value("port"+settingsSettingsDialog.analyzer.name).toString();
    settingsSettingsDialog.analyzer.baudRate = regSet->value("baudRate"+settingsSettingsDialog.analyzer.name).toString();
    settingsSettingsDialog.analyzer.lBusAddress = regSet->value("lBusAddress"+settingsSettingsDialog.analyzer.name).toInt();
    settingsSettingsDialog.analyzer.serialNameUSB = regSet->value("serialNameUSB"+settingsSettingsDialog.analyzer.name).toString();
    settingsSettingsDialog.analyzer.numberUSB = regSet->value("numberUSB"+settingsSettingsDialog.analyzer.name).toInt();

    settingsSettingsDialog.defaultBackgroundTime = regSet->value("defaultBlankTime").toInt();
    settingsSettingsDialog.defaultSourceTime = regSet->value("defaultSourceTime").toInt();
    settingsSettingsDialog.defaultReps = regSet->value("defaultReps").toInt();
    settingsSettingsDialog.delayedStart = regSet->value("delayedStart").toInt();

    settingsSettingsDialog.logins = db->loginList();

    emit setSettingsSettingsDialog(settingsSettingsDialog);
    emit setSettingsUser(loginUser);

    //Set NewMeasurementDialog

    settingsNewMeasDialog.company = settingsSettingsDialog.nameCompany;
    settingsNewMeasDialog.counter = settingsSettingsDialog.nameCounter;
    settingsNewMeasDialog.modeList = modeMeasurement;
    settingsNewMeasDialog.nameSeriesSourcesList = db->seriesSourcesList();
    settingsNewMeasDialog.nameParametersList = db->parametersList();
    settingsNewMeasDialog.completerNuclideList = db->nuclideList();
    settingsNewMeasDialog.completerUserList = db->userList();
    settingsNewMeasDialog.defaultBackgroundTime = settingsSettingsDialog.defaultBackgroundTime;
    settingsNewMeasDialog.defaultSourceTime = settingsSettingsDialog.defaultSourceTime;
    settingsNewMeasDialog.defaultReps = settingsSettingsDialog.defaultReps;
    settingsNewMeasDialog.user = loginUser.name+" "+loginUser.surname;
    emit setSettingsNewMeasurementDialog(settingsNewMeasDialog);

    //Set SeriesSourceDialog
    settingsSeriesSourcesDialog.nameSeriesSourcesList = settingsNewMeasDialog.nameSeriesSourcesList;
    settingsSeriesSourcesDialog.completerNuclideList = settingsNewMeasDialog.completerNuclideList;
    settingsSeriesSourcesDialog.completerUserList = settingsNewMeasDialog.completerUserList;
    settingsSeriesSourcesDialog.completerSolutionList = db->solutionList();
    settingsSeriesSourcesDialog.completerNameScintillatorList = db->scintillatorList();
    settingsSeriesSourcesDialog.completerVolumeScintillatorList = db->scintillatorVolumeList();
    settingsSeriesSourcesDialog.completerVialTypeList = db->vialTypeList();

    emit setSettingsSeriesSourcesDialog(settingsSeriesSourcesDialog);

    //Set ParametersDialog
    settingsParametersDialog.nameParametersList = settingsNewMeasDialog.nameParametersList;
    settingsParametersDialog.completerNuclideList = settingsNewMeasDialog.completerNuclideList;
    settingsParametersDialog.channelLimitedList = channelLimetedList;

    emit setSettingsParametersDialog(settingsParametersDialog);

    //Statisctics
    measurementStatistics.solutionCount = QString::number(settingsSeriesSourcesDialog.completerSolutionList.count());
    measurementStatistics.nuclideCount = QString::number(settingsNewMeasDialog.completerNuclideList.count());
    measurementStatistics.totalTimeCount = generatorStrTotalTime(regSet->value("totalTimeCount").toLongLong());
    emit setDisplayMeasurementStatistics(measurementStatistics);
}

QString Core::language()
{
    return regSet->value("language").toString();
}

void Core::setObject(MeasurementWorker *meas, MonitorWorker *mon, PowerSupplyHV *hv, Analyzer *ana)
{
    measObj = meas;
    monObj = mon;
    hvObj = hv;
    anaObj = ana;
}

void Core::searchMeasurementMainWindow(QString arg)
{
    QList<QStringList> records = db->registerMeasurement(arg);
    emit setMeasurementRegisterTable(records);
}

void Core::savePositionMainWindow(QPoint p)
{
    regSet->setValue("posMainWindow",p);
}

void Core::saveSizemainWindow(QSize s)
{
    regSet->setValue("sizeMainWindow",s);
}

void Core::saveMaxmizedMainWindow(bool m)
{
    regSet->setValue("maximizedMainWindow",m);
}

void Core::repeatMeasurementMainWindow(QString id)
{
    MEASUREMENTDATA data = db->measurement(id);
    data.madeBy = loginUser.name+" "+loginUser.surname;
    data.company = settingsSettingsDialog.nameCompany;
    data.counter = settingsSettingsDialog.nameCounter;
    emit showNewMeasurementDialog(data);
}

void Core::deleteReportMainWindow(QStringList ids)
{
    foreach(QString id,ids) {
        MEASUREMENTDATA data = db->measurement(id);
        data.status = STATUS_DELETED;
        data.lastModification = lastModificationDateTimeBy();
        db->updateMeasurement(data);
    }
    QList<QStringList> records = db->registerMeasurement();
    emit setMeasurementRegisterTable(records);
}

void Core::openReportLocationMainWindow(QString id)
{
    MEASUREMENTDATA data = db->measurement(id);
    bool ret = QDesktopServices::openUrl(QUrl("file:///"+QDir::toNativeSeparators(settingsSettingsDialog.reportLocation+"/Report/"+data.nuclide+"/"+data.filename)));
    if(!ret)
    {
        QMessageBox::warning(0,tr("Error"),tr("Report not found"),QMessageBox::Ok);
        return;
    }
}

void Core::clickedMonitorButtonMainWindow()
{
    signalsBlock = new QSignalBlocker(measObj);
    signalStatusDisabler(false);

    emit showMonitorDialog();

    emit setPowerSupplyHV(settingsSettingsDialog.hv,settingsSettingsDialog.delayedStart); //parameters of connection
    emit setAnalyzer(settingsSettingsDialog.analyzer); //parameters of connection

    emit startMonitor();
}

void Core::setLoginUser(QString login, QString password)
{
    loginUser = db->user(login);
    if(loginUser.password == generatorCryptographicHash(password)) {
        qDebug() << "Core(): Accepted login and password. Username: "+login;
        emit acceptedLoginUser();
    }else {
        qDebug() << "Core(): Rejected login and password. Username: "+login;
        emit rejectedLoginuser();
    }
}

void Core::saveSettingsDialog(SETTINGSDIALOGSETTINGS settings)
{
    settingsSettingsDialog.nameCompany = settings.nameCompany;
    settingsSettingsDialog.nameCounter = settings.nameCounter;
    settingsSettingsDialog.language = settings.language;
    settingsSettingsDialog.dbLocation = settings.dbLocation;
    settingsSettingsDialog.reportLocation = settings.reportLocation;

    settingsSettingsDialog.hv.name = settings.hv.name;
    if(!settings.hv.port.isEmpty()) {
        settingsSettingsDialog.hv.port = settings.hv.port;
        settingsSettingsDialog.hv.baudRate = settings.hv.baudRate;
        settingsSettingsDialog.hv.lBusAddress = settings.hv.lBusAddress;
        settingsSettingsDialog.hv.serialNameUSB = settings.hv.serialNameUSB;
        settingsSettingsDialog.hv.numberUSB = settings.hv.numberUSB;
    }
    settingsSettingsDialog.analyzer.name = settings.analyzer.name;
    if(!settings.analyzer.serialNameUSB.isEmpty()) {
        settingsSettingsDialog.analyzer.port = settings.analyzer.port;
        settingsSettingsDialog.analyzer.baudRate = settings.analyzer.baudRate;
        settingsSettingsDialog.analyzer.lBusAddress = settings.analyzer.lBusAddress;
        settingsSettingsDialog.analyzer.serialNameUSB = settings.analyzer.serialNameUSB;
        settingsSettingsDialog.analyzer.numberUSB = settings.analyzer.numberUSB;
    }

    settingsSettingsDialog.defaultBackgroundTime = settings.defaultBackgroundTime;
    settingsSettingsDialog.defaultSourceTime = settings.defaultSourceTime;
    settingsSettingsDialog.defaultReps = settings.defaultReps;
    settingsSettingsDialog.delayedStart = settings.delayedStart;


    regSet->setValue("companyName",settingsSettingsDialog.nameCompany);
    regSet->setValue("counterName",settingsSettingsDialog.nameCounter);
    regSet->setValue("language",settingsSettingsDialog.language);
    regSet->setValue("dbLocation",settingsSettingsDialog.dbLocation);
    regSet->setValue("reportLocation",settingsSettingsDialog.reportLocation);
    regSet->setValue("namePowerSupplyHV",settingsSettingsDialog.hv.name);
    regSet->setValue("port"+settingsSettingsDialog.hv.name,settingsSettingsDialog.hv.port);
    regSet->setValue("baudRate"+settingsSettingsDialog.hv.name,settingsSettingsDialog.hv.baudRate);
    regSet->setValue("lBusAddress"+settingsSettingsDialog.hv.name,settingsSettingsDialog.hv.lBusAddress);
    regSet->setValue("serialNameUSB"+settingsSettingsDialog.hv.name,settingsSettingsDialog.hv.serialNameUSB);
    regSet->setValue("numberUSB"+settingsSettingsDialog.hv.name,settingsSettingsDialog.hv.numberUSB);

    regSet->setValue("nameAnalyzer",settingsSettingsDialog.analyzer.name);
    regSet->setValue("port"+settingsSettingsDialog.analyzer.name,settingsSettingsDialog.analyzer.port);
    regSet->setValue("baudRate"+settingsSettingsDialog.analyzer.name,settingsSettingsDialog.analyzer.baudRate);
    regSet->setValue("lBusAddress"+settingsSettingsDialog.analyzer.name,settingsSettingsDialog.analyzer.lBusAddress);
    regSet->setValue("serialNameUSB"+settingsSettingsDialog.analyzer.name,settingsSettingsDialog.analyzer.serialNameUSB);
    regSet->setValue("numberUSB"+settingsSettingsDialog.analyzer.name,settingsSettingsDialog.analyzer.numberUSB);

    regSet->setValue("defaultBlankTime",settingsSettingsDialog.defaultBackgroundTime);
    regSet->setValue("defaultSourceTime",settingsSettingsDialog.defaultSourceTime);
    regSet->setValue("defaultReps",settingsSettingsDialog.defaultReps);
    regSet->setValue("delayedStart",settingsSettingsDialog.delayedStart);
}

void Core::testConnection(MODULEDATA data)
{
    if(data.name == "N1470") {
        emit setPowerSupplyHV(data,settingsSettingsDialog.delayedStart);
        emit testPowerSupply();
    }else if(data.name == "BAD1" || data.name == "BAD2") {
        emit setAnalyzer(data);
        emit testAnalyzer();
    }
}

void Core::checkAvailableNewLogin(QString login)
{
    USERDATA data = db->user(login);

    if(data.login == login) {
        emit availableNewLogin(false);
    }else {
        emit availableNewLogin(true);
    }

}

void Core::saveUserData(USERDATA data)
{
    data.lastModification = lastModificationDateTimeBy();
    data.password = generatorCryptographicHash(data.password);
    if(db->updateUser(data)) {
        loginUser.login = data.login;
        loginUser.password = data.password;
        loginUser.name = data.name;
        loginUser.surname = data.surname;
        loginUser.lastModification = data.lastModification;
        emit setSettingsUser(loginUser);
    }

}

void Core::setUsername(QString login)
{
    loginUser = db->user(login);
    emit setSettingsUser(loginUser);
}

void Core::removeUserData(QString login)
{
    db->removeUser(login);
}

void Core::confirmPassword(QString login, QString password)
{
    if(db->userPassword(login) == generatorCryptographicHash(password)) {
        emit confirmedPassword(true);
    }else {
        emit confirmedPassword(false);
    }
}

void Core::changePasswordUser(QString login, QString password)
{
    if(loginUser.login == login) {
        loginUser.password = generatorCryptographicHash(password);
        loginUser.lastModification = lastModificationDateTimeBy();
        if(!db->updateUser(loginUser))
            qDebug() << "Error with change password user - Login:"+login;
        emit setSettingsUser(loginUser);
    }else {
        USERDATA data = db->user(login);
        data.password = generatorCryptographicHash(password);
        data.lastModification = lastModificationDateTimeBy();
        if(!db->updateUser(data))
            qDebug() << "Error with change password user - Login:"+login;
    }
}

void Core::initMeasurement(MEASUREMENTDATA data)
{
    //adding to the db a new measurement record
    timeMeasurement = new QTime(0,0,0);
    timeMeasurement->start();

    data.id = QString::number(db->registerMeasurementCount().toInt()+1);
    data.filename = QString(data.nuclide).replace("-","")+"-"+QDateTime::currentDateTime().date().toString("yyMMdd")+"-"+data.id;
    data.date = QDateTime::currentDateTime().date().toString(formatDate);
    data.status = STATUS_ADDED;
    data.lastModification = lastModificationDateTimeBy();
    if(!db->insertMeasurement(data)) {
        //error with adding to db record of new measurement
        return;
    }
    //set settings MeasurementDialog
    SERIESSOURCESDATA seriesData = db->seriesSources(data.nameSeriesSources);
    PARAMETERSDATA paramData = db->parameters(data.nameParameters);

    settingsMeasurementDialog.nuclide = data.nuclide;
    settingsMeasurementDialog.filename = data.filename;
    settingsMeasurementDialog.solution = seriesData.solution;

    settingsMeasurementDialog.mode = data.mode;
    settingsMeasurementDialog.nameSeriesSources = seriesData.name;
    settingsMeasurementDialog.nameParameters = paramData.name;
    settingsMeasurementDialog.blankTime = QString::number(data.backgroundTime);
    settingsMeasurementDialog.sourceTime = QString::number(data.sourceTime);
    settingsMeasurementDialog.reps = QString::number(data.reps);
    settingsMeasurementDialog.headersRecordsTableRT = recordMeasurementHeadersRT;
    settingsMeasurementDialog.headersRecordsTablePM = recordMeasurementHeadersPM;

    signalsBlock = new QSignalBlocker(monObj);
    signalStatusDisabler(true);


    emit showMeasurementDialog(settingsMeasurementDialog);

    emit setPowerSupplyHV(settingsSettingsDialog.hv,settingsSettingsDialog.delayedStart); //parameters of connection
    emit setAnalyzer(settingsSettingsDialog.analyzer); //parameters of connection
    emit setMeasurement(data, seriesData, paramData,settingsSettingsDialog.reportLocation);  //parameters of measurement

    emit startMeasurement();

}

void Core::changedNuclideNewMeasurementDialog(QString nuclide)
{
   settingsNewMeasDialog.nameSeriesSourcesList = db->seriesSourcesList(nuclide);
   settingsNewMeasDialog.nameParametersList = db->parametersList(nuclide);
   emit setSettingsNewMeasurementDialog(settingsNewMeasDialog);
}

void Core::getDataSeriesSources(QString name)
{
    SERIESSOURCESDATA data = db->seriesSources(name);
    emit setDataSeriesSourceDialog(data);
}

void Core::removeSeriesSources(QString name)
{
    db->removeSeriesSources(name);
}

void Core::saveSeriesSources(SERIESSOURCESDATA data)
{
    data.lastModification = lastModificationDateTimeBy();
    db->insertSeriesSources(data);
    if(!settingsSeriesSourcesDialog.nameSeriesSourcesList.contains(data.name))
        settingsSeriesSourcesDialog.nameSeriesSourcesList.prepend(data.name);
    if(!settingsSeriesSourcesDialog.completerUserList.contains(data.preparedBy))
        settingsSeriesSourcesDialog.completerUserList << data.preparedBy;
    if(!settingsSeriesSourcesDialog.completerVialTypeList.contains(data.vialType))
        settingsSeriesSourcesDialog.completerVialTypeList << data.vialType;
    if(!settingsSeriesSourcesDialog.completerVolumeScintillatorList.contains(data.volumeScintillator))
        settingsSeriesSourcesDialog.completerVolumeScintillatorList << data.volumeScintillator;
    if(!settingsSeriesSourcesDialog.completerNameScintillatorList.contains(data.nameScintillator))
        settingsSeriesSourcesDialog.completerNameScintillatorList << data.nameScintillator;
    if(!settingsSeriesSourcesDialog.completerNuclideList.contains(data.nuclide)) {
        settingsSeriesSourcesDialog.completerNuclideList << data.nuclide;
        settingsParametersDialog.completerNuclideList << data.nuclide;
    }
    if(!settingsSeriesSourcesDialog.completerSolutionList.contains(data.solution))
        settingsSeriesSourcesDialog.completerSolutionList << data.solution;

    emit setSettingsSeriesSourcesDialog(settingsSeriesSourcesDialog);
    emit setSettingsParametersDialog(settingsParametersDialog);
}

void Core::getDataParameters(QString name)
{
    PARAMETERSDATA data = db->parameters(name);
    emit setDataParametersDialog(data);
}

void Core::removeParameters(QString name)
{
    db->removeParameters(name);
}

void Core::saveParameters(PARAMETERSDATA data)
{
    data.lastModification = lastModificationDateTimeBy();
    db->insertParameters(data);
    if(!settingsParametersDialog.completerNuclideList.contains(data.nuclide)) {
        settingsParametersDialog.completerNuclideList << data.nuclide;
        settingsSeriesSourcesDialog.completerNuclideList << data.nuclide;
        measurementStatistics.nuclideCount = QString::number(measurementStatistics.nuclideCount.toInt()+1);
    }
    if(!settingsParametersDialog.nameParametersList.contains(data.name))
        settingsParametersDialog.nameParametersList.prepend(data.name);
    emit setSettingsParametersDialog(settingsParametersDialog);
    emit setSettingsSeriesSourcesDialog(settingsSeriesSourcesDialog);
}

void Core::finishedStatus(QString idMeas, int status, QString lastError)
{
    MEASUREMENTDATA data = db->measurement(idMeas);
    data.status = status;
    data.lastModification = lastModificationDateTimeBy();
    db->updateMeasurement(data);
    QList<QStringList> measurementRecords = db->registerMeasurement();
    emit setMeasurementRegisterTable(measurementRecords);


    if(status != STATUS_FINISHED) {
        if(!lastError.isEmpty()) {
            emit showFinishedBoxMeasurementDialog(tr("Measurement canceled!\nError log:\n")+lastError,tr("\nTime of measurement: ")+QTime(0,0,0).addMSecs(timeMeasurement->elapsed()).toString(formatTime));
        }else {
            emit showFinishedBoxMeasurementDialog(tr("Measurement canceled!"),tr("\nTime of measurement: ")+QTime(0,0,0).addMSecs(timeMeasurement->elapsed()).toString(formatTime));
        }
    }else {
        emit showFinishedBoxMeasurementDialog(tr("Measurement finished!"),tr("\nTime of measurement: ")+QTime(0,0,0).addMSecs(timeMeasurement->elapsed()).toString(formatTime));
        regSet->setValue("totalTimeCount",regSet->value("totalTimeCount").toLongLong() + (qint64)(timeMeasurement->elapsed()));
        measurementStatistics.totalTimeCount = generatorStrTotalTime(regSet->value("totalTimeCount").toLongLong());
        measurementStatistics.measurementCount = QString::number(measurementStatistics.measurementCount.toInt()+1);
        measurementStatistics.solutionCount = QString::number(settingsSeriesSourcesDialog.completerSolutionList.count());
        measurementStatistics.nuclideCount = QString::number(settingsSeriesSourcesDialog.completerNuclideList.count());
    }
    emit setDisplayMeasurementStatistics(measurementStatistics);
    emit finishedMeasThread();
    delete timeMeasurement;
    delete signalsBlock;
}

void Core::saveToFileMonitorData(MONITORDIALOGDATA data)
{
    QString filename = data.nuclide;
    filename.replace("-","");
    filename.append(QDateTime::currentDateTime().toString("-yyMMddhhmm"));
    bool ok;
    filename = QInputDialog::getText(0, tr("Enter the file name"), tr("File name:"),QLineEdit::Normal, filename, &ok);

    if(ok && !filename.isEmpty()) {
        QString path = QFileDialog::getExistingDirectory(0,tr("Select"),regSet->value("lastDir").toString());
        regSet->setValue("lastDir",path);
        if(!path.isEmpty()) {
            QString filenameRT = path+"/"+filename+"_RT.mon";
            QString filenamePM = path+"/"+filename+"_PM.mon";
            QStringList lines;
            lines << "Report QTD Control Software (rel. "+QFileInfo(QCoreApplication::applicationFilePath()).lastModified().date().toString("yyyy.M.d)")
                  << "===============================================================================\n";
            Report::appendText(filenameRT,lines.join("\n"));
            Report::appendText(filenamePM,lines.join("\n"));
            List list;
            list.insert("Nuclide",data.nuclide);
            list.insert("Vial",data.vial);
            list.insert("Comment",data.comment);
            list.insert("Data",QDateTime::currentDateTime().toString(formatDate));
            list.insert("Made by",loginUser.name+" "+loginUser.surname);
            Report::appendMapsList(filenameRT,list);
            Report::appendMapsList(filenamePM,list);
            Report::appendText(filenameRT,"===============================================================================\n");
            Report::appendText(filenamePM,"===============================================================================\n");
            data.recordsRT.prepend(recordMonitorHeadersRT);
            data.recordsPM.prepend(recordMonitorHeadersPM);
            data.recordsRT.first().append(QStringList()
                                          << ""
                                          << "HV Anode(V)"
                                          << "HV D1(V)"
                                          << "HV F(V)"
                                          << "HV Gamma"
                                          << ""
                                          << "R1(ns)"
                                          << "DT1(ns)"
                                          << "R2(ns)"
                                          << "DT2(ns)"
                                          << "DLB(ns)");
            data.recordsPM.first().append(QStringList()
                                          << ""
                                          << "HV Anode(V)"
                                          << "HV D1(V)"
                                          << "HV F(V)"
                                          << "HV Gamma"
                                          << ""
                                          << "R1(ns)"
                                          << "DT1(ns)"
                                          << "R2(ns)"
                                          << "DT2(ns)"
                                          << "DLB(ns)");
            Report::appendTable(filenameRT,data.recordsRT);
            Report::appendTable(filenamePM,data.recordsPM);
        }
    }
}

void Core::finishedMonitor()
{
    delete signalsBlock;
}

void Core::timeoutHalfSecondTimer()
{
    currentDateTime.currentTime = QDateTime::currentDateTime().time().toString("hh:mm:ss");
    currentDateTime.currentDate = QDateTime::currentDateTime().date().toString("dddd, d MMMM yyyy");
    currentDateTime.timeZone = "UTC+"+QString::number(QDateTime::currentDateTime().offsetFromUtc()/3600,'f',0)+":00";
    emit setDisplayDateTime(currentDateTime);
}

inline QString Core::lastModificationDateTimeBy()
{
    return QDateTime::currentDateTime().date().toString(formatDate)+" "+QDateTime::currentDateTime().time().toString(formatTime)+" "+loginUser.login;
}

void Core::signalStatusDisabler(bool block)
{
    if(block) {
        QCoreApplication::disconnect(hvObj,SIGNAL(status(QVector<QString>,QVector<float>,QVector<float>)),monObj,SLOT(statusPowerSupplyHV(QVector<QString>,QVector<float>,QVector<float>)));
        QCoreApplication::disconnect(anaObj,SIGNAL(status(QString,QVector<int>,QVector<ulong>)),monObj,SLOT(statusAnalyzer(QString,QVector<int>,QVector<ulong>)));
        QCoreApplication::connect(hvObj,SIGNAL(status(QVector<QString>,QVector<float>,QVector<float>)),measObj,SLOT(statusPowerSupplyHV(QVector<QString>,QVector<float>,QVector<float>)));
        QCoreApplication::connect(anaObj,SIGNAL(status(QString,QVector<int>,QVector<ulong>)),measObj,SLOT(statusAnalyzer(QString,QVector<int>,QVector<ulong>)));
    }else {
        QCoreApplication::connect(hvObj,SIGNAL(status(QVector<QString>,QVector<float>,QVector<float>)),monObj,SLOT(statusPowerSupplyHV(QVector<QString>,QVector<float>,QVector<float>)));
        QCoreApplication::connect(anaObj,SIGNAL(status(QString,QVector<int>,QVector<ulong>)),monObj,SLOT(statusAnalyzer(QString,QVector<int>,QVector<ulong>)));
        QCoreApplication::disconnect(hvObj,SIGNAL(status(QVector<QString>,QVector<float>,QVector<float>)),measObj,SLOT(statusPowerSupplyHV(QVector<QString>,QVector<float>,QVector<float>)));
        QCoreApplication::disconnect(anaObj,SIGNAL(status(QString,QVector<int>,QVector<ulong>)),measObj,SLOT(statusAnalyzer(QString,QVector<int>,QVector<ulong>)));
    }
}


