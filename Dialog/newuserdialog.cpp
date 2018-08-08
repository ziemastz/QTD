#include "newuserdialog.h"
#include "ui_newuserdialog.h"

NewUserDialog::NewUserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewUserDialog)
{
    ui->setupUi(this);
    this->setModal(true);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    connect(ui->login_lineEdit,SIGNAL(textChanged(QString)),this,SIGNAL(newLogin(QString)));
    connect(ui->confirmPassword_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(confirmPassword(QString)));
    connect(ui->add_pushButton,SIGNAL(clicked(bool)),this,SLOT(clickedAdd()));
    connect(ui->cancel_pushButton,SIGNAL(clicked(bool)),this,SLOT(close()));

}

NewUserDialog::~NewUserDialog()
{
    delete ui;
}
void NewUserDialog::mousePressEvent(QMouseEvent *event)
{
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void NewUserDialog::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}
void NewUserDialog::availableNewLogin(bool available)
{
    if(available) {
        ui->errorLogin_label->clear();
    }else {
        ui->errorLogin_label->setText("Unavailable username!");
    }
}

void NewUserDialog::confirmPassword(QString confirm)
{
    if(ui->password_lineEdit->text() == confirm) {
        ui->errorPassword_label->clear();
    }else {
        ui->errorPassword_label->setText("Password didn't match!");
    }
}

void NewUserDialog::clickedAdd()
{
    if(!ui->errorLogin_label->text().isEmpty() || !ui->errorPassword_label->text().isEmpty()) {
        QMessageBox::warning(this,"Error","Sorry, please check entered username or password!",QMessageBox::Ok);
        return;
    }
    USERDATA user;
    user.login = ui->login_lineEdit->text();
    user.password = ui->password_lineEdit->text();
    user.name = ui->name_lineEdit->text();
    user.surname = ui->surname_lineEdit->text();

    emit saveUserData(user);
    this->accept();
}
