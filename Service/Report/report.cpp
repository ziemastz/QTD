#include "report.h"

Report::Report(MEASUREMENTDATA measData, SERIESSOURCESDATA seriesData, PARAMETERSDATA paramData, QString location, QObject *parent) : QObject(parent)
{
    if(location.isEmpty())
        location = QDir::currentPath();

    QDir dir(location+"/Report/"+measData.nuclide+"/"+measData.filename);
    if(!dir.exists()) {
        if(!dir.mkpath(dir.path())) {
            debug("Report:Report() - Cannot create the folder: "+dir.path());
            dir.setPath(QDir::homePath()+"/QTDControlSoftware/Report/"+measData.nuclide);
            if(!dir.mkpath(dir.path())) {
                debug("Report:Report() - Cannot create the folder: "+dir.path());
                dir.setPath(QDir::homePath()+"/QTDControlSoftware/Report");
                if(!dir.mkpath(dir.path())) {
                    debug("Report:Report() - Cannot create the folder: "+dir.path());
                }
            }
        }
    }

    QStringList lines;
    List list;
    QStringList tmp;

    filenameRT_TD = dir.path()+"/"+measData.filename+"_RT.td";
    filenameRT_QTD = dir.path()+"/"+measData.filename+"_RT.qtd";
    filenamePM_RAW = dir.path()+"/"+measData.filename+"_PM.qpm";
    filenameRT_RES = dir.path()+"/"+measData.filename+"_RT.res";
    filenamePM_RES = dir.path()+"/"+measData.filename+"_PM.res";

    filenameDebug = dir.path()+"/"+measData.filename+".log";
    filenameHV = dir.path()+"/"+measData.filename+"_HV.log";

    appendText(filenameHV,"Hour\tAnode(V)\tAnode(A)\tFirst Dynode(V)\tFirst Dynode(A)\tFocusing Dynode(V)\tFocusing Dynode(A)\tGamma(V)\tGamma(A)\n");
    filenameAnalyzer = dir.path()+"/"+measData.filename+"_Analyzer.log";
    tmp.clear();
    tmp << "A"
        << "B"
        << "C"
        << "S"
        << "AB"
        << "BC"
        << "AC"
        << "T"
        << "D"
        << "LT_B"
        << "G"
        << "A'"
        << "B'"
        << "C'"
        << "S'"
        << "AB'"
        << "BC'"
        << "AC'"
        << "T'"
        << "D'"
        << "COM"
        << "Q"
        << "GS"
        << "GD"
        << "GT"
        << "LT_G"
        << "PM A"
        << "PM LT A"
        << "PM B"
        << "PM LT B"
        << "PM C"
        << "PM LT C"
        << "PM G"
        << "PM LT G"
        << "PM AB"
        << "PM LT AB"
        << "PM BC"
        << "PM LT BC"
        << "PM AC"
        << "PM LT AC"
        << "PM AG"
        << "PM LT AG"
        << "PM BG"
        << "PM LT BG"
        << "PM CG"
        << "PM LT CG"
        << "PM ABC"
        << "PM LT ABC"
        << "PM ABG"
        << "PM LT ABG"
        << "PM BCG"
        << "PM LT BCG"
        << "PM ACG"
        << "PM LT ACG"
        << "PM ABCG"
        << "PM LT ABCG"
        << "MT";
    appendText(filenameAnalyzer,"Hour\t"+tmp.join("\t")+"\n");
    //info in  *.res
    tmp.clear();
    tmp << measData.filename+" "+measData.date
        << "Nuklid:"+measData.nuclide
        << "Komentarz: "+measData.nameSeriesSources+";"+measData.nameParameters+";"+measData.comment
        << "Scyntylator : "+seriesData.nameScintillator
        << "Polokres rozpadu : 0y 0d 0h 0m\nNiedokladnosc : 0.00000000000000E+0"
        << QString::number(paramData.tagPoints.count())
        << (paramData.tagPoints.count()>1 ? paramData.tagPoints.join(" ") : "1");

    appendText(filenameRT_RES,tmp.join("\n")+"\n");
    appendText(filenamePM_RES,tmp.join("\n")+"\n");

    //info headers
    tmp.clear();
    lines << "Report QTD Control Software (rel. "+QFileInfo(QCoreApplication::applicationFilePath()).lastModified().date().toString("yyyy.M.d)")
          << "===============================================================================\n";
    appendText(filenameRT_TD,lines.join("\n"));
    appendText(filenameRT_QTD,lines.join("\n"));
    appendText(filenamePM_RAW,lines.join("\n"));

    list.insert("ID",measData.id);
    list.insert("Laboratory",measData.company);
    list.insert("Counter",measData.counter);
    list.insert("Nuclide",measData.nuclide);
    list.insert("Filename",measData.filename);
    list.insert("Date",measData.date);
    list.insert("Mode",modeMeasurement.at(measData.mode));
    list.insert("","");
    list.insert("Sources Series",measData.nameSeriesSources);
    list.insert("Solution",seriesData.solution);
    list.insert("Dilution",QString::number(seriesData.dilution,'g',10));
    list.insert("Mother solution",seriesData.motherSolution);
    list.insert("Scintillator",seriesData.nameScintillator);
    list.insert("Volume scin.",seriesData.volumeScintillator);
    list.insert("Vial type",seriesData.vialType);
    masses.clear();
    for(int i=0;i<seriesData.idVials.count();i++) {
        masses << QString::number(seriesData.masses.at(i));
    }
    list.insert("Number vials",seriesData.idVials.join(" "));
    list.insert("Mass (mg)",masses.join(" "));
    list.insert("Prepared by",seriesData.preparedBy);
    list.insert("Prepared at",seriesData.preparedAt.toString(formatDate));
    list.insert("Last modification",seriesData.lastModification);
    list.insert("","");
    list.insert("Parameters",measData.nameParameters);
    tmp.clear();
    lines.clear();
    foreach (float val, paramData.hvAnodeBeta) {
       tmp << QString::number(val);
    }
    list.insert("HV Anode(V)",tmp.join(" "));
    tmp.clear();
    foreach (float val, paramData.hvFirstDynodeBeta) {
       tmp << QString::number(val);
    }
    list.insert("HV First Dynode(V)",tmp.join(" "));
    tmp.clear();
    foreach (float val, paramData.hvFocalysingDynodeBeta) {
       tmp << QString::number(val);
    }
    list.insert("HV Focusing Dynode(V)",tmp.join(" "));
    tmp.clear();
    foreach (float val, paramData.hvAnodeGamma) {
       tmp << QString::number(val);
    }
    list.insert("HV Gamma(V)",tmp.join(" "));
    tmp.clear();
    foreach (float val, paramData.resolvintTimeBeta) {
       tmp << QString::number(val);
    }
    list.insert("R1(ns)",tmp.join(" "));
    tmp.clear();
    foreach (float val, paramData.deadTimeBeta) {
       tmp << QString::number(val);
    }
    list.insert("DT1(ns)",tmp.join(" "));
    tmp.clear();
    foreach (float val, paramData.resolvingTimeGamma) {
       tmp << QString::number(val);
    }
    list.insert("R2(ns)",tmp.join(" "));
    tmp.clear();
    foreach (float val, paramData.deadTimeGamma) {
       tmp << QString::number(val);
    }
    list.insert("DT2(ns)",tmp.join(" "));
    tmp.clear();
    foreach (float val, paramData.delayTimeBeta) {
       tmp << QString::number(val);
    }
    list.insert("DLB(ns)",tmp.join(" "));

    list.insert("Lower window gamma",QString::number(paramData.lowerWindowGamma));
    list.insert("Upper window gamma",QString::number(paramData.upperWindowGamma));
    list.insert("Last modification",paramData.lastModification);
    list.insert("","");
    list.insert("Blank time(s)",QString::number(measData.backgroundTime));
    list.insert("Source time(s)",QString::number(measData.sourceTime));
    list.insert("Reps",QString::number(measData.reps)); //for *.res format
    blankTime = QString::number(measData.backgroundTime); //for *.res format
    sourceTime = QString::number(measData.sourceTime); //for *.res format
    totalReps = QString::number(measData.reps);

    list.insert("Comment",measData.comment);
    list.insert("Made by",measData.madeBy);
    appendMapsList(filenameRT_TD,list);
    appendMapsList(filenameRT_QTD,list);
    appendMapsList(filenamePM_RAW,list);
    appendText(filenameRT_TD,"===============================================================================\n");
    appendText(filenameRT_QTD,"===============================================================================\n");
    appendText(filenamePM_RAW,"===============================================================================\n");
    tmp.clear();
    tmp << "Hour"
        << "So."
        << "Point"
        << "Tag"
        << "Reps";
    appendText(filenameRT_TD,tmp.join("\t"));
    appendText(filenameRT_QTD,tmp.join("\t"));
    appendText(filenamePM_RAW,tmp.join("\t"));
    tmp.clear();
    tmp << "\tA"
        << "B"
        << "C"
        << "S"
        << "AB"
        << "BC"
        << "AC"
        << "T"
        << "D"
        << "LT"
        << "MT\n";
    appendText(filenameRT_TD,tmp.join("\t"));
    tmp.clear();
    tmp << "\tG"
        << "A"
        << "B"
        << "C"
        << "S"
        << "AB"
        << "BC"
        << "AC"
        << "T"
        << "D"
        << "COM"
        << "Q"
        << "GS"
        << "GD"
        << "GT"
        << "LT"
        << "MT\n";
    appendText(filenameRT_QTD,tmp.join("\t"));
    tmp.clear();
    tmp << "\tA"
        << "LT A"
        << "B"
        << "LT B"
        << "C"
        << "LT C"
        << "G"
        << "LT G"
        << "AB"
        << "LT AB"
        << "BC"
        << "LT BC"
        << "AC"
        << "LT AC"
        << "AG"
        << "LT AG"
        << "BG"
        << "LT BG"
        << "CG"
        << "LT CG"
        << "ABC"
        << "LT ABC"
        << "ABG"
        << "LT ABG"
        << "BCG"
        << "LT BCG"
        << "ACG"
        << "LT ACG"
        << "ABCG"
        << "LT ABCG"
        << "MT\n";
    appendText(filenamePM_RAW,tmp.join("\t"));

    currentSource = -1;  //for *.res format
}

void Report::insertRecord(QString hour, int source, int point,QString tagPoint, int reps, QVector<unsigned long> counters)
{
    QStringList tmp;
    tmp << hour
        << QString::number(source)
        << QString::number(point)
        << tagPoint
        << QString::number(reps);
    appendText(filenameRT_TD,tmp.join("\t")+"\t");
    appendText(filenameRT_QTD,tmp.join("\t")+"\t");
    appendText(filenamePM_RAW,tmp.join("\t")+"\t");

    tmp.clear();
    for(int i=TDCR_A;i<GAMMA_G;i++)
        tmp << QString::number(counters.at(i));
    tmp << QString::number(counters.at(MEAS_REAL_TIME))+"\n";
    appendText(filenameRT_TD,tmp.join("\t"));

    tmp.clear();
    for(int i=GAMMA_G;i<PM_A;i++)
        tmp << QString::number(counters.at(i));
    tmp << QString::number(counters.at(MEAS_REAL_TIME))+"\n";
    appendText(filenameRT_QTD,tmp.join("\t"));

    tmp.clear();
    for(int i=PM_A;i<MEAS_REAL_TIME;i++)
        tmp << QString::number(counters.at(i));
    tmp << QString::number(counters.at(MEAS_REAL_TIME))+"\n";
    appendText(filenamePM_RAW,tmp.join("\t"));

    //*.res format
    tmp.clear();
    if(source != currentSource) {
        if(source == 0) {
          currentSource = 0;
          tmp << " 0.00"
              << blankTime
              << totalReps+"\n";
          appendText(filenameRT_RES,tmp.join(" "));
          appendText(filenamePM_RES,tmp.join(" "));
        }else {
            currentSource = source;
            tmp << " "+masses.at(source-1)
                << sourceTime
                << totalReps+"\n";
            appendText(filenameRT_RES,tmp.join(" "));
            appendText(filenamePM_RES,tmp.join(" "));
        }
    }
    tmp.clear();
    //resolving time *.res format
    tmp << QString::number(QTime::fromString(hour,formatTime).msecsSinceStartOfDay()/1000,'f',0);
    for(int i=TDCR_AB;i<GAMMA_G;i++)
        tmp << QString::number(counters.at(i));
    tmp << QString::number(counters.at(MEAS_REAL_TIME)/10,'f',0)+"\n";
    appendText(filenameRT_RES,tmp.join("\t"));

    //pulses-mixing method *.res format
    double Na = (double)(counters.at(PM_A))/((double)(counters.at(PM_LT_A))/1000);
    double Nb = (double)(counters.at(PM_B))/((double)(counters.at(PM_LT_B))/1000);
    double Nc = (double)(counters.at(PM_C))/((double)(counters.at(PM_LT_C))/1000);

    double Sab = (double)(counters.at(PM_AB))/((double)(counters.at(PM_LT_AB))/1000);
    double Sbc = (double)(counters.at(PM_BC))/((double)(counters.at(PM_LT_BC))/1000);
    double Sac = (double)(counters.at(PM_AC))/((double)(counters.at(PM_LT_AC))/1000);

    double Sabc = (double)(counters.at(PM_ABC))/((double)(counters.at(PM_LT_ABC))/1000);

    double Nab = Na + Nb - Sab;
    double Nbc = Nc + Nb - Sbc;
    double Nac = Na + Nc - Sac;
    double Nt = Na + Nb + Nc - Sab - Sbc - Sac + Sabc;
    double Nd = Sab + Sbc + Sac - 2*Sabc;

    tmp.clear();
    tmp << QString::number(QTime::fromString(hour,formatTime).msecsSinceStartOfDay()/1000,'f',0)
        << QString::number(Nab,'f',4)
        << QString::number(Nbc,'f',4)
        << QString::number(Nac,'f',4)
        << QString::number(Nt,'f',4)
        << QString::number(Nd,'f',4)
        << "1000"
        << "100\n";
    appendText(filenamePM_RES,tmp.join("\t"));

}

void Report::debug(QString text)
{
    appendText(filenameDebug,QDateTime::currentDateTime().time().toString(formatTime)+" "+text+"\n");
}

void Report::addCurrentStatusCounters(QVector<unsigned long> counters)
{
    QString text;
    text.append(QDateTime::currentDateTime().time().toString(formatTime)+"\t");
    foreach (unsigned long val, counters) {
       text.append(QString::number(val)+"\t");
    }
    text.append("\n");
    appendText(filenameAnalyzer,text);
}

void Report::addCurrentStatusHV(QVector<float> volt, QVector<float> amp)
{
    QString text;
    text.append(QDateTime::currentDateTime().time().toString(formatTime)+"\t");
    text.append(QString::number(volt[CH_HVANODEBETA])+"\t"+QString::number(amp[CH_HVANODEBETA])+"\t");
    text.append(QString::number(volt[CH_HVFIRSTDYNODEBETA])+"\t"+QString::number(amp[CH_HVFIRSTDYNODEBETA])+"\t");
    text.append(QString::number(volt[CH_HVFOCUSINGDYNODEBETA])+"\t"+QString::number(amp[CH_HVFOCUSINGDYNODEBETA])+"\t");
    text.append(QString::number(volt[CH_HVANODEGAMMA])+"\t"+QString::number(amp[CH_HVANODEGAMMA])+"\n");
    appendText(filenameHV,text);
}

void Report::appendText(QString filename, QString text)
{
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << text;
        file.close();
    }else {
        qDebug() << "Report:appendText() - Cannot open the file: "+filename+", "+file.errorString();
    }
}

void Report::appendMapsList(QString filename, List map)
{
    int maxWidth = 0;
    QList<QString> keys = map.keys;
    QList<QString> values = map.values;
    for(int i=0;i<keys.count();i++) {
        if(keys.at(i).size() > maxWidth)
            maxWidth = keys.at(i).size();
    }
    maxWidth += 2;
    QString text;
    for(int i=0;i<keys.count();i++) {
        text.append(QString("%1|%2").arg(keys.at(i),maxWidth).arg(values.at(i)));
        text.append("\n");
    }
    appendText(filename,text);
}

void Report::appendTable(QString filename, QList<QStringList> table)
{
    QVector<int> fieldWidth;

    for(int i=0;i<table.count();i++) {
        for(int j=0;j<table.at(i).count();j++) {
            int size = table[i][j].size();
            if(j==fieldWidth.count()) {
                fieldWidth << size;
            }else{
                if(fieldWidth[j] < size) {
                    fieldWidth[j] = size;
                }
            }
        }
    }
    QString text;
    for(int i=0;i<table.count();i++) {
        for(int j=0;j<table.at(i).count();j++) {
            if(j==0){
                text.append(QString("%1").arg(table[i][j],fieldWidth.at(j)));
            }else{
                text.append(QString("%1").arg(table[i][j],fieldWidth.at(j)+2));
            }
        }
        text.append("\n");
    }
    text.append("\n");
    appendText(filename,text);

}

void List::insert(QString key, QString value)
{
    keys << key;
    values << value;
}

void List::clear()
{
    keys.clear();
    values.clear();
}
