#include "newmeasurementdialog.h"
#include "ui_newmeasurementdialog.h"

NewMeasurementDialog::NewMeasurementDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewMeasurementDialog)
{
    ui->setupUi(this);
    this->setModal(true);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setSizeGripEnabled(true);

    //seriesDialog = SeriesSourcesDialog(this);
    //paramDialog = ParametersDialog(this);

    connect(ui->seriesSources_pushButton,SIGNAL(clicked(bool)),&seriesDialog,SLOT(exec()));
    connect(ui->parameters_pushButton,SIGNAL(clicked(bool)),&paramDialog,SLOT(exec()));

    connect(ui->cancelPushButton,SIGNAL(clicked(bool)),this,SLOT(reject()));
    connect(ui->startPushButton,SIGNAL(clicked(bool)),this,SLOT(clickedStart()));
    connect(ui->nuclide_lineEdit,SIGNAL(textChanged(QString)),this,SIGNAL(changedNuclide(QString)));

    //SeriesSourceDialog
    connect(ui->seriesSources_comboBox,SIGNAL(currentIndexChanged(QString)),&seriesDialog,SLOT(setName(QString)));
    connect(&seriesDialog,SIGNAL(selectedName(QString)),this,SLOT(changedNameSeriesSources(QString)));
    connect(&seriesDialog,SIGNAL(remove(QString)),this,SLOT(removeNameSeriesSources(QString)));

    //parametersDialog
    connect(ui->parameters_comboBox,SIGNAL(currentIndexChanged(QString)),&paramDialog,SLOT(setName(QString)));
    connect(&paramDialog,SIGNAL(selectedName(QString)),this,SLOT(changedNameParameters(QString)));
    connect(&paramDialog,SIGNAL(remove(QString)),this,SLOT(removeNameParameters(QString)));
}

NewMeasurementDialog::~NewMeasurementDialog()
{
    delete ui;
}
void NewMeasurementDialog::mousePressEvent(QMouseEvent *event)
{
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void NewMeasurementDialog::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}
void NewMeasurementDialog::setSettings(NEWMEASUREMENTDIALOGSETTINGS settings)
{
    QString currText;
    currText = ui->mode_comboBox->currentText();
    ui->mode_comboBox->clear();
    foreach (QString var, settings.modeList) {
        ui->mode_comboBox->addItem(QCoreApplication::translate("MainWindow",var.toStdString().c_str()));

    }
    ui->mode_comboBox->setCurrentIndex(ui->mode_comboBox->findText(currText));

    ui->seriesSources_comboBox->blockSignals(true);
    currText = ui->seriesSources_comboBox->currentText();
    ui->seriesSources_comboBox->clear();
    ui->seriesSources_comboBox->addItems(settings.nameSeriesSourcesList);
    ui->seriesSources_comboBox->setCurrentIndex(ui->seriesSources_comboBox->findText(currText));
    ui->seriesSources_comboBox->blockSignals(false);

    ui->parameters_comboBox->blockSignals(true);
    currText = ui->parameters_comboBox->currentText();
    ui->parameters_comboBox->clear();
    ui->parameters_comboBox->addItems(settings.nameParametersList);
    ui->parameters_comboBox->setCurrentIndex(ui->parameters_comboBox->findText(currText));
    ui->parameters_comboBox->blockSignals(false);

    QCompleter *completer = new QCompleter(settings.completerNuclideList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->nuclide_lineEdit->setCompleter(completer);

    completer = new QCompleter(settings.completerUserList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->madeBy_lineEdit->setCompleter(completer);

    if(this->isHidden()) {
        ui->company_lineEdit->setText(settings.company);
        ui->counter_lineEdit->setText(settings.counter);
        ui->madeBy_lineEdit->setText(settings.user);
        ui->bg_spinBox->setValue(settings.defaultBackgroundTime);
        ui->so_spinBox->setValue(settings.defaultSourceTime);
        ui->reps_spinBox->setValue(settings.defaultReps);
        ui->nuclide_lineEdit->setFocus();
    }
}

void NewMeasurementDialog::clickedStart()
{
    MEASUREMENTDATA data;
    data.company = ui->company_lineEdit->text();
    data.counter = ui->counter_lineEdit->text();
    data.nuclide = ui->nuclide_lineEdit->text();
    data.mode = ui->mode_comboBox->currentIndex();
    data.nameSeriesSources = ui->seriesSources_comboBox->currentText();
    data.nameParameters = ui->parameters_comboBox->currentText();
    data.backgroundTime = ui->bg_spinBox->value();
    data.sourceTime = ui->so_spinBox->value();
    data.reps = ui->reps_spinBox->value();
    data.comment = ui->comment_textEdit->toPlainText().replace(QRegExp("[\\n]")," ");

    data.madeBy = ui->madeBy_lineEdit->text();

    if(data.nuclide.isEmpty()) {
        QMessageBox::information(this,tr("Missing nuclide"),tr("Please entry measurement nuclide"),QMessageBox::Ok);
        return;
    }
    if(data.nameSeriesSources.isEmpty()) {
        QMessageBox::information(this,tr("Sources series"),tr("Please select or entry measurment sources series"),QMessageBox::Ok);
        return;
    }
    if(data.nameParameters.isEmpty()) {
        QMessageBox::information(this,tr("Parameters"),tr("Please select or entry measurment parameters"),QMessageBox::Ok);
        return;
    }
    this->accept();
    emit startMeasurement(data);
}

void NewMeasurementDialog::setData(MEASUREMENTDATA data)
{
    ui->company_lineEdit->setText(data.company);
    ui->counter_lineEdit->setText(data.counter);
    ui->nuclide_lineEdit->setText(data.nuclide);
    ui->mode_comboBox->setCurrentIndex(data.mode);
    ui->seriesSources_comboBox->setCurrentIndex(ui->seriesSources_comboBox->findText(data.nameSeriesSources));
    ui->parameters_comboBox->setCurrentIndex(ui->parameters_comboBox->findText(data.nameParameters));
    ui->bg_spinBox->setValue(data.backgroundTime);
    ui->so_spinBox->setValue(data.sourceTime);
    ui->reps_spinBox->setValue(data.reps);
    ui->comment_textEdit->setPlainText(data.comment);
    ui->nuclide_lineEdit->setFocus();
}

void NewMeasurementDialog::changedNameSeriesSources(QString name)
{
    ui->seriesSources_comboBox->blockSignals(true);
    int index = ui->seriesSources_comboBox->findText(name);
    if(index != -1) {
        ui->seriesSources_comboBox->setCurrentIndex(index);
    }else {
        ui->seriesSources_comboBox->addItem(name);
        ui->seriesSources_comboBox->setCurrentIndex(ui->seriesSources_comboBox->findText(name));
    }
    ui->seriesSources_comboBox->blockSignals(false);
}

void NewMeasurementDialog::removeNameSeriesSources(QString name)
{
    ui->seriesSources_comboBox->removeItem(ui->seriesSources_comboBox->findText(name));
}

void NewMeasurementDialog::changedNameParameters(QString name)
{
    ui->parameters_comboBox->blockSignals(true);
    int index = ui->parameters_comboBox->findText(name);
    if(index != -1) {
        ui->parameters_comboBox->setCurrentIndex(index);
    }else {
        ui->parameters_comboBox->addItem(name);
        ui->parameters_comboBox->setCurrentIndex(ui->parameters_comboBox->findText(name));
    }
    ui->parameters_comboBox->blockSignals(false);
}

void NewMeasurementDialog::removeNameParameters(QString name)
{
    ui->parameters_comboBox->removeItem(ui->parameters_comboBox->findText(name));
}
