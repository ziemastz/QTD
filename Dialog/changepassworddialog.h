#ifndef CHANGEPASSWORDDIALOG_H
#define CHANGEPASSWORDDIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <QMessageBox>

namespace Ui {
class ChangePasswordDialog;
}

class ChangePasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePasswordDialog(QWidget *parent = 0);
    ~ChangePasswordDialog();
    void setLogin(QString login);

signals:
    void confirmPassword(QString login,QString password);
    void changePasswordUser(QString login, QString password);

public slots:
    void confirmedPassword(bool confirmed);

private slots:
    void changedCurrentPassword(QString arg);
    void changedConfirmNewPassword(QString arg);
    void clickedChangePassword();


private:
    Ui::ChangePasswordDialog *ui;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;
    QString loginUser;
};

#endif // CHANGEPASSWORDDIALOG_H
