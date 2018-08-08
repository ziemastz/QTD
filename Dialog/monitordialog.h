#ifndef MONITORDIALOG_H
#define MONITORDIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPoint>
#include <QMenu>
#include <QTableWidget>
#include "global.h"

namespace Ui {
class MonitorDialog;
}

class MonitorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MonitorDialog(QWidget *parent = 0);
    ~MonitorDialog();

signals:
    void stop();
    void startMeasMonitor(int time);
    void stopMeasMonitor();
    void resetMeasMonitor();
    void setHV(float value, int ch);
    void setAnalyzer(int value, int ch);
    void saveToFileMonitorData(MONITORDIALOGDATA data);
public slots:
    void setStatusParameters(STATUSPARAMETERSMEASUREMENTDIALOG status);
    void setStatusTrack(STATUSTRACKMEASUREMENTDIALOG status);
    void insertRowTable();
    void setRTTable(QStringList record);
    void setPMTable(QStringList record);
    void clear();

    void stopped();

private slots:
    void showContextMenu(const QPoint &pos);
    void anodeBetaChanged(double value);
    void firstDynodeChanged(double value);
    void focusingDynodeChanged(double value);
    void anodeGammaChanged(double value);

    void resolvingTimeBetaChanged(int value);
    void deadTimeBetaChanged(int value);
    void resolvingTimeGammaChanged(int value);
    void deadTimeGammaChanged(int value);
    void delayTimeBetaChanged(int value);

    void clickedStart();
    void clickedStop();
    void clickedReset();


    void clickedClose();
    void clickedMaximize();
    void reject();

    void clickedSaveToFile();

protected:
    void keyPressEvent(QKeyEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    Ui::MonitorDialog *ui;

    QList<QStringList> recordsRT, recordsPM;
    int time;
    bool isMeasurement;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;
};

#endif // MONITORDIALOG_H
