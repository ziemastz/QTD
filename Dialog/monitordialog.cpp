#include "monitordialog.h"
#include "ui_monitordialog.h"

MonitorDialog::MonitorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MonitorDialog)
{
    ui->setupUi(this);
    this->setModal(true);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setSizeGripEnabled(true);

    int columns = recordMonitorHeadersRT.count();
    if(columns > ui->rt_tableWidget->columnCount()) {
        for(int j=ui->rt_tableWidget->columnCount();j<columns;j++)
            ui->rt_tableWidget->insertColumn(j);
    }else if(columns < ui->rt_tableWidget->columnCount()) {
        for(int j=ui->rt_tableWidget->columnCount()-1; j>=columns; j--)
            ui->rt_tableWidget->removeColumn(j);
    }
    ui->rt_tableWidget->setHorizontalHeaderLabels(recordMonitorHeadersRT);

    columns = recordMonitorHeadersPM.count();
    if(columns > ui->pm_tableWidget->columnCount()) {
        for(int j=ui->pm_tableWidget->columnCount();j<columns;j++)
            ui->pm_tableWidget->insertColumn(j);
    }else if(columns < ui->pm_tableWidget->columnCount()) {
        for(int j=ui->pm_tableWidget->columnCount()-1; j>=columns; j--)
            ui->pm_tableWidget->removeColumn(j);
    }
    ui->pm_tableWidget->setHorizontalHeaderLabels(recordMonitorHeadersPM);

    insertRowTable();
    isMeasurement = false;
    time = 0;

    connect(ui->rt_tableWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showContextMenu(QPoint)));
    connect(ui->pm_tableWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showContextMenu(QPoint)));

    //hv
    connect(ui->anodeBeta_doubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(anodeBetaChanged(double)));
    connect(ui->firstDynodeBeta_doubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(firstDynodeChanged(double)));
    connect(ui->focusingDynodeBeta_doubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(focusingDynodeChanged(double)));
    connect(ui->anodeGamma_doubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(anodeGammaChanged(double)));
    //analyzer
    connect(ui->resolvingTimeBeta_spinBox,SIGNAL(valueChanged(int)),this,SLOT(resolvingTimeBetaChanged(int)));
    connect(ui->deadTimeBeta_spinBox,SIGNAL(valueChanged(int)),this,SLOT(deadTimeBetaChanged(int)));
    connect(ui->resolvingTimeGamma_spinBox,SIGNAL(valueChanged(int)),this,SLOT(resolvingTimeGammaChanged(int)));
    connect(ui->deadTimeGamma_spinBox,SIGNAL(valueChanged(int)),this,SLOT(deadTimeGammaChanged(int)));
    connect(ui->delayTimeBeta_spinBox,SIGNAL(valueChanged(int)),this,SLOT(delayTimeBetaChanged(int)));

    //control
    connect(ui->start_pushButton,SIGNAL(clicked(bool)),this,SLOT(clickedStart()));
    connect(ui->stop_pushButton,SIGNAL(clicked(bool)),this,SLOT(clickedStop()));
    connect(ui->reset_pushButton,SIGNAL(clicked(bool)),this,SLOT(clickedReset()));
    connect(ui->addRow_pushButton,SIGNAL(clicked(bool)),this,SLOT(insertRowTable()));

    connect(ui->maximize_pushButton,SIGNAL(clicked(bool)),this,SLOT(clickedMaximize()));
    connect(ui->stopMonitor_PushButton,SIGNAL(clicked(bool)),this,SLOT(clickedClose()));
    connect(ui->save_pushButton,SIGNAL(clicked(bool)),this,SLOT(clickedSaveToFile()));
    //save to file

}

MonitorDialog::~MonitorDialog()
{
    delete ui;
}
void MonitorDialog::mousePressEvent(QMouseEvent *event)
{
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void MonitorDialog::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}
void MonitorDialog::setStatusParameters(STATUSPARAMETERSMEASUREMENTDIALOG status)
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

    if(!ui->resolvingTimeBeta_spinBox->hasFocus()) {
        ui->resolvingTimeBeta_spinBox->blockSignals(true);
        ui->resolvingTimeBeta_spinBox->setValue(status.resolvingTimeBeta.toInt());
        ui->resolvingTimeBeta_spinBox->blockSignals(false);
    }
    if(!ui->deadTimeBeta_spinBox->hasFocus()) {
        ui->deadTimeBeta_spinBox->blockSignals(true);
        ui->deadTimeBeta_spinBox->setValue(status.deadTimeBeta.toInt());
        ui->deadTimeBeta_spinBox->blockSignals(false);
    }
    if(!ui->resolvingTimeGamma_spinBox->hasFocus()) {
        ui->resolvingTimeGamma_spinBox->blockSignals(true);
        ui->resolvingTimeGamma_spinBox->setValue(status.resolvingTimeGamma.toInt());
        ui->resolvingTimeGamma_spinBox->blockSignals(false);
    }
    if(!ui->deadTimeGamma_spinBox->hasFocus()) {
        ui->deadTimeGamma_spinBox->blockSignals(true);
        ui->deadTimeGamma_spinBox->setValue(status.deadTimeGamma.toInt());
        ui->deadTimeGamma_spinBox->blockSignals(false);
    }
    if(!ui->delayTimeBeta_spinBox->hasFocus()) {
        ui->delayTimeBeta_spinBox->blockSignals(true);
        ui->delayTimeBeta_spinBox->setValue(status.delayTimeBeta.toInt());
        ui->delayTimeBeta_spinBox->blockSignals(false);
    }
}

void MonitorDialog::setStatusTrack(STATUSTRACKMEASUREMENTDIALOG status)
{
    ui->cpsA_doubleSpinBox->setValue(status.cpsA);
    ui->cpsB_doubleSpinBox->setValue(status.cpsB);
    ui->cpsC_doubleSpinBox->setValue(status.cpsC);
    ui->cpsG_doubleSpinBox->setValue(status.cpsG);

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

void MonitorDialog::insertRowTable()
{
    int rows = ui->rt_tableWidget->rowCount();
    ui->rt_tableWidget->insertRow(rows);
    rows = ui->pm_tableWidget->rowCount();
    ui->pm_tableWidget->insertRow(rows);
    ui->rt_tableWidget->scrollToBottom();
    ui->pm_tableWidget->scrollToBottom();

    if(rows != 0) {
        QStringList record;
        for(int j=0;j<recordMonitorHeadersRT.size();j++) {
            QTableWidgetItem *item = ui->rt_tableWidget->item(rows-1,j);
            if(item != 0) {
                if(!item->text().isEmpty())
                    record << item->text();
                else
                    record << "-";
            }else {
                record << "-";
            }


        }
        record << "-"
               << QString::number(ui->voltAnode_doubleSpinBox->value())
               << QString::number(ui->voltFirstD_doubleSpinBox->value())
               << QString::number(ui->voltFocusingD_doubleSpinBox->value())
               << QString::number(ui->voltGamma_doubleSpinBox->value())
               << "-"
               << QString::number(ui->resolvingTimeBeta_spinBox->value())
               << QString::number(ui->deadTimeBeta_spinBox->value())
               << QString::number(ui->resolvingTimeGamma_spinBox->value())
               << QString::number(ui->deadTimeGamma_spinBox->value())
               << QString::number(ui->delayTimeBeta_spinBox->value());
        recordsRT << record;
        record.clear();
        for(int j=0;j<recordMonitorHeadersPM.size();j++) {
            QTableWidgetItem *item = ui->pm_tableWidget->item(rows-1,j);
            if(item != 0) {
                if(!item->text().isEmpty())
                    record << item->text();
                else
                    record << "";
            }else {
                record << "";
            }


        }
        record << "-"
               << QString::number(ui->voltAnode_doubleSpinBox->value())
               << QString::number(ui->voltFirstD_doubleSpinBox->value())
               << QString::number(ui->voltFocusingD_doubleSpinBox->value())
               << QString::number(ui->voltGamma_doubleSpinBox->value())
               << "-"
               << QString::number(ui->resolvingTimeBeta_spinBox->value())
               << QString::number(ui->deadTimeBeta_spinBox->value())
               << QString::number(ui->resolvingTimeGamma_spinBox->value())
               << QString::number(ui->deadTimeGamma_spinBox->value())
               << QString::number(ui->delayTimeBeta_spinBox->value());
        recordsPM << record;
    }
}

void MonitorDialog::setRTTable(QStringList record)
{
    int row = ui->rt_tableWidget->rowCount()-1;
    if(row>-1) {
        for(int j=0;j<record.count();j++) {
            QTableWidgetItem *item = new QTableWidgetItem(record.at(j));
            item->setTextAlignment(Qt::AlignCenter);
            if(record.at(18)!="0" || record.at(19)!="0"  || record.at(20)!="0"  || record.at(21)!="0"  || record.at(22)!="0"  || record.at(23)!="0")
                item->setForeground(QColor::fromRgb(255,0,0));
            ui->rt_tableWidget->setItem(row,j,item);
        }
        ui->rt_tableWidget->resizeColumnsToContents();
    }

    if(isMeasurement) {
        ui->time_spinBox->setValue(time-(int)(record.last().toFloat()/freqClk));
    }
}

void MonitorDialog::setPMTable(QStringList record)
{
    int row = ui->pm_tableWidget->rowCount()-1;
    if(row>-1) {
        for(int j=0;j<record.count();j++) {
            QTableWidgetItem *item = new QTableWidgetItem(record.at(j));
            item->setTextAlignment(Qt::AlignCenter);
            if(record.at(18)!="0" || record.at(19)!="0"  || record.at(20)!="0"  || record.at(21)!="0"  || record.at(22)!="0"  || record.at(23)!="0")
                item->setForeground(QColor::fromRgb(255,0,0));
            ui->pm_tableWidget->setItem(row,j,item);
        }
        ui->pm_tableWidget->resizeColumnsToContents();
    }

}

void MonitorDialog::clear()
{
    ui->time_spinBox->setValue(0);
    recordsRT.clear();
    recordsPM.clear();
    ui->start_pushButton->blockSignals(true);
    ui->stop_pushButton->blockSignals(true);
    ui->reset_pushButton->blockSignals(true);
    ui->start_pushButton->setChecked(false);
    ui->stop_pushButton->setChecked(false);
    ui->reset_pushButton->setChecked(false);
    ui->start_pushButton->blockSignals(false);
    ui->stop_pushButton->blockSignals(false);
    ui->reset_pushButton->blockSignals(false);


    int rows = ui->rt_tableWidget->rowCount();
    for(int i=0;i<rows;i++)
        ui->rt_tableWidget->removeRow(0);
    rows = ui->pm_tableWidget->rowCount();
    for(int i=0;i<rows;i++)
        ui->pm_tableWidget->removeRow(0);
    insertRowTable();
}

void MonitorDialog::stopped()
{
    ui->anodeBeta_doubleSpinBox->setEnabled(true);
    ui->firstDynodeBeta_doubleSpinBox->setEnabled(true);
    ui->focusingDynodeBeta_doubleSpinBox->setEnabled(true);
    ui->anodeGamma_doubleSpinBox->setEnabled(true);

    ui->resolvingTimeBeta_spinBox->setEnabled(true);
    ui->deadTimeBeta_spinBox->setEnabled(true);
    ui->resolvingTimeGamma_spinBox->setEnabled(true);
    ui->deadTimeGamma_spinBox->setEnabled(true);
    ui->delayTimeBeta_spinBox->setEnabled(true);

    if(isMeasurement) {
        isMeasurement = false;
        ui->time_spinBox->setValue(time);
        ui->time_spinBox->setEnabled(true);
        insertRowTable();
        emit startMeasMonitor(0);
        ui->start_pushButton->setChecked(false);
        ui->stop_pushButton->setChecked(false);
        ui->reset_pushButton->setChecked(false);
    }

}

void MonitorDialog::showContextMenu(const QPoint &pos)
{
    QTableWidget *table = (QTableWidget *)sender();

    if(table->selectedItems().count()<1)
        return;

    QMenu myMenu;
    if(table->selectedItems().count() == 1) {
        myMenu.addAction(tr("Delete"));
    }else {
        myMenu.addAction(tr("Delete ")+QString::number(table->selectedItems().count())+tr(" rows"));
    }

    QPoint globalPos = table->mapToGlobal(pos);
    QAction* selectedItem = myMenu.exec(globalPos);
    if(selectedItem) {
        QString textAction = selectedItem->text();

        if(textAction==tr("Delete") || textAction==tr("Delete ")+QString::number(table->selectedItems().count())+tr(" rows")) {

            for(int i=table->selectedItems().count()-1; i>=0; i--) {
                int row = table->selectedItems().last()->row();
                if(row<recordsRT.size())
                    recordsRT.removeAt(row);
                if(row<recordsPM.size())
                    recordsPM.removeAt(row);
                ui->rt_tableWidget->removeRow(row);
                ui->pm_tableWidget->removeRow(row);
            }

        }

    }


}

void MonitorDialog::anodeBetaChanged(double value)
{
    emit setHV((float)value,CH_HVANODEBETA);
}

void MonitorDialog::firstDynodeChanged(double value)
{
    emit setHV((float)value,CH_HVFIRSTDYNODEBETA);
}

void MonitorDialog::focusingDynodeChanged(double value)
{
    emit setHV((float)value,CH_HVFOCUSINGDYNODEBETA);
}

void MonitorDialog::anodeGammaChanged(double value)
{
    emit setHV((float)value,CH_HVANODEGAMMA);
}

void MonitorDialog::resolvingTimeBetaChanged(int value)
{
    emit setAnalyzer(value,CH_RESOLVINGTIMEBETA);
}

void MonitorDialog::deadTimeBetaChanged(int value)
{
    emit setAnalyzer(value,CH_DEADTIMEBETA);
}

void MonitorDialog::resolvingTimeGammaChanged(int value)
{
    emit setAnalyzer(value,CH_RESOLVINGTIMEGAMMA);
}

void MonitorDialog::deadTimeGammaChanged(int value)
{
    emit setAnalyzer(value,CH_DEADTIMEGAMMA);
}

void MonitorDialog::delayTimeBetaChanged(int value)
{
    emit setAnalyzer(value,CH_DELAYTIMEBETA);
}

void MonitorDialog::clickedStart()
{

    time = ui->time_spinBox->value();

    if(time > 0) {
        ui->start_pushButton->setChecked(true);
        ui->stop_pushButton->setChecked(false);
        ui->reset_pushButton->setChecked(false);

        ui->anodeBeta_doubleSpinBox->setEnabled(false);
        ui->firstDynodeBeta_doubleSpinBox->setEnabled(false);
        ui->focusingDynodeBeta_doubleSpinBox->setEnabled(false);
        ui->anodeGamma_doubleSpinBox->setEnabled(false);

        ui->resolvingTimeBeta_spinBox->setEnabled(false);
        ui->deadTimeBeta_spinBox->setEnabled(false);
        ui->resolvingTimeGamma_spinBox->setEnabled(false);
        ui->deadTimeGamma_spinBox->setEnabled(false);
        ui->delayTimeBeta_spinBox->setEnabled(false);

        ui->time_spinBox->setEnabled(false);
        isMeasurement = true;
    }else {
        ui->start_pushButton->setChecked(false);
        ui->stop_pushButton->setChecked(false);
        ui->reset_pushButton->setChecked(false);

        ui->anodeBeta_doubleSpinBox->setEnabled(true);
        ui->firstDynodeBeta_doubleSpinBox->setEnabled(true);
        ui->focusingDynodeBeta_doubleSpinBox->setEnabled(true);
        ui->anodeGamma_doubleSpinBox->setEnabled(true);

        ui->resolvingTimeBeta_spinBox->setEnabled(true);
        ui->deadTimeBeta_spinBox->setEnabled(true);
        ui->resolvingTimeGamma_spinBox->setEnabled(true);
        ui->deadTimeGamma_spinBox->setEnabled(true);
        ui->delayTimeBeta_spinBox->setEnabled(true);

        ui->time_spinBox->setEnabled(true);
        isMeasurement = false;
    }

    emit startMeasMonitor(time);
}

void MonitorDialog::clickedStop()
{
    ui->start_pushButton->setChecked(false);
    ui->stop_pushButton->setChecked(true);
    ui->reset_pushButton->setChecked(false);

    ui->anodeBeta_doubleSpinBox->setEnabled(true);
    ui->firstDynodeBeta_doubleSpinBox->setEnabled(true);
    ui->focusingDynodeBeta_doubleSpinBox->setEnabled(true);
    ui->anodeGamma_doubleSpinBox->setEnabled(true);

    ui->resolvingTimeBeta_spinBox->setEnabled(true);
    ui->deadTimeBeta_spinBox->setEnabled(true);
    ui->resolvingTimeGamma_spinBox->setEnabled(true);
    ui->deadTimeGamma_spinBox->setEnabled(true);
    ui->delayTimeBeta_spinBox->setEnabled(true);

    ui->time_spinBox->setEnabled(true);
    if(isMeasurement) {
        isMeasurement = false;
        ui->time_spinBox->setValue(time);
    }

    emit stopMeasMonitor();
}

void MonitorDialog::clickedReset()
{
    ui->start_pushButton->setChecked(false);
    ui->stop_pushButton->setChecked(false);
    ui->reset_pushButton->setChecked(true);

    ui->anodeBeta_doubleSpinBox->setEnabled(true);
    ui->firstDynodeBeta_doubleSpinBox->setEnabled(true);
    ui->focusingDynodeBeta_doubleSpinBox->setEnabled(true);
    ui->anodeGamma_doubleSpinBox->setEnabled(true);

    ui->resolvingTimeBeta_spinBox->setEnabled(true);
    ui->deadTimeBeta_spinBox->setEnabled(true);
    ui->resolvingTimeGamma_spinBox->setEnabled(true);
    ui->deadTimeGamma_spinBox->setEnabled(true);
    ui->delayTimeBeta_spinBox->setEnabled(true);

    ui->time_spinBox->setEnabled(true);
    if(isMeasurement) {
        isMeasurement = false;
        ui->time_spinBox->setValue(time);
    }

    emit resetMeasMonitor();
}

void MonitorDialog::clickedClose()
{
    if(QMessageBox::question(this,tr("Monitor"),tr("Are you sure you want stopped monitor?"),QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
        emit stop();

    }
}

void MonitorDialog::clickedMaximize()
{
    if(this->isMaximized()) {
        this->showNormal();
        ui->maximize_pushButton->setChecked(false);
    }else{
        this->showMaximized();
        ui->maximize_pushButton->setChecked(true);
    }
}

void MonitorDialog::reject()
{
    clickedClose();
}

void MonitorDialog::clickedSaveToFile()
{    
    MONITORDIALOGDATA data;
    data.nuclide = ui->nuclide_lineEdit->text();
    data.vial = ui->vial_lineEdit->text();
    data.comment = ui->comment_lineEdit->text();
    data.recordsRT = recordsRT;
    data.recordsPM = recordsPM;

    emit saveToFileMonitorData(data);
}

void MonitorDialog::keyPressEvent(QKeyEvent *e)
{
    if(e->key() != Qt::Key_Escape) {
        QDialog::keyPressEvent(e);
    }else {
        clickedClose();
    }
}

void MonitorDialog::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        clickedMaximize();
    }else {
        QDialog::mouseDoubleClickEvent(event);
    }
}
