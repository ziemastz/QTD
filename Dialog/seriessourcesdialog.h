#ifndef SERIESSOURCESDIALOG_H
#define SERIESSOURCESDIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <QCompleter>
#include <QInputDialog>
#include <QMessageBox>
#include <QLocale>
#include <global.h>

namespace Ui {
class SeriesSourcesDialog;
}

class SeriesSourcesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SeriesSourcesDialog(QWidget *parent = 0);
    ~SeriesSourcesDialog();

signals:
    void changedName(QString name);
    void selectedName(QString name);
    void remove(QString name);
    void save(SERIESSOURCESDATA data);

public slots:
    void setSettings(SERIESSOURCESDIALOGSETTINGS settings);
    void setName(QString name);
    void setData(SERIESSOURCESDATA data);

private slots:
    void cellChanged(int row, int column);
    void clickedEdit();
    void finishedEdit();
    void clickedAdd();
    void clickedRemove();
    void clickedSave();

private:
    Ui::SeriesSourcesDialog *ui;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;
};

#endif // SERIESSOURCESDIALOG_H
