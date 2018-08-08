#ifndef AUTHORIZATIONDIALOG_H
#define AUTHORIZATIONDIALOG_H

#include <QDialog>
#include <QMouseEvent>

namespace Ui {
class AuthorizationDialog;
}

class AuthorizationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AuthorizationDialog(QWidget *parent = 0);
    ~AuthorizationDialog();
signals:
    void confirmPassword(QString login, QString password);

public slots:
    void confirmedPassword(bool confirmed);

private slots:
    void changedPassword(QString arg);

private:
    Ui::AuthorizationDialog *ui;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;
};

#endif // AUTHORIZATIONDIALOG_H
