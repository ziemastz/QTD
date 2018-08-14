#include "measurementworker.h"

MeasurementWorker::MeasurementWorker(QObject *parent) : QObject(parent)
{
    isTurnOnHV = true;
    isStoppedCounting = true;
    isChargedHV = false;
    report = nullptr;
}

MeasurementWorker::~MeasurementWorker()
{
    delete report;
}

void MeasurementWorker::set(MEASUREMENTDATA measData, SERIESSOURCESDATA seriesData, PARAMETERSDATA paramData, QString location)
{

    lastError.clear();

    stop = false;
    id = measData.id;

    //measurement time
    bgTime = measData.backgroundTime;
    soTime = measData.sourceTime;
    reps = measData.reps;

    //series sources
    idVials = seriesData.idVials;
    masses = seriesData.masses;
    currentSource = 0;

    //parameters of measurement
    hvAnodeBeta = toVariantList(paramData.hvAnodeBeta);
    hvFirstDynodeBeta = toVariantList(paramData.hvFirstDynodeBeta);
    hvFocusingDynodeBeta = toVariantList(paramData.hvFocalysingDynodeBeta);
    hvAnodeGamma = toVariantList(paramData.hvAnodeGamma);

    resolvintTimeBeta = toVariantList(paramData.resolvintTimeBeta);
    resolvingTimeGamma = toVariantList(paramData.resolvingTimeGamma);
    deadTimeBeta = toVariantList(paramData.deadTimeBeta);
    deadTimeGamma = toVariantList(paramData.deadTimeGamma);
    delayTimeBeta = toVariantList(paramData.delayTimeBeta);

    resetPoint();
    paramData.tagPoints = tagPoints;
    lowerWindowGamma = paramData.lowerWindowGamma;
    upperWindowGamma = paramData.upperWindowGamma;
    timeLimited = paramData.timeLimited;
    channelLimited = paramData.channelLimited;
    valueCountLimited = paramData.valueCountLimited;
    valueUncertaintlyLimited = paramData.valueUncertaintlyLimited;

    //report create
    if(report != nullptr) {
        delete report;
        report = nullptr;
    }
    report = new Report(measData,seriesData,paramData,location);
    connect(this,SIGNAL(debug(QString)),report,SLOT(debug(QString)));
    connect(this,SIGNAL(error(QString)),report,SLOT(debug(QString)));
    connect(this,SIGNAL(error(QString)),this,SLOT(addError(QString)));

    statusProcess.beginMeasurement = QDateTime::currentDateTime();
    statusProcess.sourceCount = masses.count();
    statusProcess.pointCount = maxPoint;
    statusProcess.reps = reps;
    if(bgTime>0) {
        statusProcess.time = bgTime;
        statusProcess.endMeasurement = QDateTime::currentDateTime().addSecs(bgTime*maxPoint*reps+soTime*maxPoint*reps*masses.count());
    }else {
        statusProcess.time = soTime;
        statusProcess.endMeasurement = QDateTime::currentDateTime().addSecs(soTime*maxPoint*reps*masses.count());
    }
    statusProcess.currentTime = 0;
    statusProcess.currentReps = currentReps;
    statusProcess.currentPoint = currentPoint;
    statusProcess.currentSource = currentSource;

    emit setStatusProcessMeasurementDialog(statusProcess);
    emit debug("MeasWorker():set() - Parameters set");

}

void MeasurementWorker::startedHV()
{
    isStartedHV = true;
}

void MeasurementWorker::startedAnalyzer()
{
    isStartedAnalyzer = true;
}

void MeasurementWorker::finishedHV()
{
    isStartedHV = false;
    quit();
}

void MeasurementWorker::finishedAnalyzer()
{
    isStartedAnalyzer = false;
    quit();
}

void MeasurementWorker::statusAnalyzer(QString stat, QVector<int> timeCh, QVector<unsigned long> counters)
{
    if(this->signalsBlocked() || stop)
        return;
    //emit debug("MeasWorker():statusAnalyzer() - Received status");
    statusParameters.resolvingTimeBeta = QString::number(timeCh[CH_RESOLVINGTIMEBETA])+" ns";
    statusParameters.deadTimeBeta = QString::number(static_cast<double>(timeCh[CH_DEADTIMEBETA])/1000)+" μs";
    statusParameters.resolvingTimeGamma = QString::number(timeCh[CH_RESOLVINGTIMEGAMMA])+" ns";
    statusParameters.deadTimeGamma = QString::number(static_cast<double>(timeCh[CH_DEADTIMEGAMMA])/1000)+" μs";
    statusParameters.delayTimeBeta = QString::number(static_cast<double>(timeCh[CH_DELAYTIMEBETA])/1000)+" μs";
    statusParameters.statusAnalyzer = stat;
    emit setStatusParametersMeasurementDialog(statusParameters);

    if(counters.count() != 57) {
        emit error("MeasWorker():statusAnalyzer() - Failed reading data");
        return;
    }
    countersStatus = counters;

    if(stat == tr("Started") || stat == tr("Stopped")) {
        statusTrack.cpsA = static_cast<double>(counters[PM_A])/(static_cast<double>(counters[PM_LT_A])/freqClk);
        statusTrack.cpsB = static_cast<double>(counters[PM_B])/(static_cast<double>(counters[PM_LT_B])/freqClk);
        statusTrack.cpsC = static_cast<double>(counters[PM_C])/(static_cast<double>(counters[PM_LT_C])/freqClk);
        statusTrack.cpsG = static_cast<double>(counters[PM_G])/(static_cast<double>(counters[PM_LT_G])/freqClk);

        statusTrack.countsA = static_cast<int>(counters[PM_A]);
        statusTrack.countsB = static_cast<int>(counters[PM_B]);
        statusTrack.countsC = static_cast<int>(counters[PM_C]);
        statusTrack.countsG = static_cast<int>(counters[PM_G]);

        statusTrack.deatTimeA = static_cast<int>(100 - (static_cast<double>(counters[PM_LT_A])*100/static_cast<double>(counters[MEAS_REAL_TIME])));
        statusTrack.deatTimeB = static_cast<int>(100 - (static_cast<double>(counters[PM_LT_B])*100/static_cast<double>(counters[MEAS_REAL_TIME])));
        statusTrack.deatTimeC = static_cast<int>(100 - (static_cast<double>(counters[PM_LT_C])*100/static_cast<double>(counters[MEAS_REAL_TIME])));
        statusTrack.deatTimeG = static_cast<int>(100 - (static_cast<double>(counters[PM_LT_G])*100/static_cast<double>(counters[MEAS_REAL_TIME])));

        statusTrack.efficiencyTDCR = static_cast<int>(static_cast<double>(counters[TDCR_T])*100/static_cast<double>(counters[TDCR_D]));
        statusTrack.efficiencyBeta4LS = static_cast<int>(static_cast<double>(counters[GAMMA_GD])*100/static_cast<double>(counters[GAMMA_G]));
        statusTrack.efficiencyGamma4LS = static_cast<int>((static_cast<double>(counters[GAMMA_GD]))*100/static_cast<double>(counters[GAMMA_D]));
    }else {
        statusTrack.cpsA = 0;
        statusTrack.cpsB = 0;
        statusTrack.cpsC = 0;
        statusTrack.cpsG = 0;

        statusTrack.countsA = 0;
        statusTrack.countsB = 0;
        statusTrack.countsC = 0;
        statusTrack.countsG = 0;

        statusTrack.deatTimeA = 0;
        statusTrack.deatTimeB = 0;
        statusTrack.deatTimeC = 0;
        statusTrack.deatTimeG = 0;

        statusTrack.efficiencyTDCR = 0;
        statusTrack.efficiencyBeta4LS = 0;
        statusTrack.efficiencyGamma4LS = 0;
    }

    emit setStatusTrackMeasurementDialog(statusTrack);

    statusProcess.currentPoint = currentPoint;
    statusProcess.currentReps = currentReps;
    statusProcess.currentSource = currentSource;

    if(stat == tr("Stopped")) {
        if(!isStoppedCounting) {
            emit debug("MeasWorker():statusAnalyzer() - Stopped counting");
            emit setRTTableMeasurementDialog(generatorRecordRT());
            emit setPMTableMeasurementDialog(generatorRecordPM());
            isStoppedCounting = true;
            statusProcess.time = statusProcess.currentTime;
            leftTime = 0;
        }
    }else if(stat == tr("Started")) {
        statusProcess.currentTime = static_cast<int>(static_cast<float>(counters[MEAS_REAL_TIME])/freqClk);
        leftTime = 0;

        switch(timeLimited) {
        case REAL_TIME:
        {

            if((currentSource == 0 || !isChannelLimited()) && (static_cast<int>((float)counters[MEAS_REAL_TIME]/freqClk) >= (currentSource == 0 ? bgTime : soTime)) ){
                emit setCommand(C_STOP);
            }
            if(statusProcess.time <= (currentSource == 0 ? bgTime : soTime)) {
                statusProcess.time = (currentSource == 0 ? bgTime : soTime);
                statusProcess.currentTime = static_cast<int>(static_cast<float>(counters[MEAS_REAL_TIME])/freqClk);
            }
            leftTime = (statusProcess.time - statusProcess.currentTime) + (statusProcess.reps-statusProcess.currentReps)*statusProcess.time + (statusProcess.pointCount - statusProcess.currentPoint)*statusProcess.time*statusProcess.reps;
            if(statusProcess.currentPoint == 0 || statusProcess.currentReps == 0)
                leftTime = (statusProcess.pointCount - statusProcess.currentPoint)*statusProcess.time*statusProcess.reps;
            break;
        }
        case LIVETIME_BETA:
        {
            if((currentSource == 0 || !isChannelLimited()) && (static_cast<int>((float)counters[TDCR_LT]/freqClk) >= (currentSource == 0 ? bgTime : soTime)) ){
                emit setCommand(C_STOP);
            }
            if(statusProcess.time <= (currentSource == 0 ? bgTime : soTime)) {
                statusProcess.time = (currentSource == 0 ? bgTime : soTime);
                statusProcess.currentTime = static_cast<int>(static_cast<float>(counters[MEAS_REAL_TIME])/freqClk);

                double corDT = (double)counters[MEAS_REAL_TIME]/(double)counters[TDCR_LT];
                int liveTime = static_cast<int>((double)statusProcess.time * corDT);
                statusProcess.time = liveTime;
            }else{
                double corDT = (double)counters[MEAS_REAL_TIME]/(double)counters[TDCR_LT];
                int liveTime = static_cast<int>(static_cast<double>(currentSource == 0 ? bgTime : soTime) * corDT);
                if(statusProcess.time <= liveTime) {
                    statusProcess.time = liveTime;
                }
            }
            leftTime = (statusProcess.time - statusProcess.currentTime) + (statusProcess.reps-statusProcess.currentReps)*statusProcess.time + (statusProcess.pointCount - statusProcess.currentPoint)*statusProcess.time*statusProcess.reps;
            if(statusProcess.currentPoint == 0 || statusProcess.currentReps == 0)
                leftTime = (statusProcess.pointCount - statusProcess.currentPoint)*statusProcess.time*statusProcess.reps;
            break;
        }
        case LIVETIME_GAMMA:
        {
            if((currentSource == 0 || !isChannelLimited()) && (static_cast<int>((float)counters[GAMMA_LT]/freqClk) >= (currentSource == 0 ? bgTime : soTime)) ){
                if(!isChannelLimited() || currentSource == 0)
                    emit setCommand(C_STOP);
            }
            if(statusProcess.time <= (currentSource == 0 ? bgTime : soTime)) {
                statusProcess.time = (currentSource == 0 ? bgTime : soTime);
                statusProcess.currentTime = static_cast<int>(static_cast<float>(counters[MEAS_REAL_TIME])/freqClk);

                double corDT = (double)counters[MEAS_REAL_TIME]/(double)counters[GAMMA_LT];
                int liveTime = static_cast<int>((double)statusProcess.time * corDT);
                statusProcess.time = liveTime;
            }else{
                double corDT = (double)counters[MEAS_REAL_TIME]/(double)counters[GAMMA_LT];
                int liveTime = static_cast<int>(static_cast<double>(currentSource == 0 ? bgTime : soTime) * corDT);
                if(statusProcess.time <= liveTime) {
                    statusProcess.time = liveTime;
                }
            }
            leftTime = (statusProcess.time - statusProcess.currentTime) + (statusProcess.reps-statusProcess.currentReps)*statusProcess.time + (statusProcess.pointCount - statusProcess.currentPoint)*statusProcess.time*statusProcess.reps;
            if(statusProcess.currentPoint == 0 || statusProcess.currentReps == 0)
                leftTime = (statusProcess.pointCount - statusProcess.currentPoint)*statusProcess.time*statusProcess.reps;
            break;
        }
        default:
        {
            if(static_cast<int>((float)counters[MEAS_REAL_TIME]/freqClk) >= (currentSource == 0 ? bgTime : soTime)){
                if(!isChannelLimited() || currentSource == 0)
                    emit setCommand(C_STOP);
            }
            leftTime = (statusProcess.time - statusProcess.currentTime) + (statusProcess.reps-statusProcess.currentReps)*statusProcess.time + (statusProcess.pointCount - statusProcess.currentPoint)*statusProcess.time*statusProcess.reps;
            if(statusProcess.currentPoint ==0 || statusProcess.currentReps == 0)
                leftTime = (statusProcess.pointCount - statusProcess.currentPoint)*statusProcess.time*statusProcess.reps;
            break;
        }
        }

        if(leftTime < 0) {
            statusProcess.timeLeft = tr(" Unknown");
        }else{
            int min = leftTime/60;
            int sec = leftTime - (min*60);
            int h;
            if(leftTime <=60) {
                statusProcess.timeLeft = (QString::number(leftTime)+" s");
            }else if(min>=60) {
                h = min/60;
                min = min - (h*60);
                statusProcess.timeLeft = (QString::number(h)+" h "+QString::number(min)+" m "+QString::number(sec)+" s");
            }else {
                statusProcess.timeLeft = (QString::number(min)+" m "+QString::number(sec)+" s");
            }

        }

        if(currentPoint!=0){
            if(currentSource == 0) {
                statusProcess.endMeasurement = QDateTime::currentDateTime().addSecs((maxPoint-currentPoint)*reps*bgTime+(reps-currentReps)*bgTime+(bgTime-statusProcess.currentTime)+masses.count()*soTime*maxPoint*reps);
            }else {
                statusProcess.endMeasurement = QDateTime::currentDateTime().addSecs((masses.count()-currentSource)*statusProcess.time*maxPoint*reps+(maxPoint-currentPoint)*reps*statusProcess.time+(reps-currentReps)*statusProcess.time+(statusProcess.time-statusProcess.currentTime));
            }
        }

        report->addCurrentStatusCounters(counters);

        emit setRTTableMeasurementDialog(generatorRecordRT());
        emit setPMTableMeasurementDialog(generatorRecordPM());

        isStoppedCounting = false;
    }else {
       isStoppedCounting = false;
    }

    emit setStatusProcessMeasurementDialog(statusProcess);
}

void MeasurementWorker::statusPowerSupplyHV(QVector<QString> statCh, QVector<float> voltCh, QVector<float> ampCh)
{
    if(this->signalsBlocked() || stop)
        return;
    //emit debug("MeasWorker():statusPowerSupplyHV() - Received status");
    statusParameters.statusAnode = statCh[CH_HVANODEBETA];
    statusParameters.statusFirstDynode = statCh[CH_HVFIRSTDYNODEBETA];
    statusParameters.statusFocusingDynode = statCh[CH_HVFOCUSINGDYNODEBETA];
    statusParameters.statusGamma = statCh[CH_HVANODEGAMMA];
    statusParameters.voltAnode = voltCh[CH_HVANODEBETA];
    statusParameters.voltFirstDynode = voltCh[CH_HVFIRSTDYNODEBETA];
    statusParameters.voltFocusingDynode = voltCh[CH_HVFOCUSINGDYNODEBETA];
    statusParameters.voltGamma = voltCh[CH_HVANODEGAMMA];
    statusParameters.ampAnode = ampCh[CH_HVANODEBETA];
    statusParameters.ampFirstDynode = ampCh[CH_HVFIRSTDYNODEBETA];
    statusParameters.ampFocusingDynode = ampCh[CH_HVFOCUSINGDYNODEBETA];
    statusParameters.ampGamma = ampCh[CH_HVANODEGAMMA];
    emit setStatusParametersMeasurementDialog(statusParameters);

    if(!statCh[0].isEmpty() && !statCh[1].isEmpty() && !statCh[2].isEmpty() && !statCh[3].isEmpty()) {
        if((statCh[0].split("|").first() == "ON") && (statCh[1].split("|").first() == "ON") && (statCh[2].split("|").first() == "ON") && (statCh[3].split("|").first() == "ON")){
            if(!isTurnOnHV) {
                emit debug("MeasWorker():statusPowerSupplyHV() - Turn on");
                isTurnOnHV = true;
            }
            report->addCurrentStatusHV(voltCh,ampCh);
        }else if((statCh[0].split("|").first() == "OFF") && (statCh[1].split("|").first() == "OFF") && (statCh[2].split("|").first() == "OFF") && (statCh[3].split("|").first() == "OFF")){
            if(isTurnOnHV) {
                emit debug("MeasWorker():statusPowerSupplyHV() - Turn off");
                isTurnOnHV = false;
            }
        }
    }
}

void MeasurementWorker::setChargedHV(bool charged)
{
    isChargedHV = charged;
}

void MeasurementWorker::measurementBackground()
{
    statusProcess.time = bgTime;
    emit setTurnOn(false);
    emit debug("MeasWorker:measurementBackground() - Turn off HV");
    while(1) {
        QApplication::processEvents();
        if(!isTurnOnHV)
            break;
        if(stop) {
           return;
        }
    }

    emit insertBlankVial();
    emit debug("MeasWorker:measurementBackground() - Insert blank vial");
}

void MeasurementWorker::startBlankVial()
{
    emit setTurnOn(true);
    emit debug("MeasWorker:startBlankVial() - Turn on HV");
    while(1) {
        QApplication::processEvents();
        if(isTurnOnHV)
            break;
        if(stop) {
            return;
        }
    }

    emit debug("MeasWorker:startBlankVial() - Reset point");
    resetPoint();
    emit debug("MeasWorker:startBlankVial() - Started measurement of blank vial");
    while(!endPoint) {
        nextPoint();
        emit debug("MeasWorker:startBlankVial() - Set point: "+QString::number(currentPoint));
        while(!endReps) {
            nextReps();
            emit debug("MeasWorker:startBlankVial() - Set reps: "+QString::number(currentReps));

            waitForRecord();
            if(stop)
                return;
        }
    }
    emit debug("MeasWorker:startBlankVial() - Finished measurement of blank vial");
    if(soTime > 0) {
        measurementSource();
    }else {
        emit finished(id,STATUS_FINISHED,lastError);
        stop = true;
        emit debug("MeasWorker:startBlankVial() - Finished measurement");
    }

}

void MeasurementWorker::measurementSource()
{
    statusProcess.time = soTime;
    emit setTurnOn(false);
    emit debug("MeasWorker:measurementSource() - Turn off HV");
    while(1) {
        QApplication::processEvents();
        if(!isTurnOnHV)
            break;
        if(stop) {
            return;
        }
    }

    if(currentSource >= masses.count()) {
        stop = true;
        emit finished(id,STATUS_FINISHED,lastError);
        emit debug("MeasWorker:measurementSource() - Finished measurement");
    }else {
        emit insertSourceVial(idVials.at(currentSource)+" ("+QString::number(masses.at(currentSource))+" mg)");
        emit debug("MeasWorker:measurementSource() - Insert source: "+QString::number(currentSource+1));
    }
}

void MeasurementWorker::startSourceVial()
{
    emit setTurnOn(true);
    emit debug("MeasWorker:startSourceVial() - Turn on HV");
    while(1) {
         QApplication::processEvents();
        if(isTurnOnHV)
            break;
        if(stop) {
            return;
        }
    }
    emit debug("MeasWorker:startSourceVial() - Reset point");
    resetPoint();
    currentSource++;
    emit debug("MeasWorker:startSourceVial() - Started measurement of source vial: "+QString::number(currentSource));
    while(!endPoint) {
        nextPoint();
        emit debug("MeasWorker:startSourceVial() - Set point: "+QString::number(currentPoint));
        while(!endReps) {
            nextReps();
            emit debug("MeasWorker:startSourceVial() - Set reps: "+QString::number(currentReps));
            waitForRecord();
            if(stop)
                return;

        }
    }
    emit debug("MeasWorker:startSourceVial() - Finished measurement of source vial: "+QString::number(currentSource));
    measurementSource();
}

void MeasurementWorker::init()
{
    lastError.clear();
    isStartedAnalyzer = false;
    isStartedHV = false;
    isTurnOnHV = true;
    isStoppedCounting = true;
    isChargedHV = false;

    emit initHV();
    emit initAnalyzer();
    emit debug("MeasWorker:init() - Initiation of PowerSupplyHV and Analyzer");
    while(1) {
        QApplication::processEvents();
        if(isStartedAnalyzer && isStartedHV)
            break;
        if(stop){
            return;
        }
    }
    emit debug("MeasWorker:init() - Begin of measurement");
    if(bgTime > 0) {
        measurementBackground();
    }else if(soTime > 0) {

        measurementSource();
    }else {
        emit finished(id,STATUS_FINISHED,lastError);
        emit debug("MeasWorker:init() - Finished measurement");

    }
}

void MeasurementWorker::nextPoint()
{
    endReps = false;
    if(currentPoint == 0) {
        currentPoint = 1;
        for(int i=0;i<9;i++) {
            axle[i]->move();
            axle[i]->apply();
        }
    }else if(currentPoint < maxPoint) {
        int r=0;
        while(1) {
            Rack *rack = axle[r];
            if(rack->curr+1 <= rack->max) {
                rack->move();
                break;
            }else {
                rack->move();
                r++;
            }
            if(r == 9)
                r = 0;
        }
        currentPoint++;
        for(int i=0;i<9;i++) {
            axle[i]->apply();
        }

    }else {
        endPoint = true;
    }
    if(currentPoint == maxPoint)
        endPoint = true;
}

void MeasurementWorker::nextReps()
{
    emit setCommand(C_RESET);

    if(currentReps == 0) {
        currentReps = 1;
        endReps = false;
    }else if (currentReps == reps) {
        currentReps = 1;
        endReps = false;
    }else {
        currentReps++;
    }
    if(currentReps == reps)
        endReps = true;

    emit debug("MeasWorker:nextReps() - Reset counters");
}

void MeasurementWorker::waitForRecord()
{
    emit debug("MeasWorker:waitForRecord() - Wait for charged");

    while(1) {
        QApplication::processEvents();
        if(isChargedHV && isTurnOnHV)
            break;
        if(stop) {
            return;
        }
    }

    emit insertRowTableMeasurementDialog();
    emit setCommand(C_START);
    startedHour = QDateTime::currentDateTime().toString(formatTime);
    isStoppedCounting = false;
    emit debug("MeasWorker:waitForRecord() - Started counting");
    while(1) {
        QApplication::processEvents();
        if(isStoppedCounting)
            break;
        if(stop)
            return;

    }
    emit debug("MeasWorker:waitForRecord() - Finished counting");
    report->insertRecord(startedHour,currentSource,currentPoint,tagPoints.at(currentPoint-1),currentReps,countersStatus);
}

void MeasurementWorker::quit()
{
    if(stop)
        return;

    stop = true;
    emit debug("MeasWorker:quit() - Measurement canceled");
    emit finished(id,STATUS_CANCELED,lastError);
}

void MeasurementWorker::addError(QString error)
{
    lastError.append(QDateTime::currentDateTime().time().toString(formatTime)+" "+error+"\n");
}

QStringList MeasurementWorker::generatorRecordRT()
{
    QStringList ret;
    if(currentPoint == 0)
        return ret;
    ret << startedHour
        << QString::number(currentSource)
        << QString::number(currentPoint)+" "+tagPoints.at(currentPoint-1)
        << QString::number(currentReps)
        << "";

    ret << QString::number(static_cast<double>(countersStatus.at(TDCR_AB))/(static_cast<double>(countersStatus.at(TDCR_LT))/freqClk),'f',2)
        << QString::number(static_cast<double>(countersStatus.at(TDCR_BC))/(static_cast<double>(countersStatus.at(TDCR_LT))/freqClk),'f',2)
        << QString::number(static_cast<double>(countersStatus.at(TDCR_AC))/(static_cast<double>(countersStatus.at(TDCR_LT))/freqClk),'f',2)
        << QString::number(static_cast<double>(countersStatus.at(TDCR_T))/(static_cast<double>(countersStatus.at(TDCR_LT))/freqClk),'f',2)
        << QString::number(static_cast<double>(countersStatus.at(TDCR_D))/(static_cast<double>(countersStatus.at(TDCR_LT))/freqClk),'f',2)
        << QString::number(static_cast<double>(countersStatus.at(TDCR_T))/static_cast<double>(countersStatus.at(TDCR_D)),'f',4)
        << QString::number((1-(static_cast<double>(countersStatus.at(TDCR_LT))/static_cast<double>(countersStatus.at(MEAS_REAL_TIME))))*100,'g',2)
        << ""
        << QString::number(static_cast<double>(countersStatus.at(GAMMA_G))/(static_cast<double>(countersStatus.at(GAMMA_LT))/freqClk),'f',2)
        << QString::number(static_cast<double>(countersStatus.at(GAMMA_T))/(static_cast<double>(countersStatus.at(GAMMA_LT))/freqClk),'f',2)
        << QString::number(static_cast<double>(countersStatus.at(GAMMA_D))/(static_cast<double>(countersStatus.at(GAMMA_LT))/freqClk),'f',2)
        << QString::number(static_cast<double>(countersStatus.at(GAMMA_GT))/(static_cast<double>(countersStatus.at(GAMMA_LT))/freqClk),'f',2)
        << QString::number(static_cast<double>(countersStatus.at(GAMMA_GD))/(static_cast<double>(countersStatus.at(GAMMA_LT))/freqClk),'f',2)
        << QString::number(static_cast<double>(countersStatus.at(GAMMA_GD))/static_cast<double>(countersStatus.at(GAMMA_G)),'f',4)
        << QString::number(static_cast<double>(countersStatus.at(GAMMA_GT))/static_cast<double>(countersStatus.at(GAMMA_G)),'f',4)
        << QString::number(static_cast<double>(countersStatus.at(GAMMA_GD))/static_cast<double>(countersStatus.at(GAMMA_D)),'f',4)
        << QString::number((1-(static_cast<double>(countersStatus.at(GAMMA_LT))/static_cast<double>(countersStatus.at(MEAS_REAL_TIME))))*100,'g',2)
        << "";

    ret << QString::number((static_cast<int>(countersStatus.at(TDCR_A)+countersStatus.at(TDCR_B)+countersStatus.at(TDCR_C))-static_cast<int>(countersStatus.at(TDCR_S)+countersStatus.at(TDCR_T)+countersStatus.at(TDCR_D))),'f',0) //Rel.1
        << QString::number((static_cast<int>(countersStatus.at(TDCR_AB)+countersStatus.at(TDCR_BC)+countersStatus.at(TDCR_AC))-static_cast<int>(countersStatus.at(TDCR_T)*2+countersStatus.at(TDCR_D))),'f',0) // Rel.2
        << QString::number(static_cast<int>(countersStatus.at(GAMMA_A)+countersStatus.at(GAMMA_B)+countersStatus.at(GAMMA_C))-static_cast<int>(countersStatus.at(GAMMA_S)+countersStatus.at(GAMMA_T)+countersStatus.at(GAMMA_D)),'f',0) // Rel.3
        << QString::number(static_cast<int>((countersStatus.at(GAMMA_A)+countersStatus.at(GAMMA_B)+countersStatus.at(GAMMA_C)+countersStatus.at(GAMMA_G))-(countersStatus.at(GAMMA_COM)+countersStatus.at(GAMMA_GS)+countersStatus.at(GAMMA_D)+countersStatus.at(GAMMA_T))),'f',0) //Rel.4
        << QString::number(static_cast<int>((countersStatus.at(GAMMA_G)+countersStatus.at(GAMMA_S))-(countersStatus.at(GAMMA_COM)+countersStatus.at(GAMMA_GS))),'f',0) //Rel.5
        << QString::number(static_cast<int>((countersStatus.at(GAMMA_GT)+countersStatus.at(GAMMA_Q))-(countersStatus.at(GAMMA_T)+countersStatus.at(GAMMA_GD))),'f',0)
        << "-";
    for(int i=TDCR_A;i<PM_A;i++)
        ret << QString::number(countersStatus.at(i));
    ret << QString::number(countersStatus.at(MEAS_REAL_TIME));

    return ret;
}

QStringList MeasurementWorker::generatorRecordPM()
{
    QStringList ret;
    if(currentPoint == 0)
        return ret;
    double Na = static_cast<double>(countersStatus.at(PM_A))/(static_cast<double>(countersStatus.at(PM_LT_A))/freqClk);
    double Nb = static_cast<double>(countersStatus.at(PM_B))/(static_cast<double>(countersStatus.at(PM_LT_B))/freqClk);
    double Nc = static_cast<double>(countersStatus.at(PM_C))/(static_cast<double>(countersStatus.at(PM_LT_C))/freqClk);
    double Ng = static_cast<double>(countersStatus.at(PM_G))/(static_cast<double>(countersStatus.at(PM_LT_G))/freqClk);

    double Sab = static_cast<double>(countersStatus.at(PM_AB))/(static_cast<double>(countersStatus.at(PM_LT_AB))/freqClk);
    double Sbc = static_cast<double>(countersStatus.at(PM_BC))/(static_cast<double>(countersStatus.at(PM_LT_BC))/freqClk);
    double Sac = static_cast<double>(countersStatus.at(PM_AC))/(static_cast<double>(countersStatus.at(PM_LT_AC))/freqClk);
    double Sag = static_cast<double>(countersStatus.at(PM_AG))/(static_cast<double>(countersStatus.at(PM_LT_AG))/freqClk);
    double Sbg = static_cast<double>(countersStatus.at(PM_BG))/(static_cast<double>(countersStatus.at(PM_LT_BG))/freqClk);
    double Scg = static_cast<double>(countersStatus.at(PM_CG))/(static_cast<double>(countersStatus.at(PM_LT_CG))/freqClk);

    double Sabc = static_cast<double>(countersStatus.at(PM_ABC))/(static_cast<double>(countersStatus.at(PM_LT_ABC))/freqClk);
    double Sabg = static_cast<double>(countersStatus.at(PM_ABG))/(static_cast<double>(countersStatus.at(PM_LT_ABG))/freqClk);
    double Sbcg = static_cast<double>(countersStatus.at(PM_BCG))/(static_cast<double>(countersStatus.at(PM_LT_BCG))/freqClk);
    double Sacg = static_cast<double>(countersStatus.at(PM_ACG))/(static_cast<double>(countersStatus.at(PM_LT_ACG))/freqClk);

    double Sabcg = static_cast<double>(countersStatus.at(PM_ABCG))/(static_cast<double>(countersStatus.at(PM_LT_ABCG))/freqClk);

    double Nab = Na + Nb - Sab;
    double Nbc = Nc + Nb - Sbc;
    double Nac = Na + Nc - Sac;
    double Nt = Na + Nb + Nc - Sab - Sbc - Sac + Sabc;
    double Nd = Sab + Sbc + Sac - 2*Sabc;

    double Ngs = Ng + Sabc - Sabcg;
    double Ngd = Ng + Sab + Sbc + Sac - 2*Sabc - Sabg - Sbcg - Sacg + 2*Sabcg;
    double Ngt = Na + Nb + Nc + Ng - Sab - Sbc - Sac - Sag - Sbg - Scg + Sabc + Sabg + Sbcg + Sacg - Sabcg;
    double Nq = Sab + Sbc + Sac + Sag + Sbg + Scg - 2*Sabc - 2*Sabg - 2*Sbcg - 2*Sacg + 3*Sabcg;

    ret << startedHour
        << QString::number(currentSource)
        << QString::number(currentPoint)+" "+tagPoints.at(currentPoint-1)
        << QString::number(currentReps)
        << "";

    ret << QString::number(Nab,'f',2)
        << QString::number(Nbc,'f',2)
        << QString::number(Nac,'f',2)
        << QString::number(Nt,'f',2)
        << QString::number(Nd,'f',2)
        << QString::number(Nt/Nd,'f',4)
        << QString::number((1-(static_cast<double>(countersStatus.at(PM_LT_ABC))/static_cast<double>(countersStatus.at(MEAS_REAL_TIME))))*100,'g',2)
        << ""
        << QString::number(Ng,'f',2)
        << QString::number(Nt,'f',2)
        << QString::number(Nd,'f',2)
        << QString::number(Ngt,'f',2)
        << QString::number(Ngd,'f',2)
        << QString::number(Ngd/Ng,'f',4)
        << QString::number(Ngt/Ng,'f',4)
        << QString::number(Ngd/Nd,'f',4)
        << QString::number((1-(static_cast<double>(countersStatus.at(PM_LT_ABCG))/static_cast<double>(countersStatus.at(MEAS_REAL_TIME))))*100,'g',2)
        << "";

    ret << QString::number(round((Na+Nb+Nc)-(Sabc+Nt+Nd)))
        << QString::number(round((Nab+Nbc+Nac)-(2*Nt+Nd)))
        << QString::number(round((Na+Nb+Nc)-(Sabc+Nt+Nd)))
        << QString::number(round((Na+Nb+Nc+Ng)-(Sabcg+Ngs+Nt+Nd)))
        << QString::number(round((Ng+Sabc)-(Sabcg+Ngs)))
        << QString::number(round((Ngt+Nq)-(Nt+Ngd)))
        << "";
    for(int i=PM_A;i<MEAS_REAL_TIME;i++)
        ret << QString::number(countersStatus.at(i));
    ret << QString::number(countersStatus.at(MEAS_REAL_TIME));

    return ret;
}

void MeasurementWorker::resetPoint()
{
    endPoint = false;
    endReps = false;
    axle.clear();
    axle << new Rack(hvFocusingDynodeBeta,HV_MODULE,CH_HVFOCUSINGDYNODEBETA);
    axle << new Rack(hvAnodeBeta,HV_MODULE,CH_HVANODEBETA);
    axle << new Rack(hvFirstDynodeBeta,HV_MODULE,CH_HVFIRSTDYNODEBETA);
    axle << new Rack(hvAnodeGamma,HV_MODULE,CH_HVANODEGAMMA);
    axle << new Rack(resolvintTimeBeta,ANALYZER_MODULE,CH_RESOLVINGTIMEBETA);
    axle << new Rack(deadTimeBeta,ANALYZER_MODULE,CH_DEADTIMEBETA);
    axle << new Rack(resolvingTimeGamma,ANALYZER_MODULE,CH_RESOLVINGTIMEGAMMA);
    axle << new Rack(deadTimeGamma,ANALYZER_MODULE,CH_DEADTIMEGAMMA);
    axle << new Rack(delayTimeBeta,ANALYZER_MODULE,CH_DELAYTIMEBETA);

    currentPoint = 0;
    currentReps = 0;
    maxPoint = 1;

    tagPoints.clear();
    for(int i=0;i<axle.count();i++) {
        maxPoint *= axle[i]->max;
        connect(axle.at(i),SIGNAL(setVolt(float,int)),this,SIGNAL(setVoltage(float,int)));
        connect(axle.at(i),SIGNAL(setTime(int,int)),this,SIGNAL(setTime(int,int)));
        if(axle[i]->max > 1) {
            if(tagPoints.isEmpty()) {
                tagPoints << axle[i]->tags;
            }else {
                QStringList tmp;
                foreach(QString tag, axle[i]->tags) {
                    foreach(QString point, tagPoints) {
                        tmp << tag+"|"+point;
                    }
                }
                tagPoints = tmp;
            }
        }
    }
    if(tagPoints.isEmpty())
        tagPoints << "";
}

bool MeasurementWorker::isChannelLimited()
{
    if(valueCountLimited !=0) {
        switch (channelLimited) {
        case D_BETA:
        {
            if(countersStatus.at(TDCR_D) >= static_cast<unsigned long>(valueCountLimited))
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*static_cast<double>(valueCountLimited))/static_cast<double>(countersStatus.at(TDCR_D));
                statusProcess.currentTime = static_cast<int>(mt);
                statusProcess.time = static_cast<int>(time);
                return true;
            }
        }
        case T_BETA:
        {
            if(countersStatus.at(TDCR_T) >= (unsigned long)valueCountLimited)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*static_cast<double>(valueCountLimited))/static_cast<double>(countersStatus.at(TDCR_T));
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case S_BETA:
        {
            if(countersStatus.at(TDCR_S) >= (unsigned long)valueCountLimited)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*static_cast<double>(valueCountLimited))/static_cast<double>(countersStatus.at(TDCR_S));
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case G_GAMMA:
        {
            if(countersStatus.at(GAMMA_G) >= (unsigned long)valueCountLimited)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(double)valueCountLimited)/(double)countersStatus.at(GAMMA_G);
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case COM_GAMMA:
        {
            if(countersStatus.at(GAMMA_COM) >= (unsigned long)valueCountLimited)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(double)valueCountLimited)/(double)countersStatus.at(GAMMA_COM);
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case D_GAMMA:
        {
            if(countersStatus.at(GAMMA_D) >= (unsigned long)valueCountLimited)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(double)valueCountLimited)/(double)countersStatus.at(GAMMA_D);
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case T_GAMMA:
        {
            if(countersStatus.at(GAMMA_T) >= (unsigned long)valueCountLimited)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(double)valueCountLimited)/(double)countersStatus.at(GAMMA_T);
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case S_GAMMA:
        {
            if(countersStatus.at(GAMMA_S) >= (unsigned long)valueCountLimited)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(double)valueCountLimited)/(double)countersStatus.at(GAMMA_S);
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case GD_GAMMA:
        {
            if(countersStatus.at(GAMMA_GD) >= (unsigned long)valueCountLimited)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(double)valueCountLimited)/(double)countersStatus.at(GAMMA_GD);
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case GT_GAMMA:
        {
            if(countersStatus.at(GAMMA_GT) >= (unsigned long)valueCountLimited)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(double)valueCountLimited)/(double)countersStatus.at(GAMMA_GT);
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        default:
            return false;
        }
    }else if(valueUncertaintlyLimited != 0) {
        switch (channelLimited) {
        case D_BETA:
        {
            double uncer = sqrt((double)countersStatus.at(TDCR_D))/(double)countersStatus.at(TDCR_D);
            if( uncer <= valueUncertaintlyLimited/100)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(valueUncertaintlyLimited/100))/uncer;
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case T_BETA:
        {
            double uncer = sqrt((double)countersStatus.at(TDCR_T))/(double)countersStatus.at(TDCR_T);
            if( uncer <= valueUncertaintlyLimited/100)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(valueUncertaintlyLimited/100))/uncer;
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case S_BETA:
        {
            double uncer = sqrt((double)countersStatus.at(TDCR_S))/(double)countersStatus.at(TDCR_S);
            if( uncer <= valueUncertaintlyLimited/100)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(valueUncertaintlyLimited/100))/uncer;
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case TD_BETA:
        {
            double d = sqrt((double)countersStatus.at(TDCR_D))/(double)countersStatus.at(TDCR_D);
            double t = sqrt((double)countersStatus.at(TDCR_T))/(double)countersStatus.at(TDCR_T);
            double uncer = sqrt(pow(d,2)+pow(t,2));
            if( uncer <= valueUncertaintlyLimited/100)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(valueUncertaintlyLimited/100))/uncer;
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case G_GAMMA:
        {
            double uncer = sqrt((double)countersStatus.at(GAMMA_G))/(double)countersStatus.at(GAMMA_G);
            if( uncer <= valueUncertaintlyLimited/100)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(valueUncertaintlyLimited/100))/uncer;
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case COM_GAMMA:
        {
            double uncer = sqrt((double)countersStatus.at(GAMMA_COM))/(double)countersStatus.at(GAMMA_COM);
            if( uncer <= valueUncertaintlyLimited/100)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(valueUncertaintlyLimited/100))/uncer;
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case D_GAMMA:
        {
            double uncer = sqrt((double)countersStatus.at(GAMMA_D))/(double)countersStatus.at(GAMMA_D);
            if( uncer <= valueUncertaintlyLimited/100)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(valueUncertaintlyLimited/100))/uncer;
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case T_GAMMA:
        {
            double uncer = sqrt((double)countersStatus.at(GAMMA_T))/(double)countersStatus.at(GAMMA_T);
            if( uncer <= valueUncertaintlyLimited/100)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(valueUncertaintlyLimited/100))/uncer;
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case S_GAMMA:
        {
            double uncer = sqrt((double)countersStatus.at(GAMMA_S))/(double)countersStatus.at(GAMMA_S);
            if( uncer <= valueUncertaintlyLimited/100)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(valueUncertaintlyLimited/100))/uncer;
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case GD_GAMMA:
        {
            double uncer = sqrt((double)countersStatus.at(GAMMA_GD))/(double)countersStatus.at(GAMMA_GD);
            if( uncer <= valueUncertaintlyLimited/100)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(valueUncertaintlyLimited/100))/uncer;
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case GT_GAMMA:
        {
            double uncer = sqrt((double)countersStatus.at(GAMMA_GT))/(double)countersStatus.at(GAMMA_GT);
            if( uncer <= valueUncertaintlyLimited/100)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(valueUncertaintlyLimited/100))/uncer;
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case AD_GAMMA:
        {
            double b = sqrt((double)countersStatus.at(GAMMA_D))/(double)countersStatus.at(GAMMA_D);
            double g = sqrt((double)countersStatus.at(GAMMA_G))/(double)countersStatus.at(GAMMA_G);
            double c = sqrt((double)countersStatus.at(GAMMA_GD))/(double)countersStatus.at(GAMMA_GD);
            double uncer = sqrt(pow(b,2)+pow(g,2)+pow(c,2));
            if( uncer <= valueUncertaintlyLimited/100)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(valueUncertaintlyLimited/100))/uncer;
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        case AT_GAMMA:
        {
            double b = sqrt((double)countersStatus.at(GAMMA_T))/(double)countersStatus.at(GAMMA_T);
            double g = sqrt((double)countersStatus.at(GAMMA_G))/(double)countersStatus.at(GAMMA_G);
            double c = sqrt((double)countersStatus.at(GAMMA_GT))/(double)countersStatus.at(GAMMA_GT);
            double uncer = sqrt(pow(b,2)+pow(g,2)+pow(c,2));
            if( uncer <= valueUncertaintlyLimited/100)
                return false;
            else {
                double mt = static_cast<double>(countersStatus.at(MEAS_REAL_TIME))/freqClk;
                double time = (mt*(valueUncertaintlyLimited/100))/uncer;
                statusProcess.currentTime = (int)mt;
                statusProcess.time = (int)time;
                return true;
            }
        }
        default:
            return false;
        }
    }else {
        return false;
    }
}
