#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    this->setModal(true);
    this->setWindowFlags(Qt::FramelessWindowHint);

    ui->wrong_label->clear();
    connect(ui->cancel_pushButton,SIGNAL(clicked(bool)),this,SLOT(reject()));
    connect(ui->login_pushButton,SIGNAL(clicked(bool)),this,SLOT(clickedLogin()));

 }

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::rejectedLoginuser()
{
    ui->wrong_label->setText(tr("Incorrect username or password"));
}

void LoginDialog::clickedLogin()
{
    ui->wrong_label->clear();
    emit setLoginUser(ui->login_lineEdit->text(),ui->password_lineEdit->text());
}
void LoginDialog::mousePressEvent(QMouseEvent *event)
{
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void LoginDialog::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}
