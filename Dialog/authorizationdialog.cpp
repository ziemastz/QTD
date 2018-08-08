#include "authorizationdialog.h"
#include "ui_authorizationdialog.h"

AuthorizationDialog::AuthorizationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AuthorizationDialog)
{
    ui->setupUi(this);
    this->setModal(true);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    connect(ui->adminPassword_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(changedPassword(QString)));
    connect(ui->verified_pushButton,SIGNAL(clicked(bool)),this,SLOT(accept()));
}

AuthorizationDialog::~AuthorizationDialog()
{
    delete ui;
}
void AuthorizationDialog::mousePressEvent(QMouseEvent *event)
{
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void AuthorizationDialog::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}
void AuthorizationDialog::confirmedPassword(bool confirmed)
{
    if(confirmed) {
        ui->verified_pushButton->setEnabled(true);
    }else {
        ui->verified_pushButton->setEnabled(false);
    }
}

void AuthorizationDialog::changedPassword(QString arg)
{
    emit confirmPassword("admin",arg);
}
