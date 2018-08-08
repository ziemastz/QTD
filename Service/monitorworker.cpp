#include "monitorworker.h"

MonitorWorker::MonitorWorker(QObject *parent) : QObject(parent)
{
    stop = false;
    timeMeas = 0;
}

void MonitorWorker::start()
{
    isStartedAnalyzer = false;
    isStartedHV = false;
    stop = false;
    clear();

    emit initHV();
    emit initAnalyzer();
    qDebug() << ("MonitorWorker:start() - Initiation of PowerSupplyHV and Analyzer");
    while(1) {
        QApplication::processEvents();
        if(isStartedAnalyzer && isStartedHV)
            break;
        if(stop){
            return;
        }
    }
    emit setHV(0,0);
    emit setHV(0,1);
    emit setHV(0,2);
    emit setHV(0,3);
    emit setTurnOn(true);
    qDebug() << ("MonitorWorker:start() - Turn on HV");
    emit setCommand(C_RESET);
    emit setCommand(C_START);
    isStoppedCounting = true;
}

void MonitorWorker::quit()
{
    if(stop)
        return;

    stop = true;
    isStoppedCounting = true;
    emit finished();
}

void MonitorWorker::startMeas(int time)
{
    emit setCommand(C_RESET);
    emit setCommand(C_START);
    timeMeas = time;
    if(timeMeas > 0)
        isStoppedCounting = false;
    else
        isStoppedCounting = true;
}

void MonitorWorker::stopMeas()
{
    emit setCommand(C_STOP);
}

void MonitorWorker::resetMeas()
{
    emit setCommand(C_RESET);
}
void MonitorWorker::startedHV()
{
    isStartedHV = true;
}

void MonitorWorker::startedAnalyzer()
{
    isStartedAnalyzer = true;
}

void MonitorWorker::finishedHV()
{
    isStartedHV = false;
    quit();
}

void MonitorWorker::finishedAnalyzer()
{
    isStartedAnalyzer = false;
    quit();
}
void MonitorWorker::statusAnalyzer(QString stat, QVector<int> timeCh, QVector<unsigned long> counters)
{
    //emit debug("MeasWorker():statusAnalyzer() - Received status");
    statusParameters.resolvingTimeBeta = QString::number(timeCh[CH_RESOLVINGTIMEBETA]);
    statusParameters.deadTimeBeta = QString::number(timeCh[CH_DEADTIMEBETA]);
    statusParameters.resolvingTimeGamma = QString::number(timeCh[CH_RESOLVINGTIMEGAMMA]);
    statusParameters.deadTimeGamma = QString::number(timeCh[CH_DEADTIMEGAMMA]);
    statusParameters.delayTimeBeta = QString::number(timeCh[CH_DELAYTIMEBETA]);
    statusParameters.statusAnalyzer = stat;
    emit setStatusParametersMonitorDialog(statusParameters);

    if(counters.count() != 57) {
        emit error("MonitorWorker():statusAnalyzer() - Failed reading data");
        return;
    }

    if(statusParameters.statusAnalyzer == QCoreApplication::translate("MeasurementWorker","Stopped")){
        isStoppedCounting = true;
    }

    if(!isStoppedCounting) {
        countersStatus = counters;
        if((int)((float)(counters[MEAS_REAL_TIME])/freqClk) >= timeMeas) {
            emit setCommand(C_STOP);
            QThread::msleep(500);

        }

    }else {
        if(statusParameters.statusAnalyzer == QCoreApplication::translate("MeasurementWorker","Started")) {
            if(!beforCounterStatus.isEmpty() && !beforbeforCounterStatus.isEmpty()) {
                countersStatus.clear();
                for(int i=0; i<counters.size(); i++) {
                    countersStatus <<  counters.at(i) - beforbeforCounterStatus.at(i);
                }
            }else if(!beforCounterStatus.isEmpty()) {
                countersStatus.clear();
                for(int i=0; i<counters.size(); i++) {
                    countersStatus <<  counters.at(i) - beforCounterStatus.at(i);
                }
            }else {
                countersStatus = counters;
            }
            beforbeforCounterStatus = beforCounterStatus;
            beforCounterStatus = counters;
        }else {
            countersStatus = counters;
        }

    }

    statusTrack.cpsA = (double)(countersStatus[PM_A])/((double)(countersStatus[PM_LT_A])/freqClk);
    statusTrack.cpsB = (double)(countersStatus[PM_B])/((double)(countersStatus[PM_LT_B])/freqClk);
    statusTrack.cpsC = (double)(countersStatus[PM_C])/((double)(countersStatus[PM_LT_C])/freqClk);
    statusTrack.cpsG = (double)(countersStatus[PM_G])/((double)(countersStatus[PM_LT_G])/freqClk);

    statusTrack.countsA = (int)(countersStatus[PM_A]);
    statusTrack.countsB = (int)(countersStatus[PM_B]);
    statusTrack.countsC = (int)(countersStatus[PM_C]);
    statusTrack.countsG = (int)(countersStatus[PM_G]);

    statusTrack.deatTimeA = (int)(100 - ((double)(countersStatus[PM_LT_A])*100/(double)countersStatus[MEAS_REAL_TIME]));
    statusTrack.deatTimeB = (int)(100 - ((double)(countersStatus[PM_LT_B])*100/(double)countersStatus[MEAS_REAL_TIME]));
    statusTrack.deatTimeC = (int)(100 - ((double)(countersStatus[PM_LT_C])*100/(double)countersStatus[MEAS_REAL_TIME]));
    statusTrack.deatTimeG = (int)(100 - ((double)(countersStatus[PM_LT_G])*100/(double)countersStatus[MEAS_REAL_TIME]));

    statusTrack.efficiencyTDCR = (int)((double)(countersStatus[TDCR_T])*100/(double)countersStatus[TDCR_D]);
    statusTrack.efficiencyBeta4LS = (int)((double)(countersStatus[GAMMA_GD])*100/(double)countersStatus[GAMMA_G]);
    statusTrack.efficiencyGamma4LS = (int)(((double)countersStatus[GAMMA_GD])*100/(double)countersStatus[GAMMA_D]);

    emit setStatusTrackMonitorDialog(statusTrack);

    //if time is ok
    emit setRTTableMonitorDialog(generatorRecordRT());
    emit setPMTableMonitorDialog(generatorRecordPM());

    if(statusParameters.statusAnalyzer == QCoreApplication::translate("MeasurementWorker","Stopped")){
        emit stoppedMeas();
    }
}

void MonitorWorker::statusPowerSupplyHV(QVector<QString> statCh, QVector<float> voltCh, QVector<float> ampCh)
{

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
    emit setStatusParametersMonitorDialog(statusParameters);

}

void MonitorWorker::debug(QString text)
{
    qDebug() << text;

}

void MonitorWorker::error(QString text)
{
    qWarning() << text;
}

void MonitorWorker::clear()
{
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

    statusParameters.statusAnode.clear();
    statusParameters.statusFirstDynode.clear();
    statusParameters.statusFocusingDynode.clear();
    statusParameters.statusGamma.clear();
    statusParameters.voltAnode = 0;
    statusParameters.voltFirstDynode = 0;
    statusParameters.voltFocusingDynode = 0;
    statusParameters.voltGamma = 0;
    statusParameters.ampAnode = 0;
    statusParameters.ampFirstDynode = 0;
    statusParameters.ampFocusingDynode = 0;
    statusParameters.ampGamma = 0;

    emit setStatusParametersMonitorDialog(statusParameters);
    emit setStatusTrackMonitorDialog(statusTrack);
}

QStringList MonitorWorker::generatorRecordRT()
{
    QStringList ret;

    ret << QString::number((float)(countersStatus.at(TDCR_AB))/((float)(countersStatus.at(TDCR_LT))/freqClk),'f',2)
        << QString::number((float)(countersStatus.at(TDCR_BC))/((float)(countersStatus.at(TDCR_LT))/freqClk),'f',2)
        << QString::number((float)(countersStatus.at(TDCR_AC))/((float)(countersStatus.at(TDCR_LT))/freqClk),'f',2)
        << QString::number((float)(countersStatus.at(TDCR_T))/((float)(countersStatus.at(TDCR_LT))/freqClk),'f',2)
        << QString::number((float)(countersStatus.at(TDCR_D))/((float)(countersStatus.at(TDCR_LT))/freqClk),'f',2)
        << QString::number((float)(countersStatus.at(TDCR_T))/(float)(countersStatus.at(TDCR_D)),'f',4)
        << QString::number((1-((float)(countersStatus.at(TDCR_LT))/(float)(countersStatus.at(MEAS_REAL_TIME))))*100,'g',2)
        << "-"
        << QString::number((float)(countersStatus.at(GAMMA_G))/((float)(countersStatus.at(GAMMA_LT))/freqClk),'f',2)
        << QString::number((float)(countersStatus.at(GAMMA_T))/((float)(countersStatus.at(GAMMA_LT))/freqClk),'f',2)
        << QString::number((float)(countersStatus.at(GAMMA_D))/((float)(countersStatus.at(GAMMA_LT))/freqClk),'f',2)
        << QString::number((float)(countersStatus.at(GAMMA_GT))/((float)(countersStatus.at(GAMMA_LT))/freqClk),'f',2)
        << QString::number((float)(countersStatus.at(GAMMA_GD))/((float)(countersStatus.at(GAMMA_LT))/freqClk),'f',2)
        << QString::number((float)(countersStatus.at(GAMMA_GD))/(float)(countersStatus.at(GAMMA_G)),'f',4)
        << QString::number((float)(countersStatus.at(GAMMA_GT))/(float)(countersStatus.at(GAMMA_G)),'f',4)
        << QString::number((float)(countersStatus.at(GAMMA_GD))/(float)(countersStatus.at(GAMMA_D)),'f',4)
        << QString::number((1-((float)(countersStatus.at(GAMMA_LT))/(float)(countersStatus.at(MEAS_REAL_TIME))))*100,'g',2)
        << "-";

    ret << QString::number((int)((countersStatus.at(TDCR_A)+countersStatus.at(TDCR_B)+countersStatus.at(TDCR_C))-(countersStatus.at(TDCR_S)+countersStatus.at(TDCR_T)+countersStatus.at(TDCR_D))),'f',0) //Rel.1
        << QString::number((int)((countersStatus.at(TDCR_AB)+countersStatus.at(TDCR_BC)+countersStatus.at(TDCR_AC))-(countersStatus.at(TDCR_T)*2+countersStatus.at(TDCR_D))),'f',0) // Rel.2
        << QString::number((int)(countersStatus.at(GAMMA_A)+countersStatus.at(GAMMA_B)+countersStatus.at(GAMMA_C))-(int)(countersStatus.at(GAMMA_S)+countersStatus.at(GAMMA_T)+countersStatus.at(GAMMA_D)),'f',0) // Rel.3
        << QString::number((int)((countersStatus.at(GAMMA_A)+countersStatus.at(GAMMA_B)+countersStatus.at(GAMMA_C)+countersStatus.at(GAMMA_G))-(countersStatus.at(GAMMA_COM)+countersStatus.at(GAMMA_GS)+countersStatus.at(GAMMA_D)+countersStatus.at(GAMMA_T))),'f',0) //Rel.4
        << QString::number((int)((countersStatus.at(GAMMA_G)+countersStatus.at(GAMMA_S))-(countersStatus.at(GAMMA_COM)+countersStatus.at(GAMMA_GS))),'f',0) //Rel.5
        << QString::number((int)((countersStatus.at(GAMMA_GT)+countersStatus.at(GAMMA_Q))-(countersStatus.at(GAMMA_T)+countersStatus.at(GAMMA_GD))),'f',0)
        << "-";
    for(int i=TDCR_A;i<PM_A;i++)
        ret << QString::number(countersStatus.at(i));
    ret << QString::number(countersStatus.at(MEAS_REAL_TIME));

    return ret;
}

QStringList MonitorWorker::generatorRecordPM()
{
    QStringList ret;

    double Na = (double)(countersStatus.at(PM_A))/((double)(countersStatus.at(PM_LT_A))/freqClk);
    double Nb = (double)(countersStatus.at(PM_B))/((double)(countersStatus.at(PM_LT_B))/freqClk);
    double Nc = (double)(countersStatus.at(PM_C))/((double)(countersStatus.at(PM_LT_C))/freqClk);
    double Ng = (double)(countersStatus.at(PM_G))/((double)(countersStatus.at(PM_LT_G))/freqClk);

    double Sab = (double)(countersStatus.at(PM_AB))/((double)(countersStatus.at(PM_LT_AB))/freqClk);
    double Sbc = (double)(countersStatus.at(PM_BC))/((double)(countersStatus.at(PM_LT_BC))/freqClk);
    double Sac = (double)(countersStatus.at(PM_AC))/((double)(countersStatus.at(PM_LT_AC))/freqClk);
    double Sag = (double)(countersStatus.at(PM_AG))/((double)(countersStatus.at(PM_LT_AG))/freqClk);
    double Sbg = (double)(countersStatus.at(PM_BG))/((double)(countersStatus.at(PM_LT_BG))/freqClk);
    double Scg = (double)(countersStatus.at(PM_CG))/((double)(countersStatus.at(PM_LT_CG))/freqClk);

    double Sabc = (double)(countersStatus.at(PM_ABC))/((double)(countersStatus.at(PM_LT_ABC))/freqClk);
    double Sabg = (double)(countersStatus.at(PM_ABG))/((double)(countersStatus.at(PM_LT_ABG))/freqClk);
    double Sbcg = (double)(countersStatus.at(PM_BCG))/((double)(countersStatus.at(PM_LT_BCG))/freqClk);
    double Sacg = (double)(countersStatus.at(PM_ACG))/((double)(countersStatus.at(PM_LT_ACG))/freqClk);

    double Sabcg = (double)(countersStatus.at(PM_ABCG))/((double)(countersStatus.at(PM_LT_ABCG))/freqClk);

    double Nab = Na + Nb - Sab;
    double Nbc = Nc + Nb - Sbc;
    double Nac = Na + Nc - Sac;
    double Nt = Na + Nb + Nc - Sab - Sbc - Sac + Sabc;
    double Nd = Sab + Sbc + Sac - 2*Sabc;

    double Ngs = Ng + Sabc - Sabcg;
    double Ngd = Ng + Sab + Sbc + Sac - 2*Sabc - Sabg - Sbcg - Sacg + 2*Sabcg;
    double Ngt = Na + Nb + Nc + Ng - Sab - Sbc - Sac - Sag - Sbg - Scg + Sabc + Sabg + Sbcg + Sacg - Sabcg;
    double Nq = Sab + Sbc + Sac + Sag + Sbg + Scg - 2*Sabc - 2*Sabg - 2*Sbcg - 2*Sacg + 3*Sabcg;

    ret << QString::number(Nab,'f',2)
        << QString::number(Nbc,'f',2)
        << QString::number(Nac,'f',2)
        << QString::number(Nt,'f',2)
        << QString::number(Nd,'f',2)
        << QString::number(Nt/Nd,'f',4)
        << QString::number((1-((float)(countersStatus.at(PM_LT_ABC))/(float)(countersStatus.at(MEAS_REAL_TIME))))*100,'g',2)
        << "-"
        << QString::number(Ng,'f',2)
        << QString::number(Nt,'f',2)
        << QString::number(Nd,'f',2)
        << QString::number(Ngt,'f',2)
        << QString::number(Ngd,'f',2)
        << QString::number(Ngd/Ng,'f',4)
        << QString::number(Ngt/Ng,'f',4)
        << QString::number(Ngd/Nd,'f',4)
        << QString::number((1-((float)(countersStatus.at(PM_LT_ABCG))/(float)(countersStatus.at(MEAS_REAL_TIME))))*100,'g',2)
        << "-";

    ret << QString::number(round((Na+Nb+Nc)-(Sabc+Nt+Nd)))
        << QString::number(round((Nab+Nbc+Nac)-(2*Nt+Nd)))
        << QString::number(round((Na+Nb+Nc)-(Sabc+Nt+Nd)))
        << QString::number(round((Na+Nb+Nc+Ng)-(Sabcg+Ngs+Nt+Nd)))
        << QString::number(round((Ng+Sabc)-(Sabcg+Ngs)))
        << QString::number(round((Ngt+Nq)-(Nt+Ngd)))
        << "-";
    for(int i=PM_A;i<MEAS_REAL_TIME;i++)
        ret << QString::number(countersStatus.at(i));
    ret << QString::number(countersStatus.at(MEAS_REAL_TIME));

    return ret;
}
