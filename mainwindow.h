#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QMenu>
#include <QCompleter>
#include <QMouseEvent>
#include <QFont>
#include <QRgb>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QStatusBar>

#include "global.h"
#include "Dialog/settingsdialog.h"
#include "Dialog/newmeasurementdialog.h"
#include "Dialog/measurementdialog.h"
#include "Dialog/monitordialog.h"

namespace Ui {
class MainWindow;
}
class TableWidgetNumberItem : public QTableWidgetItem {
    public:
        TableWidgetNumberItem(const QString txt = QString("0"))
            :QTableWidgetItem(txt)
        {
        }
        bool operator <(const QTableWidgetItem &other) const
        {
            return text().toInt() < other.text().toInt();
        }
};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    SettingsDialog settingsDialog;
    NewMeasurementDialog newMeasDialog;
    MeasurementDialog measDialog;
    MonitorDialog monDialog;

signals:
    void searchMeasurement(QString arg);
    void positionWindow(QPoint p);
    void sizeWindow(QSize s);
    void maximizedWindow(bool m);

    void repeatMeasurement(QString id);
    void deleteReport(QStringList ids);
    void openReportLocation(QString id);

    void clickedMonitorButton();

public slots:
    void setSettingsMainWindow(MAINWINDOWSETTINGS settings);
    void setDisplayDateTime(DISPLAYDATETIME dateTime);
    void setDisplayMeasurementStatistics(DISPLAYSTATISTICS statistics);
    void setMeasurementRegisterTable(QList<QStringList> records);
    void setLastStandardizationTable(QList<QStringList> records);
    //NewMeasurementDialog
    void showNewMeasurementDialog(MEASUREMENTDATA data);

    //MeasurementDialog
    void showMeasurementDialog(MEASUREMENTDIALOGSETTINGS settings);
private slots:
    void showContextMenu(const QPoint &pos);
    void selectionChangedLastStandaTableWidget();
    void clickedHideButton();
    void clickedMaximizeButton();

protected:
    void changeEvent(QEvent *event);

private:
    Ui::MainWindow *ui;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;

};

#endif // MAINWINDOW_H
