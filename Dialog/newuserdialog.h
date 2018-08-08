#ifndef NEWUSERDIALOG_H
#define NEWUSERDIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <QMessageBox>

#include <global.h>

namespace Ui {
class NewUserDialog;
}

class NewUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewUserDialog(QWidget *parent = 0);
    ~NewUserDialog();

signals:
    void newLogin(QString login);
    void saveUserData(USERDATA data);

public slots:
    void availableNewLogin(bool available);
private slots:
    void confirmPassword(QString confirm);
    void clickedAdd();

private:
    Ui::NewUserDialog *ui;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;
};

#endif // NEWUSERDIALOG_H
