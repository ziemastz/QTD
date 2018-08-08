#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->setModal(true);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setSizeGripEnabled(true);

    ui->settingsTabWidget->setCurrentIndex(0);
    ui->baudRateN1470_comboBox->addItems(QStringList() << "9600" << "19200" << "38400" << "57600" << "115200");
    ui->language_comboBox->blockSignals(true);
    ui->language_comboBox->addItems(QStringList() << "en" << "sk" << "pl");
    ui->language_comboBox->blockSignals(false);

    connect(ui->donePushButton,SIGNAL(clicked(bool)),this,SLOT(clickedDone()));
    connect(ui->cancel_pushButton,SIGNAL(clicked(bool)),this,SLOT(close()));
    connect(ui->connectionTestN1470_pushButton,SIGNAL(clicked(bool)),this,SLOT(clickedConnectionTestN1470()));
    connect(ui->connectionTestBAD1_pushButton,SIGNAL(clicked(bool)),this,SLOT(clickedConnectionTestBAD1()));
    connect(ui->connectionTestBAD2_pushButton,SIGNAL(clicked(bool)),this,SLOT(clickedConnectionTestBAD2()));
    connect(ui->selectDBLocation_pushButton,SIGNAL(clicked(bool)),this,SLOT(clickedSelectDBLocation()));
    connect(ui->selectReportLocation_pushButton,SIGNAL(clicked(bool)),this,SLOT(clickedSelectReportLocation()));
    connect(ui->language_comboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(changedLanguage()));

    //user tab
    connect(ui->createUser_pushButton,SIGNAL(clicked(bool)),this,SLOT(createUser()));
    connect(ui->nameUser_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(userNameChanged(QString)));
    connect(ui->surnameUser_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(userSurnameChanged(QString)));
    connect(ui->saveUser_pushButton,SIGNAL(clicked(bool)),this,SLOT(saveUserClicked()));
    connect(ui->deleteUser_pushButton,SIGNAL(clicked(bool)),this,SLOT(deleteUserClicked()));
    connect(ui->changePassword_pushButton,SIGNAL(clicked(bool)),this,SLOT(changePasswordClicked()));
    connect(ui->changeAdminPassword_pushButton,SIGNAL(clicked(bool)),this,SLOT(changeAdminPasswordClicked()));
    connect(ui->username_comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(changedUsername(int)));

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
void SettingsDialog::mousePressEvent(QMouseEvent *event)
{
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void SettingsDialog::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}
void SettingsDialog::setSettings(SETTINGSDIALOGSETTINGS settings)
{
    ui->companyName_lineEdit->setText(settings.nameCompany);
    ui->counterName_lineEdit->setText(settings.nameCounter);
    ui->language_comboBox->blockSignals(true);
    ui->language_comboBox->setCurrentIndex(ui->language_comboBox->findText(settings.language));
    ui->language_comboBox->blockSignals(false);
    ui->selectedDBLocation_lineEdit->setText(settings.dbLocation);
    ui->selectedReportLocation_lineEdit->setText(settings.reportLocation);

    ui->portN1470_comboBox->clear();
    ui->portN1470_comboBox->addItems(settings.availablePort);

    ui->powerSupplyHV_comboBox->addItems(settings.powerSupplyHVList);
    ui->analyzer_comboBox->addItems(settings.analyzerList);

    ui->powerSupplyHV_comboBox->setCurrentIndex(ui->powerSupplyHV_comboBox->findText(settings.hv.name));
    if(settings.hv.name == "N1470") {
        ui->portN1470_comboBox->setCurrentIndex(ui->portN1470_comboBox->findText(settings.hv.port));
        ui->baudRateN1470_comboBox->setCurrentIndex(ui->baudRateN1470_comboBox->findText(settings.hv.baudRate));
        ui->lBusAddressN1470_spinBox->setValue(settings.hv.lBusAddress);
    }

    ui->analyzer_comboBox->setCurrentIndex(ui->analyzer_comboBox->findText(settings.analyzer.name));
    if(settings.analyzer.name == "BAD1") {
        ui->serialNameBAD1_lineEdit->setText(settings.analyzer.serialNameUSB);
    }else if(settings.analyzer.name == "BAD2") {
        ui->serialNameBAD2_lineEdit->setText(settings.analyzer.serialNameUSB);
    }

    ui->bg_spinBox->setValue(settings.defaultBackgroundTime);
    ui->so_spinBox->setValue(settings.defaultSourceTime);
    ui->rep_spinBox->setValue(settings.defaultReps);
    ui->delayedStart_spinBox->setValue(settings.delayedStart);

    ui->username_comboBox->blockSignals(true);
    ui->username_comboBox->clear();
    ui->username_comboBox->addItems(settings.logins);
    ui->username_comboBox->blockSignals(false);
}

void SettingsDialog::setSettingsUser(USERDATA data)
{
    user.login = data.login;
    user.password = data.password;
    user.name = data.name;
    user.surname = data.surname;
    user.lastModification = data.lastModification;
    ui->username_comboBox->blockSignals(true);
    int index = ui->username_comboBox->findText(user.login);
    if(index == -1)
        ui->username_comboBox->addItem(user.login);
    ui->username_comboBox->setCurrentIndex(ui->username_comboBox->findText(user.login));
    ui->username_comboBox->blockSignals(false);
    ui->nameUser_lineEdit->setText(user.name);
    ui->surnameUser_lineEdit->setText(user.surname);
}

void SettingsDialog::connectionTested(bool res)
{
    if(res) {
        QMessageBox::information(this,"Connection test","Connection test successful",QMessageBox::Ok);
    }else {
        QMessageBox::warning(this,"Connection test","Connection test failed!",QMessageBox::Ok);
    }
}

void SettingsDialog::clickedDone()
{
    SETTINGSDIALOGSETTINGS set;
    set.nameCompany = ui->companyName_lineEdit->text();
    set.nameCounter = ui->counterName_lineEdit->text();
    set.language = ui->language_comboBox->currentText();
    set.dbLocation = ui->selectedDBLocation_lineEdit->text();
    set.reportLocation = ui->selectedReportLocation_lineEdit->text();

    set.hv.name = ui->powerSupplyHV_comboBox->currentText();

    if(set.hv.name == "N1470") {
        if(ui->portN1470_comboBox->currentIndex() != -1) {
            set.hv.port = ui->portN1470_comboBox->currentText();
            set.hv.baudRate = ui->baudRateN1470_comboBox->currentText();
            set.hv.lBusAddress = ui->lBusAddressN1470_spinBox->value();
        }
    }
    set.analyzer.name = ui->analyzer_comboBox->currentText();

    if(set.analyzer.name == "BAD1") {
        set.analyzer.serialNameUSB = ui->serialNameBAD1_lineEdit->text();
    }else if(set.analyzer.name == "BAD2") {
        set.analyzer.serialNameUSB = ui->serialNameBAD2_lineEdit->text();
    }
    set.defaultBackgroundTime = ui->bg_spinBox->value();
    set.defaultSourceTime = ui->so_spinBox->value();
    set.defaultReps = ui->rep_spinBox->value();
    set.delayedStart = ui->delayedStart_spinBox->value();

    if(ui->username_comboBox->currentIndex() == -1) {
        QMessageBox::warning(this, tr("Missing user"), tr("Missing user selected.\nPlease select or create user."), QMessageBox::Ok);
        return;
    }
    emit settings(set);
    this->accept();
}

void SettingsDialog::clickedConnectionTestN1470()
{
    MODULEDATA n1470;
    n1470.name = "N1470";
    n1470.port = ui->portN1470_comboBox->currentText();
    n1470.baudRate = ui->baudRateN1470_comboBox->currentText();
    n1470.lBusAddress = ui->lBusAddressN1470_spinBox->value();

    emit testConnection(n1470);
}

void SettingsDialog::clickedConnectionTestBAD1()
{
    MODULEDATA bad1;
    bad1.name = "BAD1";
    bad1.serialNameUSB = ui->serialNameBAD1_lineEdit->text();

    emit testConnection(bad1);
}

void SettingsDialog::clickedConnectionTestBAD2()
{
    MODULEDATA bad2;
    bad2.name = "BAD2";
    bad2.serialNameUSB = ui->serialNameBAD2_lineEdit->text();

    emit testConnection(bad2);
}

void SettingsDialog::clickedSelectDBLocation()
{
    QString location = QFileDialog::getExistingDirectory(this,tr("Select"),ui->selectedDBLocation_lineEdit->text());
    if(!location.isEmpty())
        ui->selectedDBLocation_lineEdit->setText(location);
}

void SettingsDialog::clickedSelectReportLocation()
{
    QString location = QFileDialog::getExistingDirectory(this,tr("Select"),ui->selectedReportLocation_lineEdit->text());
    if(!location.isEmpty())
        ui->selectedReportLocation_lineEdit->setText(location);
}

void SettingsDialog::changedLanguage()
{
    QMessageBox::information(this,tr("Changed language"),tr("To apply a selected language,\nyou must save the settings and reset the program."),QMessageBox::Ok);
}

void SettingsDialog::createUser()
{
    AuthorizationDialog authorizationDialog;
    connect(&authorizationDialog,SIGNAL(confirmPassword(QString,QString)),this,SIGNAL(confirmPassword(QString,QString)));
    connect(this,SIGNAL(confirmedPassword(bool)),&authorizationDialog,SLOT(confirmedPassword(bool)));
    if(authorizationDialog.exec() == QDialog::Accepted) {
        NewUserDialog newUserDialog;
        connect(&newUserDialog,SIGNAL(newLogin(QString)),this,SIGNAL(newLogin(QString)));
        connect(this,SIGNAL(availableNewLogin(bool)),&newUserDialog,SLOT(availableNewLogin(bool)));
        connect(&newUserDialog,SIGNAL(saveUserData(USERDATA)),this,SIGNAL(saveUserData(USERDATA)));

        newUserDialog.exec();
    }

}

void SettingsDialog::userNameChanged(QString arg)
{
    if((arg != user.name) || (ui->surnameUser_lineEdit->text() != user.surname)) {
        ui->saveUser_pushButton->setEnabled(true);
    }else {
        ui->saveUser_pushButton->setEnabled(false);
    }
    if(ui->nameUser_lineEdit->text().isEmpty() && ui->surnameUser_lineEdit->text().isEmpty())
        ui->saveUser_pushButton->setEnabled(false);
}

void SettingsDialog::userSurnameChanged(QString arg)
{
    if((arg != user.surname) || (ui->nameUser_lineEdit->text() != user.name)) {
        ui->saveUser_pushButton->setEnabled(true);
    }else {
        ui->saveUser_pushButton->setEnabled(false);
    }
    if(ui->nameUser_lineEdit->text().isEmpty() && ui->surnameUser_lineEdit->text().isEmpty())
        ui->saveUser_pushButton->setEnabled(false);
}

void SettingsDialog::saveUserClicked()
{
    user.name = ui->nameUser_lineEdit->text();
    user.surname = ui->surnameUser_lineEdit->text();
    emit saveUserData(user);
    ui->saveUser_pushButton->setEnabled(false);
}

void SettingsDialog::deleteUserClicked()
{
    AuthorizationDialog authorizationDialog;
    connect(&authorizationDialog,SIGNAL(confirmPassword(QString,QString)),this,SIGNAL(confirmPassword(QString,QString)));
    connect(this,SIGNAL(confirmedPassword(bool)),&authorizationDialog,SLOT(confirmedPassword(bool)));
    if(authorizationDialog.exec() == QDialog::Accepted) {
        emit removeUserData(user.login);
        ui->username_comboBox->blockSignals(true);
        ui->username_comboBox->removeItem(ui->username_comboBox->currentIndex());
        ui->username_comboBox->setCurrentIndex(-1);
        ui->username_comboBox->blockSignals(false);
        ui->nameUser_lineEdit->clear();
        ui->surnameUser_lineEdit->clear();
    }
}

void SettingsDialog::changePasswordClicked()
{
    ChangePasswordDialog changeDialog;
    changeDialog.setLogin(user.login);
    connect(&changeDialog,SIGNAL(confirmPassword(QString,QString)),this,SIGNAL(confirmPassword(QString,QString)));
    connect(this,SIGNAL(confirmedPassword(bool)),&changeDialog,SLOT(confirmedPassword(bool)));
    connect(&changeDialog,SIGNAL(changePasswordUser(QString,QString)),this,SIGNAL(changePasswordUser(QString,QString)));

    changeDialog.exec();
}

void SettingsDialog::changeAdminPasswordClicked()
{
    ChangePasswordDialog changeDialog;
    changeDialog.setLogin("admin");
    connect(&changeDialog,SIGNAL(confirmPassword(QString,QString)),this,SIGNAL(confirmPassword(QString,QString)));
    connect(this,SIGNAL(confirmedPassword(bool)),&changeDialog,SLOT(confirmedPassword(bool)));
    connect(&changeDialog,SIGNAL(changePasswordUser(QString,QString)),this,SIGNAL(changePasswordUser(QString,QString)));

    changeDialog.exec();
}

void SettingsDialog::changedUsername(int index)
{
    AuthorizationDialog authorizationDialog;
    connect(&authorizationDialog,SIGNAL(confirmPassword(QString,QString)),this,SIGNAL(confirmPassword(QString,QString)));
    connect(this,SIGNAL(confirmedPassword(bool)),&authorizationDialog,SLOT(confirmedPassword(bool)));
    if(authorizationDialog.exec() == QDialog::Accepted) {
        ui->username_comboBox->blockSignals(true);
        ui->username_comboBox->setCurrentIndex(index);
        ui->username_comboBox->blockSignals(false);
        emit setUsername(ui->username_comboBox->currentText());
    }else {
        ui->username_comboBox->blockSignals(true);
        ui->username_comboBox->setCurrentIndex(ui->username_comboBox->findText(user.login));
        ui->username_comboBox->blockSignals(false);
    }
}
