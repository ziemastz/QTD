#ifndef NEWMEASUREMENTDIALOG_H
#define NEWMEASUREMENTDIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <QCompleter>
#include "Dialog/seriessourcesdialog.h"
#include "Dialog/parametersdialog.h"

namespace Ui {
class NewMeasurementDialog;
}

class NewMeasurementDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewMeasurementDialog(QWidget *parent = 0);
    ~NewMeasurementDialog();
    SeriesSourcesDialog seriesDialog;
    ParametersDialog paramDialog;

signals:
    void startMeasurement(MEASUREMENTDATA data);
    void changedNuclide(QString nuclide);

public slots:
    void setSettings(NEWMEASUREMENTDIALOGSETTINGS settings);
    void setData(MEASUREMENTDATA data);

private slots:
    void clickedStart();

    //SeriesSourceDialog
    void changedNameSeriesSources(QString name);
    void removeNameSeriesSources(QString name);

    //ParametersDialog
    void changedNameParameters(QString name);
    void removeNameParameters(QString name);

private:
    Ui::NewMeasurementDialog *ui;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;
};

#endif // NEWMEASUREMENTDIALOG_H
