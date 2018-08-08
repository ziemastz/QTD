#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QMouseEvent>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

signals:
    void setLoginUser(QString login, QString password);

public slots:
    void rejectedLoginuser();

private slots:
    void clickedLogin();

private:
    Ui::LoginDialog *ui;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;
};

#endif // LOGINDIALOG_H
