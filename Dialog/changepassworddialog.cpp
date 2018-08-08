#include "changepassworddialog.h"
#include "ui_changepassworddialog.h"

ChangePasswordDialog::ChangePasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePasswordDialog)
{
    ui->setupUi(this);
    this->setModal(true);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    connect(ui->currentPassword_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(changedCurrentPassword(QString)));
    connect(ui->confirmNewPassword_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(changedConfirmNewPassword(QString)));
    connect(ui->cancel_pushButton,SIGNAL(clicked(bool)),this,SLOT(close()));
    connect(ui->changePassword_pushButton,SIGNAL(clicked(bool)),this,SLOT(clickedChangePassword()));
}

ChangePasswordDialog::~ChangePasswordDialog()
{
    delete ui;
}
void ChangePasswordDialog::mousePressEvent(QMouseEvent *event)
{
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void ChangePasswordDialog::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}
void ChangePasswordDialog::setLogin(QString login)
{
    loginUser = login;
}

void ChangePasswordDialog::confirmedPassword(bool confirmed)
{
    if(confirmed) {
        ui->errorCurrentpassword_label->clear();
    }else {
        ui->errorCurrentpassword_label->setText("Incorrect password!");
    }
}

void ChangePasswordDialog::changedCurrentPassword(QString arg)
{
    emit confirmPassword(loginUser,arg);
}

void ChangePasswordDialog::changedConfirmNewPassword(QString arg)
{
    if(ui->newPassword_lineEdit->text() != arg) {
        ui->errorConfirmPassword_label->setText("Password didn't match!");
    }else {
        ui->errorConfirmPassword_label->clear();
    }
}

void ChangePasswordDialog::clickedChangePassword()
{
    if(!ui->errorConfirmPassword_label->text().isEmpty() || !ui->errorCurrentpassword_label->text().isEmpty()) {
        QMessageBox::warning(this,"Error","Sorry, please check entered current or new password!",QMessageBox::Ok);
        return;
    }
    emit changePasswordUser(loginUser,ui->newPassword_lineEdit->text());
    this->accept();
}
