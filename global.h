#ifndef GLOBAL_H
#define GLOBAL_H
#include <QString>
#include <QPoint>
#include <QSize>
#include <QStringList>
#include <QDateTime>
#include <QObject>


//User table
#define U_LOGIN 0
#define U_PASSWORD 1
#define U_NAME 2
#define U_SURNAME 3
#define U_LASTMODIFICATION 4

//Source table
#define S_NAME 0
#define S_NUCLIDE 1
#define S_SOLUTION 2
#define S_DILUTION 3
#define S_MOTHERSOLUTION 4
#define S_SCINTNAME 5
#define S_SCINTVOLUME 6
#define S_VIALTYPE 7
#define S_IDVIALS 8
#define S_MASSES 9
#define S_PREPAREDBY 10
#define S_PREPAREDAT 11
#define S_LASTMODIFICATION 12

//Parameter table
#define P_NAME 0
#define P_NUCLIDE 1
#define P_HVA1 2
#define P_HVD1 3
#define P_HVF 4
#define P_HVA2 5
#define P_R1 6
#define P_DT1 7
#define P_R2 8
#define P_DT2 9
#define P_DLB 10
#define P_GWLOWER 11
#define P_GWUPPER 12
#define P_LCCHANNEL 13
#define P_LCCOUNTS 14
#define P_LCUNCERTAINTY 15
#define P_TIME 16
#define P_LASTMODIFICATION 17

//Mesurement table
#define M_ID 0
#define M_NUCLIDE 1
#define M_FILENAME 2
#define M_MODE 3
#define M_DATE 4
#define M_SERIESSOURCES 5
#define M_PARAMETERS 6
#define M_BACKGROUNDTIME 7
#define M_SOURCETIME 8
#define M_REPS 9
#define M_COMMENT 10
#define M_MADEBY 11
#define M_STATUS 12
#define M_LASTMODIFICATION 13

//counters
#define TDCR_A 0
#define TDCR_B 1
#define TDCR_C 2
#define TDCR_S 3
#define TDCR_AB 4
#define TDCR_BC 5
#define TDCR_AC 6
#define TDCR_T 7
#define TDCR_D 8
#define TDCR_LT 9
#define GAMMA_G 10
#define GAMMA_A 11
#define GAMMA_B 12
#define GAMMA_C 13
#define GAMMA_S 14
#define GAMMA_AB 15
#define GAMMA_BC 16
#define GAMMA_AC 17
#define GAMMA_T 18
#define GAMMA_D 19
#define GAMMA_COM 20
#define GAMMA_Q 21
#define GAMMA_GS 22
#define GAMMA_GD 23
#define GAMMA_GT 24
#define GAMMA_LT 25
#define PM_A 26
#define PM_LT_A 27
#define PM_B 28
#define PM_LT_B 29
#define PM_C 30
#define PM_LT_C 31
#define PM_G 32
#define PM_LT_G 33
#define PM_AB 34
#define PM_LT_AB 35
#define PM_BC 36
#define PM_LT_BC 37
#define PM_AC 38
#define PM_LT_AC 39
#define PM_AG 40
#define PM_LT_AG 41
#define PM_BG 42
#define PM_LT_BG 43
#define PM_CG 44
#define PM_LT_CG 45
#define PM_ABC 46
#define PM_LT_ABC 47
#define PM_ABG 48
#define PM_LT_ABG 49
#define PM_BCG 50
#define PM_LT_BCG 51
#define PM_ACG 52
#define PM_LT_ACG 53
#define PM_ABCG 54
#define PM_LT_ABCG 55
#define MEAS_REAL_TIME 56

//MODULE
#define HV_MODULE 0
#define ANALYZER_MODULE 1

//Module channels hv
#define CH_HVANODEBETA 0
#define CH_HVFIRSTDYNODEBETA 1
#define CH_HVFOCUSINGDYNODEBETA 2
#define CH_HVANODEGAMMA 3
//Module channels analyzer (bad1)
#define CH_RESOLVINGTIMEBETA 0
#define CH_RESOLVINGTIMEGAMMA 1
#define CH_DEADTIMEBETA 2
#define CH_DEADTIMEGAMMA 3
#define CH_DELAYTIMEBETA 4
//COMMAND
#define C_RESET 0
#define C_START 1
#define C_STOP 2
#define C_GETDATA 3
#define C_ENAEXTCLK 4
#define C_DISEXTCLK 5

//Mode
#define MODE_STANDARD 0
#define MODE_CONTROL 1
#define MODE_CHARACT 2
#define MODE_TEST 3
#define MODE_OTHERS 4
//Status
#define STATUS_ADDED 0
#define STATUS_CANCELED 1
#define STATUS_FINISHED 2
#define STATUS_DELETED 3


const int freqClk = 6000;
const QString formatDate = "yyyy-MM-dd";
const QString formatTime = "hh:mm:ss";

const QStringList modeMeasurement = QStringList() << QT_TRANSLATE_NOOP("MainWindow","Standardization") << QT_TRANSLATE_NOOP("MainWindow","Control") << QT_TRANSLATE_NOOP("MainWindow","Characteristic") << QT_TRANSLATE_NOOP("MainWindow","Test") << QT_TRANSLATE_NOOP("MainWindow","Others");

const QStringList statusMeasurement = QStringList() << QT_TRANSLATE_NOOP("MainWindow","Added") << QT_TRANSLATE_NOOP("MainWindow","Canceled") << QT_TRANSLATE_NOOP("MainWindow","Finished") << QT_TRANSLATE_NOOP("MainWindow","Deleted");


//  MainWindow.ui
struct DISPLAYDATETIME {
    QString currentTime;
    QString currentDate;
    QString timeZone;
};
struct DISPLAYSTATISTICS {
    QString measurementCount;
    QString solutionCount;
    QString nuclideCount;
    QString totalTimeCount;
};
struct MAINWINDOWSETTINGS {
    QString title;
    QPoint position;
    QSize size;
    bool maximized;

    QStringList headersLastStandardizationTable;
    QStringList headersMeasurementRegisterTable;
    QStringList completerMeasurementSearch;
};
struct MODULEDATA {
    QString name;
    QString port;
    QString baudRate;
    int lBusAddress;
    QString serialNameUSB;
    QString numberUSB;
};

//  SettingsDialog
struct USERDATA {
    QString login;
    QString password;
    QString name;
    QString surname;
    QString lastModification;
};
struct SETTINGSDIALOGSETTINGS {
    QString nameCompany;
    QString nameCounter;
    QString language;
    QString dbLocation;
    QString reportLocation;
    QStringList availablePort;
    const QStringList powerSupplyHVList = QStringList() << "Disable" << "N1470";
    const QStringList analyzerList = QStringList() << "BAD1" << "BAD2";
    MODULEDATA hv;
    MODULEDATA analyzer;
    int defaultBackgroundTime;
    int defaultSourceTime;
    int defaultReps;
    int delayedStart;
    QStringList logins;
};

//  NewMeasurementDialog
struct NEWMEASUREMENTDIALOGSETTINGS {
    QString company;
    QString counter;
    QStringList modeList;
    QStringList nameSeriesSourcesList;
    QStringList nameParametersList;
    QStringList completerNuclideList;
    QStringList completerUserList;
    int defaultBackgroundTime;
    int defaultSourceTime;
    int defaultReps;
    QString user;
};

struct MEASUREMENTDATA {
    QString id;
    QString company;
    QString counter;
    QString nuclide;
    QString filename;
    int mode;
    QString date;
    QString nameSeriesSources;
    QString nameParameters;
    int backgroundTime;
    int sourceTime;
    int reps;
    QString comment;
    QString madeBy;
    int status;
    QString lastModification;
};

//  SeriesSourcesDialog
struct SERIESSOURCESDIALOGSETTINGS {
    QStringList nameSeriesSourcesList;
    QStringList completerNuclideList;
    QStringList completerUserList;
    QStringList completerSolutionList;
    QStringList completerNameScintillatorList;
    QStringList completerVolumeScintillatorList;
    QStringList completerVialTypeList;
};
struct SERIESSOURCESDATA {
    QString name;
    QString nuclide;
    QString solution;
    double dilution;
    QString motherSolution;
    QString nameScintillator;
    QString volumeScintillator;
    QString vialType;
    QStringList idVials;
    QList<double> masses;
    QString preparedBy;
    QDate preparedAt;
    QString lastModification;
};

//  ParametersDialog
struct PARAMETERSDIALOGSETTINGS {
    QStringList nameParametersList;
    QStringList completerNuclideList;
    QStringList channelLimitedList;
};

enum TIMELIMITED {
    REAL_TIME,
    LIVETIME_BETA,
    LIVETIME_GAMMA
};
enum CHANNELLIMITED {
    D_BETA,
    T_BETA,
    S_BETA,
    TD_BETA,
    G_GAMMA,
    COM_GAMMA,
    D_GAMMA,
    T_GAMMA,
    S_GAMMA,
    GD_GAMMA,
    GT_GAMMA,
    AD_GAMMA,   // is not channel - AD = G*D / GD - for uncertainty limit
    AT_GAMMA    // is not channel - AT = G*T / GT - for uncertainty limit
};
const QStringList channelLimetedList = QStringList()
        << "D (beta)"
        << "T (beta)"
        << "S (beta)"
        << "T/D (beta)-only uncer."
        << "G (gamma)"
        << "COM (gamma)"
        << "D' (gamma)"
        << "T' (gamma)"
        << "S' (gamma)"
        << "GD (gamma)"
        << "GT (gamma)"
        << "A(D) (gamma)-only uncer."
        << "A(T) (gamma)-only uncer.";
struct PARAMETERSDATA {
    QString name;
    QString nuclide;
    QList<float> hvAnodeBeta, hvFirstDynodeBeta, hvFocalysingDynodeBeta, hvAnodeGamma;
    QList<int> resolvintTimeBeta, deadTimeBeta, delayTimeBeta, resolvingTimeGamma, deadTimeGamma;
    QStringList tagPoints;
    int upperWindowGamma, lowerWindowGamma;
    TIMELIMITED timeLimited;
    CHANNELLIMITED channelLimited;
    int valueCountLimited;
    double valueUncertaintlyLimited;
    QString lastModification;
};

//  MeasurementDialog
struct MEASUREMENTDIALOGSETTINGS {
    QString nuclide;
    QString filename;
    QString solution;
    int mode;
    QString nameSeriesSources;
    QString nameParameters;
    QString blankTime;
    QString sourceTime;
    QString reps;

    QStringList headersRecordsTableRT;
    QStringList headersRecordsTablePM;
};
struct STATUSPARAMETERSMEASUREMENTDIALOG {
    QString statusAnode;
    QString statusFirstDynode;
    QString statusFocusingDynode;
    QString statusGamma;
    double voltAnode;
    double voltFirstDynode;
    double voltFocusingDynode;
    double voltGamma;
    double ampAnode;
    double ampFirstDynode;
    double ampFocusingDynode;
    double ampGamma;

    QString statusAnalyzer;
    QString resolvingTimeBeta;
    QString deadTimeBeta;
    QString delayTimeBeta;
    QString resolvingTimeGamma;
    QString deadTimeGamma;
};
struct STATUSTRACKMEASUREMENTDIALOG {
    double cpsA;
    double cpsB;
    double cpsC;
    double cpsG;

    int countsA;
    int countsB;
    int countsC;
    int countsG;

    int deatTimeA;
    int deatTimeB;
    int deatTimeC;
    int deatTimeG;

    int efficiencyTDCR;
    int efficiencyBeta4LS;
    int efficiencyGamma4LS;
};
struct STATUSPROCESSMEASUREMENTDIALOG {
    QDateTime beginMeasurement;
    QDateTime endMeasurement;
    int sourceCount;
    int pointCount;
    int reps;
    int time;
    int currentTime;
    int currentReps;
    int currentPoint;
    int currentSource;
    QString timeLeft;
};

// Monitor
struct MONITORDIALOGDATA {
    QString nuclide;
    QString vial;
    QString comment;
    QList<QStringList> recordsRT, recordsPM;
};

//  ReportDialog

struct REPORTDIALOGSETTINGS {
    MEASUREMENTDATA measurement;
    QString reportText;
};

//database
const QStringList userHeaders = QStringList()
        << "Login"
        << "Password"
        << "Name"
        << "Surname"
        << "Last modification";
const QStringList sourceHeaders = QStringList()
        << "Name"
        << "Nuclide"
        << "Solution"
        << "Dilution"
        << "Mother solut."
        << "Scint. name"
        << "Scint. volume"
        << "Vial type"
        << "ID vials"
        << "Masses"
        << "Prepared by"
        << "Prepared at"
        << "Last modification";

const QStringList parameterHeaders = QStringList()
        << "Name"
        << "Nuclide"
        << "HV A1"
        << "HV D1"
        << "HV F"
        << "HV A2"
        << "R1"
        << "DT1"
        << "R2"
        << "DT2"
        << "DLB"
        << "GW-Lower"
        << "GW-Upper"
        << "LC-Channel"
        << "LC-Counts"
        << "LC-Uncertainty"
        << "Time"
        << "Last modification";

const QStringList measurementHeaders = QStringList()
        << QT_TRANSLATE_NOOP("MainWindow","ID")
        << QT_TRANSLATE_NOOP("MainWindow","Nuclide")
        << QT_TRANSLATE_NOOP("MainWindow","Filename")
        << QT_TRANSLATE_NOOP("MainWindow","Mode")
        << QT_TRANSLATE_NOOP("MainWindow","Date")
        << QT_TRANSLATE_NOOP("MainWindow","ID sources series")
        << QT_TRANSLATE_NOOP("MainWindow","ID parameters")
        << QT_TRANSLATE_NOOP("MainWindow","BG(s)")
        << QT_TRANSLATE_NOOP("MainWindow","SO(s)")
        << QT_TRANSLATE_NOOP("MainWindow","Reps")
        << QT_TRANSLATE_NOOP("MainWindow","Comment")
        << QT_TRANSLATE_NOOP("MainWindow","Made by")
        << QT_TRANSLATE_NOOP("MainWindow","Status")
        << QT_TRANSLATE_NOOP("MainWindow","Last modification");
const QStringList lastStandardizationHeaders = QStringList()
        << QT_TRANSLATE_NOOP("MainWindow","Nuclide")
        << QT_TRANSLATE_NOOP("MainWindow","Solution")
        << QT_TRANSLATE_NOOP("MainWindow","Date");

const QStringList recordMeasurementHeadersRT = QStringList()
        << QT_TRANSLATE_NOOP("MeasurementDialog","Hour")
        << QT_TRANSLATE_NOOP("MeasurementDialog","Vial")
        << QT_TRANSLATE_NOOP("MeasurementDialog","Point")
        << QT_TRANSLATE_NOOP("MeasurementDialog","Rep")
        << "-"
        << "AB(cps)"
        << "BC(cps)"
        << "AC(cps)"
        << "T(cps)"
        << "D(cps)"
        << "TDCR"
        << "D-T(%)"
        << "-"
        << "G(cps)"
        << "T'(cps)"
        << "D'(cps)"
        << "GT'(cps)"
        << "GD'(cps)"
        << "eff.D"
        << "eff.T"
        << "eff.G"
        << "D-T(%)"
        << "-"
        << "Rel.1"
        << "Rel.2"
        << "Rel.3"
        << "Rel.4"
        << "Rel.5"
        << "Rel.6"
        << "-"
        << "A"
        << "B"
        << "C"
        << "S"
        << "AB"
        << "BC"
        << "AC"
        << "T"
        << "D"
        << "LT-Beta"
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
        << "LT-Gamma"
        << "Real time";
const QStringList recordMeasurementHeadersPM = QStringList()
        << QT_TRANSLATE_NOOP("MeasurementDialog","Hour")
        << QT_TRANSLATE_NOOP("MeasurementDialog","Vial")
        << QT_TRANSLATE_NOOP("MeasurementDialog","Point")
        << QT_TRANSLATE_NOOP("MeasurementDialog","Rep")
        << "-"
        << "AB(cps)"
        << "BC(cps)"
        << "AC(cps)"
        << "T(cps)"
        << "D(cps)"
        << "TDCR"
        << "D-T(%)"
        << "-"
        << "G(cps)"
        << "T'(cps)"
        << "D'(cps)"
        << "GT'(cps)"
        << "GD'(cps)"
        << "eff.D"
        << "eff.T"
        << "eff.G"
        << "D-T(%)"
        << "-"
        << "Rel.1"
        << "Rel.2"
        << "Rel.3"
        << "Rel.4"
        << "Rel.5"
        << "Rel.6"
        << "-"
        << "PM-A"
        << "PM-LT-A"
        << "PM-B"
        << "PM-LT-B"
        << "PM-C"
        << "PM-LT-C"
        << "PM-G"
        << "PM-LT-G"
        << "PM-AB"
        << "PM-LT-AB"
        << "PM-BC"
        << "PM-LT-BC"
        << "PM-AC"
        << "PM-LT-AC"
        << "PM-AG"
        << "PM-LT-AG"
        << "PM-BG"
        << "PM-LT-BG"
        << "PM-CG"
        << "PM-LT-CG"
        << "PM-ABC"
        << "PM-LT-ABC"
        << "PM-ABG"
        << "PM-LT-ABG"
        << "PM-BCG"
        << "PM-LT-BCG"
        << "PM-ACG"
        << "PM-LT-ACG"
        << "PM-ABCG"
        << "PM-LT-ABCG"
        << "Real time";
const QStringList recordMonitorHeadersRT = QStringList()
        << "AB(cps)"
        << "BC(cps)"
        << "AC(cps)"
        << "T(cps)"
        << "D(cps)"
        << "TDCR"
        << "DT"
        << "-"
        << "G(cps)"
        << "T'(cps)"
        << "D'(cps)"
        << "GT'(cps)"
        << "GD'(cps)"
        << "eff.D"
        << "eff.T"
        << "eff.G"
        << "DT"
        << "-"
        << "Rel.1"
        << "Rel.2"
        << "Rel.3"
        << "Rel.4"
        << "Rel.5"
        << "Rel.6"
        << "-"
        << "A"
        << "B"
        << "C"
        << "S"
        << "AB"
        << "BC"
        << "AC"
        << "T"
        << "D"
        << "LT-Beta"
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
        << "LT-Gamma"
        << "Real time";
const QStringList recordMonitorHeadersPM = QStringList()
        << "AB(cps)"
        << "BC(cps)"
        << "AC(cps)"
        << "T(cps)"
        << "D(cps)"
        << "TDCR"
        << "DT"
        << "-"
        << "G(cps)"
        << "T'(cps)"
        << "D'(cps)"
        << "GT'(cps)"
        << "GD'(cps)"
        << "eff.D"
        << "eff.T"
        << "eff.G"
        << "DT"
        << "-"
        << "Rel.1"
        << "Rel.2"
        << "Rel.3"
        << "Rel.4"
        << "Rel.5"
        << "Rel.6"
        << "-"
        << "PM-A"
        << "PM-LT-A"
        << "PM-B"
        << "PM-LT-B"
        << "PM-C"
        << "PM-LT-C"
        << "PM-G"
        << "PM-LT-G"
        << "PM-AB"
        << "PM-LT-AB"
        << "PM-BC"
        << "PM-LT-BC"
        << "PM-AC"
        << "PM-LT-AC"
        << "PM-AG"
        << "PM-LT-AG"
        << "PM-BG"
        << "PM-LT-BG"
        << "PM-CG"
        << "PM-LT-CG"
        << "PM-ABC"
        << "PM-LT-ABC"
        << "PM-ABG"
        << "PM-LT-ABG"
        << "PM-BCG"
        << "PM-LT-BCG"
        << "PM-ACG"
        << "PM-LT-ACG"
        << "PM-ABCG"
        << "PM-LT-ABCG"
        << "Real time";
#endif // GLOBAL_H
