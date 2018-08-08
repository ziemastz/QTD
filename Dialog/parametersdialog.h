#ifndef PARAMETERSDIALOG_H
#define PARAMETERSDIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <QCompleter>
#include <QTableWidget>
#include <QMessageBox>
#include <QInputDialog>

#include "global.h"

namespace Ui {
class ParametersDialog;
}

class ParametersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParametersDialog(QWidget *parent = 0);
    ~ParametersDialog();

signals:
    void changedName(QString name);
    void selectedName(QString name);
    void remove(QString name);
    void save(PARAMETERSDATA data);

public slots:
    void setSettings(PARAMETERSDIALOGSETTINGS settings);
    void setName(QString name);
    void setData(PARAMETERSDATA data);

private slots:
    void cellChangedTable(int row, int column);
    void toggledEnableLimited();
    void toggledLimitedCount(bool toggled);
    void clickedEdit();
    void finishedEdit();
    void clickedAdd();
    void clickedRemove();
    void clickedSave();

private:
    Ui::ParametersDialog *ui;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;

    void clearTable(QTableWidget *table);
    void addItemTable(QTableWidget *table, QList<float> data);
    void addItemTable(QTableWidget *table, QList<int> data);
    QList<float> readTableF(QTableWidget *table);
    QList<int> readTableI(QTableWidget *table);

};

#endif // PARAMETERSDIALOG_H
