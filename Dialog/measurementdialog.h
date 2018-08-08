#ifndef MEASUREMENTDIALOG_H
#define MEASUREMENTDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QMouseEvent>

#include "global.h"

namespace Ui {
class MeasurementDialog;
}

class MeasurementDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MeasurementDialog(QWidget *parent = 0);
    ~MeasurementDialog();

signals:
    void answerQustionBox(bool answer);
    void startBlankVial();
    void startSourceVial();
    void stopMeasurement();

public slots:
    void clear();
    void set(MEASUREMENTDIALOGSETTINGS settings);
    void setStatusProcess(STATUSPROCESSMEASUREMENTDIALOG status);
    void setStatusParameters(STATUSPARAMETERSMEASUREMENTDIALOG status);
    void setStatusTrack(STATUSTRACKMEASUREMENTDIALOG status);

    void insertBlankVial();
    void insertSourceVial(QString msg);
    void showQustionBox(QString title, QString msg);

    void insertRowTable();
    void setRTTable(QStringList record);
    void setPMTable(QStringList record);

    void showFinishedBox(QString text, QString time);

private slots:
    void clickedStop();
    void clickedMaximizeButton();
    void reject();

protected:
    void keyPressEvent(QKeyEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    Ui::MeasurementDialog *ui;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;

};

#endif // MEASUREMENTDIALOG_H
