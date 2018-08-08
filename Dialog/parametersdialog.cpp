#include "parametersdialog.h"
#include "ui_parametersdialog.h"


ParametersDialog::ParametersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParametersDialog)
{
    ui->setupUi(this);
    this->setModal(true);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setSizeGripEnabled(true);

    connect(ui->name_comboBox,SIGNAL(currentIndexChanged(QString)),this,SIGNAL(changedName(QString)));
    connect(ui->cancelPushButton,SIGNAL(clicked(bool)),this,SLOT(reject()));
    connect(ui->edit_pushButton,SIGNAL(clicked(bool)),this,SLOT(clickedEdit()));
    connect(ui->add_pushButton,SIGNAL(clicked(bool)),this,SLOT(clickedAdd()));
    connect(ui->remove_pushButton,SIGNAL(clicked(bool)),this,SLOT(clickedRemove()));
    connect(ui->savePushButton,SIGNAL(clicked(bool)),this,SLOT(clickedSave()));

    connect(ui->hvABeta_tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChangedTable(int,int)));
    connect(ui->hvD1Beta_tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChangedTable(int,int)));
    connect(ui->hvFBeta_tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChangedTable(int,int)));
    connect(ui->hvAGamma_tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChangedTable(int,int)));
    connect(ui->r1_tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChangedTable(int,int)));
    connect(ui->dt1_tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChangedTable(int,int)));
    connect(ui->dlBeta_tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChangedTable(int,int)));
    connect(ui->r2_tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChangedTable(int,int)));
    connect(ui->dt2_tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChangedTable(int,int)));
    connect(ui->countsEnable_radioButton,SIGNAL(toggled(bool)),this,SLOT(toggledEnableLimited()));
    connect(ui->uncertaintyEnable_radioButton,SIGNAL(toggled(bool)),this,SLOT(toggledEnableLimited()));
    connect(ui->limitedCount_groupBox,SIGNAL(toggled(bool)),this,SLOT(toggledLimitedCount(bool)));

}

ParametersDialog::~ParametersDialog()
{
    delete ui;
}
void ParametersDialog::mousePressEvent(QMouseEvent *event)
{
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void ParametersDialog::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}
void ParametersDialog::setSettings(PARAMETERSDIALOGSETTINGS settings)
{
    ui->name_comboBox->blockSignals(true);
    QString currText = ui->name_comboBox->currentText();
    ui->name_comboBox->clear();
    ui->name_comboBox->addItems(settings.nameParametersList);
    ui->name_comboBox->setCurrentIndex(ui->name_comboBox->findText(currText));
    ui->name_comboBox->blockSignals(false);

    QCompleter *completer = new QCompleter(settings.completerNuclideList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->nuclide_lineEdit->setCompleter(completer);

    ui->channelLimited_comboBox->clear();
    ui->channelLimited_comboBox->addItems(settings.channelLimitedList);
    ui->channelLimited_comboBox->setCurrentIndex(0);
}

void ParametersDialog::setName(QString name)
{
    ui->name_comboBox->setCurrentIndex(ui->name_comboBox->findText(name));
}

void ParametersDialog::setData(PARAMETERSDATA data)
{
    if(data.name.isEmpty()) {
        ui->nuclide_lineEdit->clear();

        clearTable(ui->hvABeta_tableWidget);
        clearTable(ui->hvD1Beta_tableWidget);
        clearTable(ui->hvFBeta_tableWidget);
        clearTable(ui->hvAGamma_tableWidget);
        clearTable(ui->r1_tableWidget);
        clearTable(ui->dt1_tableWidget);
        clearTable(ui->dlBeta_tableWidget);
        clearTable(ui->r2_tableWidget);
        clearTable(ui->dt2_tableWidget);

        ui->lowerGWindow_spinBox->setValue(0);
        ui->upperGWindow_spinBox->setValue(ui->upperGWindow_spinBox->maximum());
        ui->limitedCount_groupBox->setChecked(false);
        ui->countsEnable_radioButton->setChecked(true);
        ui->countsLimit_spinBox->setValue(1000000);
        ui->uncertaintyLimit_doubleSpinBox->setValue(0);
        ui->realTimeEnable_radioButton->setChecked(true);
        ui->nuclide_lineEdit->setFocus();
    }else {
        ui->nuclide_lineEdit->setText(data.nuclide);

        addItemTable(ui->hvABeta_tableWidget,data.hvAnodeBeta);
        addItemTable(ui->hvD1Beta_tableWidget,data.hvFirstDynodeBeta);
        addItemTable(ui->hvFBeta_tableWidget,data.hvFocalysingDynodeBeta);
        addItemTable(ui->hvAGamma_tableWidget,data.hvAnodeGamma);

        addItemTable(ui->r1_tableWidget,data.resolvintTimeBeta);
        addItemTable(ui->dt1_tableWidget,data.deadTimeBeta);
        addItemTable(ui->dlBeta_tableWidget,data.delayTimeBeta);
        addItemTable(ui->r2_tableWidget,data.resolvingTimeGamma);
        addItemTable(ui->dt2_tableWidget,data.deadTimeGamma);

        ui->lowerGWindow_spinBox->setValue(data.lowerWindowGamma);
        ui->upperGWindow_spinBox->setValue(data.upperWindowGamma);
        switch(data.timeLimited) {
        case REAL_TIME:
        {
            ui->realTimeEnable_radioButton->setChecked(true);
            break;
        }
        case LIVETIME_BETA:
        {
            ui->tdcrLiveTimeEnable_radioButton->setChecked(true);
            break;
        }
        case LIVETIME_GAMMA:
        {
            ui->gammaLiveTimeEnable_radioButton->setChecked(true);
            break;
        }
        default:
        {
            ui->realTimeEnable_radioButton->setChecked(true);
            break;
        }
        }
        if(data.valueUncertaintlyLimited != 0 || data.valueCountLimited !=0) {
            ui->limitedCount_groupBox->setChecked(true);
            ui->channelLimited_comboBox->setCurrentIndex(data.channelLimited);
            if(data.valueCountLimited != 0) {
                ui->countsEnable_radioButton->setChecked(true);
                ui->countsLimit_spinBox->setEnabled(true);
                ui->countsLimit_spinBox->setValue(data.valueCountLimited);
                ui->uncertaintyLimit_doubleSpinBox->setEnabled(false);
                ui->uncertaintyLimit_doubleSpinBox->setValue(0);
            }else if(data.valueUncertaintlyLimited != 0) {
                ui->uncertaintyEnable_radioButton->setChecked(true);
                ui->uncertaintyLimit_doubleSpinBox->setEnabled(true);
                ui->uncertaintyLimit_doubleSpinBox->setValue(data.valueUncertaintlyLimited);
                ui->countsLimit_spinBox->setEnabled(false);
                ui->countsLimit_spinBox->setValue(1000000);
            }

        }else {
            ui->limitedCount_groupBox->setChecked(false);
            ui->uncertaintyLimit_doubleSpinBox->setValue(0);
            ui->countsLimit_spinBox->setValue(1000000);
            ui->channelLimited_comboBox->setCurrentIndex(0);
        }
    }
}

void ParametersDialog::cellChangedTable(int row, int column)
{
    QTableWidget *table;
    table = (QTableWidget *)sender();
    int rows = table->rowCount();
    if(row == rows-1)
    {
        QTableWidgetItem *item = table->item(row,column);
        if(item!=0)
        {
            if(!item->text().isEmpty())
            {
                table->insertRow(rows);
                //table->selectRow(rows);
            }
        }
    }
    else if(row == rows-2)
    {
        QTableWidgetItem *item = table->item(row,column);
        if(item!=0)
        {
            if(item->text().isEmpty())
                table->removeRow(rows-1);
        }
        else
            table->removeRow(rows-1);
    }
    else
    {
        QTableWidgetItem *item = table->item(row,column);
        if(item!=0)
        {
            if(item->text().isEmpty())
                table->removeRow(row);
        }
    }
}

void ParametersDialog::toggledEnableLimited()
{
    if(ui->countsEnable_radioButton->isChecked()) {
        ui->countsLimit_spinBox->setEnabled(true);
        ui->uncertaintyLimit_doubleSpinBox->setEnabled(false);
        ui->uncertaintyLimit_label->setEnabled(false);
    }else if(ui->uncertaintyEnable_radioButton->isChecked()) {
        ui->countsLimit_spinBox->setEnabled(false);
        ui->uncertaintyLimit_doubleSpinBox->setEnabled(true);
        ui->uncertaintyLimit_label->setEnabled(true);
    }else {
        ui->countsEnable_radioButton->setChecked(true);
        ui->uncertaintyEnable_radioButton->setChecked(false);
    }
}

void ParametersDialog::toggledLimitedCount(bool toggled)
{
    if(!toggled) {
        ui->countsEnable_radioButton->setChecked(true);
        ui->countsLimit_spinBox->setValue(1000000);
        ui->uncertaintyLimit_doubleSpinBox->setValue(0);
    }
}

void ParametersDialog::clickedEdit()
{
    ui->name_comboBox->blockSignals(true);
    ui->edit_pushButton->setChecked(true);
    ui->name_comboBox->setEditable(true);
    ui->name_comboBox->setFocus();
    connect(ui->name_comboBox->lineEdit(),SIGNAL(returnPressed()),this,SLOT(finishedEdit()));
    connect(ui->name_comboBox->lineEdit(),SIGNAL(editingFinished()),this,SLOT(finishedEdit()));
}

void ParametersDialog::finishedEdit()
{
    ui->edit_pushButton->setChecked(false);
    ui->name_comboBox->setEditable(false);
    ui->name_comboBox->blockSignals(false);
}

void ParametersDialog::clickedAdd()
{
    bool ok;
    QString name = QInputDialog::getText(this,tr("A new parameters"),tr("Name:"),QLineEdit::Normal,QString(),&ok);

    if(ok && !name.isEmpty()) {
        if(ui->name_comboBox->findText(name) != -1) {
            if(QMessageBox::question(this,tr("Name exists"),tr("The entered name exists\nDo you want to load data?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes) {
                ui->name_comboBox->setCurrentIndex(ui->name_comboBox->findText(name));
            }
        }else {
            ui->name_comboBox->blockSignals(true);
            ui->name_comboBox->addItem(name);
            ui->name_comboBox->setCurrentIndex(ui->name_comboBox->findText(name));
            ui->name_comboBox->blockSignals(false);
            ui->nuclide_lineEdit->clear();

            clearTable(ui->hvABeta_tableWidget);
            clearTable(ui->hvD1Beta_tableWidget);
            clearTable(ui->hvFBeta_tableWidget);
            clearTable(ui->hvAGamma_tableWidget);
            clearTable(ui->r1_tableWidget);
            clearTable(ui->dt1_tableWidget);
            clearTable(ui->dlBeta_tableWidget);
            clearTable(ui->r2_tableWidget);
            clearTable(ui->dt2_tableWidget);

            ui->lowerGWindow_spinBox->setValue(0);
            ui->upperGWindow_spinBox->setValue(ui->upperGWindow_spinBox->maximum());
            ui->limitedCount_groupBox->setChecked(false);
            ui->countsEnable_radioButton->setChecked(true);
            ui->countsLimit_spinBox->setValue(1000000);
            ui->uncertaintyLimit_doubleSpinBox->setValue(0);
            ui->realTimeEnable_radioButton->setChecked(true);
            ui->nuclide_lineEdit->setFocus();
        }
    }
}

void ParametersDialog::clickedRemove()
{
    if(QMessageBox::question(this,tr("Remove"),tr("Are you sure you want to remove?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes) {
        emit remove(ui->name_comboBox->currentText());
        ui->name_comboBox->removeItem(ui->name_comboBox->currentIndex());
    }
}

void ParametersDialog::clickedSave()
{
    PARAMETERSDATA param;
    param.name = ui->name_comboBox->currentText();
    param.nuclide = ui->nuclide_lineEdit->text();
    param.hvAnodeBeta = readTableF(ui->hvABeta_tableWidget);
    param.hvFirstDynodeBeta = readTableF(ui->hvD1Beta_tableWidget);
    param.hvFocalysingDynodeBeta = readTableF(ui->hvFBeta_tableWidget);
    param.hvAnodeGamma = readTableF(ui->hvAGamma_tableWidget);

    param.resolvintTimeBeta = readTableI(ui->r1_tableWidget);
    param.deadTimeBeta = readTableI(ui->dt1_tableWidget);
    param.delayTimeBeta = readTableI(ui->dlBeta_tableWidget);
    param.resolvingTimeGamma = readTableI(ui->r2_tableWidget);
    param.deadTimeGamma = readTableI(ui->dt2_tableWidget);

    param.lowerWindowGamma = ui->lowerGWindow_spinBox->value();
    param.upperWindowGamma = ui->upperGWindow_spinBox->value();

    if(ui->realTimeEnable_radioButton->isChecked()) {
        param.timeLimited = REAL_TIME;
    }else if(ui->tdcrLiveTimeEnable_radioButton->isChecked()) {
        param.timeLimited = LIVETIME_BETA;
    }else if(ui->gammaLiveTimeEnable_radioButton->isChecked()) {
        param.timeLimited = LIVETIME_GAMMA;
    }else {
        param.timeLimited = REAL_TIME;
    }

    param.channelLimited = static_cast<CHANNELLIMITED>(ui->channelLimited_comboBox->currentIndex());
    if(ui->limitedCount_groupBox->isChecked()) {
        if(ui->countsEnable_radioButton->isChecked()) {
            param.valueCountLimited = ui->countsLimit_spinBox->value();
            param.valueUncertaintlyLimited = 0;
        }else if(ui->uncertaintyEnable_radioButton->isChecked()) {
            param.valueUncertaintlyLimited = ui->uncertaintyLimit_doubleSpinBox->value();
            param.valueCountLimited = 0;
        }else {
            param.valueCountLimited = 0;
            param.valueUncertaintlyLimited = 0;
        }
    }else {
        param.valueCountLimited = 0;
        param.valueUncertaintlyLimited = 0;
    }
    if(param.hvAnodeBeta.isEmpty() || param.hvFirstDynodeBeta.isEmpty() || param.hvFocalysingDynodeBeta.isEmpty() || param.hvAnodeGamma.isEmpty() || param.resolvingTimeGamma.isEmpty() || param.resolvintTimeBeta.isEmpty() || param.deadTimeGamma.isEmpty() || param.delayTimeBeta.isEmpty() || param.deadTimeBeta.isEmpty()) {
        QMessageBox::information(this,tr("Empty table"),tr("Please complete tables with parameters or enter '0'"),QMessageBox::Ok);
    }else {
        emit save(param);
        this->accept();
        emit selectedName(param.name);
    }
}

void ParametersDialog::clearTable(QTableWidget *table)
{
    int rows = table->rowCount();
    for(int i=0;i<rows;i++)
        table->removeRow(0);
    table->insertRow(0);
}

void ParametersDialog::addItemTable(QTableWidget *table, QList<float> data)
{
    int rows = table->rowCount();
    for(int i=0;i<rows;i++)
        table->removeRow(0);

    rows = data.count();
    for(int i=0;i<rows;i++)
    {
        table->insertRow(i);
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(data.at(i)));
        table->setItem(i,0,item);
    }
}

void ParametersDialog::addItemTable(QTableWidget *table, QList<int> data)
{
    int rows = table->rowCount();
    for(int i=0;i<rows;i++)
        table->removeRow(0);

    rows = data.count();
    for(int i=0;i<rows;i++)
    {
        table->insertRow(i);
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(data.at(i)));
        table->setItem(i,0,item);
    }
}

QList<float> ParametersDialog::readTableF(QTableWidget *table)
{
    QList<float> ret;
    int rows = table->rowCount();
    for(int i=0;i<rows;i++)
    {
        QTableWidgetItem *item = table->item(i,0);
        if(item!=0)
        {
            if(!item->text().isEmpty())
            {

                ret << item->text().replace(",",".").toFloat();
            }
        }
    }
    return ret;
}

QList<int> ParametersDialog::readTableI(QTableWidget *table)
{
    QList<int> ret;
    int rows = table->rowCount();
    for(int i=0;i<rows;i++)
    {
        QTableWidgetItem *item = table->item(i,0);
        if(item!=0)
        {
            if(!item->text().isEmpty())
            {

                ret << item->text().replace(",",".").toInt();
            }
        }
    }
    return ret;
}
