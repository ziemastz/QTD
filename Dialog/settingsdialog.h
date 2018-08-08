#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <Dialog/newuserdialog.h>
#include <Dialog/changepassworddialog.h>
#include <Dialog/authorizationdialog.h>
#include <global.h>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
signals:
    void settings(SETTINGSDIALOGSETTINGS data);
    void testConnection(MODULEDATA data);

    void saveUserData(USERDATA data);
    void newLogin(QString login);
    void availableNewLogin(bool available);
    void removeUserData(QString login);
    void setUsername(QString login);
    void confirmPassword(QString login, QString password);
    void confirmedPassword(bool confirmed);
    void changePasswordUser(QString login, QString password);

public slots:
    void setSettings(SETTINGSDIALOGSETTINGS settings);
    void setSettingsUser(USERDATA data);
    void connectionTested(bool res);

private slots:
    void clickedDone();
    void clickedConnectionTestN1470();
    void clickedConnectionTestBAD1();
    void clickedConnectionTestBAD2();
    void clickedSelectDBLocation();
    void clickedSelectReportLocation();
    void changedLanguage();

    void createUser();
    void userNameChanged(QString arg);
    void userSurnameChanged(QString arg);
    void saveUserClicked();
    void deleteUserClicked();
    void changePasswordClicked();
    void changeAdminPasswordClicked();
    void changedUsername(int index);

private:
    Ui::SettingsDialog *ui;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;

    USERDATA user;
};

#endif // SETTINGSDIALOG_H
