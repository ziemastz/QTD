#include "measurementdialog.h"
#include "ui_measurementdialog.h"

MeasurementDialog::MeasurementDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasurementDialog)
{
    ui->setupUi(this);
    this->setModal(true);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setSizeGripEnabled(true);

    connect(ui->stopPushButton,SIGNAL(clicked(bool)),this,SLOT(clickedStop()));
    connect(ui->maximizePushButton,SIGNAL(clicked(bool)),this,SLOT(clickedMaximizeButton()));

    //connect(this,SIGNAL(rejected()),this,SLOT(clickedStop()));

}

MeasurementDialog::~MeasurementDialog()
{
    emit stopMeasurement();
    delete ui;
}
void MeasurementDialog::mousePressEvent(QMouseEvent *event)
{
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void MeasurementDialog::mouseMoveEvent(QMouseEvent *event)
{
    if(this->isMaximized()) {
        this->showNormal();
        ui->maximizePushButton->setChecked(false);
    }
    this->move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}
void MeasurementDialog::clear()
{
    ui->nuclideTitleLabel->clear();
    ui->filenameTitleLabel->clear();
    ui->solutionTitleLabel->clear();
    ui->timeLeftTitleLabel->clear();

    ui->mode_lineEdit->clear();
    ui->nameSeriesSources_lineEdit->clear();
    ui->nameParameters_lineEdit->clear();
    ui->blankTime_lineEdit->clear();
    ui->sourceTime_lineEdit->clear();
    ui->reps_lineEdit->clear();

    ui->statusAnode_lineEdit->clear();
    ui->statusFirstD_lineEdit->clear();
    ui->statusFocusingD_lineEdit->clear();
    ui->statusGamma_lineEdit->clear();
    ui->voltAnode_doubleSpinBox->setValue(0);
    ui->voltFirstD_doubleSpinBox->setValue(0);
    ui->voltFocusingD_doubleSpinBox->setValue(0);
    ui->voltGamma_doubleSpinBox->setValue(0);
    ui->ampAnode_doubleSpinBox->setValue(0);
    ui->ampFirstD_doubleSpinBox->setValue(0);
    ui->ampFocusingD_doubleSpinBox->setValue(0);
    ui->ampGamma_doubleSpinBox->setValue(0);
    ui->resolvingTimeBeta_lineEdit->clear();
    ui->resolvingTimeGamma_lineEdit->clear();
    ui->deadTimeBeta_lineEdit->clear();
    ui->deadTimeGamma_lineEdit->clear();
    ui->delayTimeBeta_lineEdit->clear();

    ui->cpsA_doubleSpinBox->setValue(0);
    ui->cpsB_doubleSpinBox->setValue(0);
    ui->cpsC_doubleSpinBox->setValue(0);
    ui->cpsG_doubleSpinBox->setValue(0);
    ui->countsA_spinBox->setValue(0);
    ui->countsB_spinBox->setValue(0);
    ui->countsC_spinBox->setValue(0);
    ui->countsG_spinBox->setValue(0);

    ui->deadTimeA_progressBar->setValue(0);
    ui->deadTimeB_progressBar->setValue(0);
    ui->deadTimeC_progressBar->setValue(0);
    ui->deadTimeG_progressBar->setValue(0);
    ui->effTDCR_progressBar->setValue(0);
    ui->effBeta_progressBar->setValue(0);
    ui->effGamma_progressBar->setValue(0);

    ui->begin_dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->end_dateTimeEdit->setDateTime(QDateTime::currentDateTime());

    ui->source_progressBar->setMinimum(0);
    ui->source_progressBar->setMaximum(0);
    ui->source_progressBar->setValue(0);
    ui->point_progressBar->setMinimum(0);
    ui->point_progressBar->setMaximum(0);
    ui->point_progressBar->setValue(0);
    ui->reps_progressBar->setMinimum(0);
    ui->reps_progressBar->setMaximum(0);
    ui->reps_progressBar->setValue(0);
    ui->time_progressBar->setMinimum(0);
    ui->time_progressBar->setMaximum(0);
    ui->time_progressBar->setValue(0);

    int rows = ui->rt_tableWidget->rowCount();
    for(int i=0;i<rows;i++)
        ui->rt_tableWidget->removeRow(0);
    rows = ui->pm_tableWidget->rowCount();
    for(int i=0;i<rows;i++)
        ui->pm_tableWidget->removeRow(0);

}

void MeasurementDialog::set(MEASUREMENTDIALOGSETTINGS settings)
{
    ui->nuclideTitleLabel->setText(settings.nuclide);
    ui->filenameTitleLabel->setText(settings.filename);
    ui->solutionTitleLabel->setText(settings.solution);

    ui->mode_lineEdit->setText(QCoreApplication::translate("MainWindow",modeMeasurement.at(settings.mode).toStdString().c_str()));
    ui->nameSeriesSources_lineEdit->setText(settings.nameSeriesSources);
    ui->nameParameters_lineEdit->setText(settings.nameParameters);
    ui->nameSeriesSources_lineEdit->adjustSize();
    ui->nameParameters_lineEdit->adjustSize();
    ui->blankTime_lineEdit->setText(settings.blankTime);
    ui->sourceTime_lineEdit->setText(settings.sourceTime);
    ui->reps_lineEdit->setText(settings.reps);

    int columns = settings.headersRecordsTableRT.count();
    if(columns > ui->rt_tableWidget->columnCount()) {
        for(int j=ui->rt_tableWidget->columnCount();j<columns;j++)
            ui->rt_tableWidget->insertColumn(j);
    }else if(columns < ui->rt_tableWidget->columnCount()) {
        for(int j=ui->rt_tableWidget->columnCount()-1; j>=columns; j--)
            ui->rt_tableWidget->removeColumn(j);
    }
    ui->rt_tableWidget->setHorizontalHeaderLabels(settings.headersRecordsTableRT);

    columns = settings.headersRecordsTablePM.count();
    if(columns > ui->pm_tableWidget->columnCount()) {
        for(int j=ui->pm_tableWidget->columnCount();j<columns;j++)
            ui->pm_tableWidget->insertColumn(j);
    }else if(columns < ui->pm_tableWidget->columnCount()) {
        for(int j=ui->pm_tableWidget->columnCount()-1; j>=columns; j--)
            ui->pm_tableWidget->removeColumn(j);
    }
    ui->pm_tableWidget->setHorizontalHeaderLabels(settings.headersRecordsTablePM);
}

void MeasurementDialog::setStatusProcess(STATUSPROCESSMEASUREMENTDIALOG status)
{
    ui->begin_dateTimeEdit->setDateTime(status.beginMeasurement);
    ui->end_dateTimeEdit->setDateTime(status.endMeasurement);
    ui->end_dateTimeEdit->adjustSize();

    ui->time_progressBar->setMaximum(status.time);
    ui->time_progressBar->setValue(status.currentTime);
    ui->source_progressBar->setMaximum(status.sourceCount);
    ui->source_progressBar->setValue(status.currentSource);
    ui->point_progressBar->setMaximum(status.pointCount);
    ui->point_progressBar->setValue(status.currentPoint);
    ui->reps_progressBar->setMaximum(status.reps);
    ui->reps_progressBar->setValue(status.currentReps);

    ui->timeLeftTitleLabel->setText(status.timeLeft);

}

void MeasurementDialog::setStatusParameters(STATUSPARAMETERSMEASUREMENTDIALOG status)
{
    ui->statusAnode_lineEdit->setText(status.statusAnode);
    ui->statusFirstD_lineEdit->setText(status.statusFirstDynode);
    ui->statusFocusingD_lineEdit->setText(status.statusFocusingDynode);
    ui->statusGamma_lineEdit->setText(status.statusGamma);
    ui->voltAnode_doubleSpinBox->setValue(status.voltAnode);
    ui->voltFirstD_doubleSpinBox->setValue(status.voltFirstDynode);
    ui->voltFocusingD_doubleSpinBox->setValue(status.voltFocusingDynode);
    ui->voltGamma_doubleSpinBox->setValue(status.voltGamma);
    ui->ampAnode_doubleSpinBox->setValue(status.ampAnode);
    ui->ampFirstD_doubleSpinBox->setValue(status.ampFirstDynode);
    ui->ampFocusingD_doubleSpinBox->setValue(status.ampFocusingDynode);
    ui->ampGamma_doubleSpinBox->setValue(status.ampGamma);

    ui->statusAnalyzer_lineEdit->setText(status.statusAnalyzer);
    ui->resolvingTimeBeta_lineEdit->setText(status.resolvingTimeBeta);
    ui->deadTimeBeta_lineEdit->setText(status.deadTimeBeta);
    ui->resolvingTimeGamma_lineEdit->setText(status.resolvingTimeGamma);
    ui->deadTimeGamma_lineEdit->setText(status.deadTimeGamma);
    ui->delayTimeBeta_lineEdit->setText(status.delayTimeBeta);
}

void MeasurementDialog::setStatusTrack(STATUSTRACKMEASUREMENTDIALOG status)
{
    if(ui->cpsA_doubleSpinBox->maximum()<status.cpsA)
        ui->cpsA_doubleSpinBox->setMaximum(status.cpsA);
    if(ui->cpsB_doubleSpinBox->maximum()<status.cpsB)
        ui->cpsB_doubleSpinBox->setMaximum(status.cpsB);
    if(ui->cpsC_doubleSpinBox->maximum()<status.cpsC)
        ui->cpsC_doubleSpinBox->setMaximum(status.cpsC);
    if(ui->cpsG_doubleSpinBox->maximum()<status.cpsG)
        ui->cpsG_doubleSpinBox->setMaximum(status.cpsG);
    ui->cpsA_doubleSpinBox->setValue(status.cpsA);
    ui->cpsB_doubleSpinBox->setValue(status.cpsB);
    ui->cpsC_doubleSpinBox->setValue(status.cpsC);
    ui->cpsG_doubleSpinBox->setValue(status.cpsG);

    if(ui->countsA_spinBox->maximum()<status.countsA)
        ui->countsA_spinBox->setMaximum(status.countsA);
    if(ui->countsB_spinBox->maximum()<status.countsB)
        ui->countsB_spinBox->setMaximum(status.countsB);
    if(ui->countsC_spinBox->maximum()<status.countsC)
        ui->countsC_spinBox->setMaximum(status.countsC);
    if(ui->countsG_spinBox->maximum()<status.countsG)
        ui->countsG_spinBox->setMaximum(status.countsG);
    ui->countsA_spinBox->setValue(status.countsA);
    ui->countsB_spinBox->setValue(status.countsB);
    ui->countsC_spinBox->setValue(status.countsC);
    ui->countsG_spinBox->setValue(status.countsG);

    ui->deadTimeA_progressBar->setValue(status.deatTimeA);
    ui->deadTimeB_progressBar->setValue(status.deatTimeB);
    ui->deadTimeC_progressBar->setValue(status.deatTimeC);
    ui->deadTimeG_progressBar->setValue(status.deatTimeG);

    ui->effTDCR_progressBar->setValue(status.efficiencyTDCR);
    ui->effBeta_progressBar->setValue(status.efficiencyBeta4LS);
    ui->effGamma_progressBar->setValue(status.efficiencyGamma4LS);
}

void MeasurementDialog::insertBlankVial()
{

    if(QMessageBox::information(this,tr("Vial"),tr("Please insert blank vial"),QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok) {
        emit startBlankVial();
    }else {
        emit stopMeasurement();
    }
}

void MeasurementDialog::insertSourceVial(QString msg)
{

    QMessageBox *msgBox = new QMessageBox;
    msgBox->setWindowFlags(Qt::WindowStaysOnTopHint);
    msgBox->setIcon(QMessageBox::Information);
    msgBox->setWindowTitle(tr("Vial"));
    msgBox->setText(tr("Please insert vial: ")+msg);
    msgBox->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    //QMessageBox::information(this,tr("Vial"),tr("Please insert vial: ")+msg,QMessageBox::Ok, QMessageBox::Cancel)
    if(QMessageBox::information(this,tr("Vial"),tr("Please insert vial: ")+msg,QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok) {
        emit startSourceVial();
    }else {
        emit stopMeasurement();
    }
    delete msgBox;
}

void MeasurementDialog::showQustionBox(QString title, QString msg)
{

    if(QMessageBox::question(this,title,msg,QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok) {
        emit answerQustionBox(true);
    }else {
        emit answerQustionBox(false);
    }
}

void MeasurementDialog::insertRowTable()
{
    int rows = ui->rt_tableWidget->rowCount();
    ui->rt_tableWidget->insertRow(rows);
    rows = ui->pm_tableWidget->rowCount();
    ui->pm_tableWidget->insertRow(rows);
    ui->rt_tableWidget->scrollToBottom();
    ui->pm_tableWidget->scrollToBottom();
}

void MeasurementDialog::setRTTable(QStringList record)
{
    int row = ui->rt_tableWidget->rowCount()-1;
    if(row>-1) {
        for(int j=0;j<record.count();j++) {
            QTableWidgetItem *item = new QTableWidgetItem(record.at(j));
            item->setTextAlignment(Qt::AlignCenter);
            if(record.at(23)!="0" || record.at(24)!="0"  || record.at(25)!="0"  || record.at(26)!="0"  || record.at(27)!="0"  || record.at(28)!="0")
                item->setForeground(QColor::fromRgb(255,0,0));
            ui->rt_tableWidget->setItem(row,j,item);
        }
        ui->rt_tableWidget->resizeColumnsToContents();
    }

}

void MeasurementDialog::setPMTable(QStringList record)
{
    int row = ui->pm_tableWidget->rowCount()-1;
    if(row>-1) {
        for(int j=0;j<record.count();j++) {
            QTableWidgetItem *item = new QTableWidgetItem(record.at(j));
            item->setTextAlignment(Qt::AlignCenter);
            if(record.at(23)!="0" || record.at(24)!="0"  || record.at(25)!="0"  || record.at(26)!="0"  || record.at(27)!="0"  || record.at(28)!="0")
                item->setForeground(QColor::fromRgb(255,0,0));
            ui->pm_tableWidget->setItem(row,j,item);
        }
        ui->pm_tableWidget->resizeColumnsToContents();
    }

}

void MeasurementDialog::showFinishedBox(QString text, QString time)
{
    this->activateWindow();
    this->raise();
    QMessageBox::information(this,tr("Measurement"),text+time,QMessageBox::Ok);
    this->accept();
}

void MeasurementDialog::clickedStop()
{
    this->activateWindow();
    this->raise();
    if(QMessageBox::question(this,tr("Stop measurement"),tr("Are you sure you want measurement stopped?"),QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
        emit stopMeasurement();

    }
}

void MeasurementDialog::clickedMaximizeButton()
{
    if(this->isMaximized()) {
        this->showNormal();
        ui->maximizePushButton->setChecked(false);
    }else {
        this->showMaximized();
        ui->maximizePushButton->setChecked(true);
    }
}


void MeasurementDialog::reject()
{
    clickedStop();
}

void MeasurementDialog::keyPressEvent(QKeyEvent *e)
{
    if(e->key() != Qt::Key_Escape) {
        QDialog::keyPressEvent(e);
    }else {
        clickedStop();
    }
}

void MeasurementDialog::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        clickedMaximizeButton();

    }else {
        QDialog::mouseDoubleClickEvent(event);
    }
}
