#include "seriessourcesdialog.h"
#include "ui_seriessourcesdialog.h"

SeriesSourcesDialog::SeriesSourcesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SeriesSourcesDialog)
{
    ui->setupUi(this);
    this->setModal(true);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setSizeGripEnabled(true);

    connect(ui->name_comboBox,SIGNAL(currentIndexChanged(QString)),this,SIGNAL(changedName(QString)));
    connect(ui->masses_tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChanged(int,int)));
    connect(ui->edit_pushButton,SIGNAL(clicked(bool)),this,SLOT(clickedEdit()));
    connect(ui->add_pushButton,SIGNAL(clicked(bool)),this,SLOT(clickedAdd()));
    connect(ui->remove_pushButton,SIGNAL(clicked(bool)),this,SLOT(clickedRemove()));
    connect(ui->savePushButton,SIGNAL(clicked(bool)),this,SLOT(clickedSave()));
    connect(ui->cancelPushButton,SIGNAL(clicked(bool)),this,SLOT(reject()));
}

SeriesSourcesDialog::~SeriesSourcesDialog()
{
    delete ui;
}
void SeriesSourcesDialog::mousePressEvent(QMouseEvent *event)
{
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void SeriesSourcesDialog::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}
void SeriesSourcesDialog::setSettings(SERIESSOURCESDIALOGSETTINGS settings)
{
    ui->name_comboBox->blockSignals(true);
    QString currText = ui->name_comboBox->currentText();
    ui->name_comboBox->clear();
    ui->name_comboBox->addItems(settings.nameSeriesSourcesList);
    ui->name_comboBox->setCurrentIndex(ui->name_comboBox->findText(currText));
    ui->name_comboBox->blockSignals(false);
    if(ui->name_comboBox->currentIndex() == -1) {
        ui->nuclide_lineEdit->clear();
        ui->solution_lineEdit->clear();
        ui->dilution_doubleSpinBox->setValue(1);
        ui->motherSolution_lineEdit->clear();
        ui->scintillatorName_lineEdit->clear();
        ui->scintillatorVolume_lineEdit->clear();
        ui->vialType_lineEdit->clear();
        ui->preparedBy_lineEdit->clear();
        ui->preparedAt_dateEdit->setDate(QDate::currentDate());

        int rows = ui->masses_tableWidget->rowCount();
        for(int i=0;i<rows;i++)
            ui->masses_tableWidget->removeRow(0);
        ui->masses_tableWidget->insertRow(0);
        ui->nuclide_lineEdit->setFocus();
    }

    QCompleter *completer = new QCompleter(settings.completerNuclideList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->nuclide_lineEdit->setCompleter(completer);

    completer = new QCompleter(settings.completerSolutionList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->solution_lineEdit->setCompleter(completer);

    completer = new QCompleter(settings.completerSolutionList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->motherSolution_lineEdit->setCompleter(completer);

    completer = new QCompleter(settings.completerNameScintillatorList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->scintillatorName_lineEdit->setCompleter(completer);

    completer = new QCompleter(settings.completerVolumeScintillatorList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->scintillatorVolume_lineEdit->setCompleter(completer);

    completer = new QCompleter(settings.completerVialTypeList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->vialType_lineEdit->setCompleter(completer);

    completer = new QCompleter(settings.completerUserList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->preparedBy_lineEdit->setCompleter(completer);

}

void SeriesSourcesDialog::setName(QString name)
{
    ui->name_comboBox->setCurrentIndex(ui->name_comboBox->findText(name));
}

void SeriesSourcesDialog::setData(SERIESSOURCESDATA data)
{
    if(data.name.isEmpty()) {
        ui->nuclide_lineEdit->clear();
        ui->solution_lineEdit->clear();
        ui->dilution_doubleSpinBox->setValue(1);
        ui->motherSolution_lineEdit->clear();
        ui->scintillatorName_lineEdit->clear();
        ui->scintillatorVolume_lineEdit->clear();
        ui->vialType_lineEdit->clear();
        ui->preparedBy_lineEdit->clear();
        ui->preparedAt_dateEdit->setDate(QDate::currentDate());

        int rows = ui->masses_tableWidget->rowCount();
        for(int i=0;i<rows;i++)
            ui->masses_tableWidget->removeRow(0);
        ui->masses_tableWidget->insertRow(0);
        ui->nuclide_lineEdit->setFocus();
    }else {
        ui->nuclide_lineEdit->setText(data.nuclide);
        ui->solution_lineEdit->setText(data.solution);
        ui->dilution_doubleSpinBox->setValue(data.dilution);
        ui->motherSolution_lineEdit->setText(data.motherSolution);
        ui->scintillatorName_lineEdit->setText(data.nameScintillator);
        ui->scintillatorVolume_lineEdit->setText(data.volumeScintillator);
        ui->vialType_lineEdit->setText(data.vialType);
        ui->preparedBy_lineEdit->setText(data.preparedBy);
        ui->preparedAt_dateEdit->setDate(data.preparedAt);

        int rows = ui->masses_tableWidget->rowCount();
        for(int i=0;i<rows;i++)
            ui->masses_tableWidget->removeRow(0);

        rows = data.masses.count();
        for(int i=0;i<rows;i++){
            ui->masses_tableWidget->insertRow(i);
            ui->masses_tableWidget->setItem(i,0, new QTableWidgetItem(data.idVials.at(i)));
            ui->masses_tableWidget->setItem(i,1, new QTableWidgetItem(QString::number(data.masses.at(i))));
        }
        ui->masses_tableWidget->resizeColumnsToContents();
    }
}


void SeriesSourcesDialog::cellChanged(int row, int column)
{
    QTableWidget *table = ui->masses_tableWidget;
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

void SeriesSourcesDialog::clickedEdit()
{
    ui->name_comboBox->blockSignals(true);
    ui->edit_pushButton->setChecked(true);
    ui->name_comboBox->setEditable(true);
    ui->name_comboBox->setFocus();
    connect(ui->name_comboBox->lineEdit(),SIGNAL(returnPressed()),this,SLOT(finishedEdit()));
    connect(ui->name_comboBox->lineEdit(),SIGNAL(editingFinished()),this,SLOT(finishedEdit()));
}

void SeriesSourcesDialog::finishedEdit()
{
    ui->edit_pushButton->setChecked(false);
    ui->name_comboBox->setEditable(false);
    ui->name_comboBox->blockSignals(false);
}

void SeriesSourcesDialog::clickedAdd()
{
    bool ok;
    QString name = QInputDialog::getText(this,tr("A new sources series"),tr("Name:"),QLineEdit::Normal,QString(),&ok);

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
            ui->solution_lineEdit->clear();
            ui->dilution_doubleSpinBox->setValue(1);
            ui->motherSolution_lineEdit->clear();
            ui->scintillatorName_lineEdit->clear();
            ui->scintillatorVolume_lineEdit->clear();
            ui->vialType_lineEdit->clear();
            ui->preparedBy_lineEdit->clear();
            ui->preparedAt_dateEdit->setDate(QDate::currentDate());

            int rows = ui->masses_tableWidget->rowCount();
            for(int i=0;i<rows;i++)
                ui->masses_tableWidget->removeRow(0);
            ui->masses_tableWidget->insertRow(0);
            ui->nuclide_lineEdit->setFocus();

        }
    }
}

void SeriesSourcesDialog::clickedRemove()
{
    if(QMessageBox::question(this,tr("Remove"),tr("Are you sure you want to remove?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes) {
        emit remove(ui->name_comboBox->currentText());
        ui->name_comboBox->removeItem(ui->name_comboBox->currentIndex());
    }
}

void SeriesSourcesDialog::clickedSave()
{
    SERIESSOURCESDATA series;
    series.name = ui->name_comboBox->currentText();
    series.nuclide = ui->nuclide_lineEdit->text();
    series.solution = ui->solution_lineEdit->text();
    series.dilution = ui->dilution_doubleSpinBox->value();
    series.motherSolution = ui->motherSolution_lineEdit->text();
    series.nameScintillator = ui->scintillatorName_lineEdit->text();
    series.volumeScintillator = ui->scintillatorVolume_lineEdit->text();
    series.vialType = ui->vialType_lineEdit->text();
    series.preparedBy = ui->preparedBy_lineEdit->text();
    series.preparedAt = ui->preparedAt_dateEdit->date();

    int rows = ui->masses_tableWidget->rowCount();
    for(int i=0;i<rows;i++) {
        QTableWidgetItem *id = ui->masses_tableWidget->item(i,0);
        QTableWidgetItem *mass = ui->masses_tableWidget->item(i,1);

        if(id!=0 && mass!=0)
        {
            if(!id->text().isEmpty() && !mass->text().isEmpty())
            {

                double value = mass->text().replace(",",".").toDouble();
                if(value>0)
                {
                    series.idVials << id->text();
                    series.masses << value;
                }
            }
        }
    }
    if(series.idVials.count() == 0) {
        QMessageBox::information(this,tr("Missing source"),tr("Please specify at least one source"),QMessageBox::Ok);
    }else {
        emit save(series);
        this->accept();
        emit selectedName(series.name);
    }

}
