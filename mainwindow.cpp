#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setStatusBar(new QStatusBar(this));

    connect(ui->searchLineEdit,SIGNAL(textChanged(QString)),this,SIGNAL(searchMeasurement(QString)));
    connect(ui->settingsPushButton,SIGNAL(clicked(bool)),&settingsDialog,SLOT(exec()));
    connect(ui->exitPushButton,SIGNAL(clicked(bool)),this,SLOT(close()));
    connect(ui->hidePushButton,SIGNAL(clicked(bool)),this,SLOT(clickedHideButton()));
    connect(ui->maximizePushButton,SIGNAL(clicked(bool)),this,SLOT(clickedMaximizeButton()));
    connect(ui->newPushButton,SIGNAL(clicked(bool)),&newMeasDialog,SLOT(exec()));
    connect(ui->monitorPushButton,SIGNAL(clicked(bool)),this,SIGNAL(clickedMonitorButton()));
    connect(ui->registerTableWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showContextMenu(QPoint)));
    connect(ui->lastStandaTableWidget,SIGNAL(itemSelectionChanged()),this,SLOT(selectionChangedLastStandaTableWidget()));
}

MainWindow::~MainWindow()
{
    emit positionWindow(this->pos());
    emit sizeWindow(this->size());
    emit maximizedWindow(this->isMaximized());
    delete ui;
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(this->isMaximized()) {
        this->showNormal();
        ui->maximizePushButton->setChecked(false);
    }
    this->move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}

void MainWindow::setSettingsMainWindow(MAINWINDOWSETTINGS settings)
{
    ui->titleLabel->setText(settings.title);
    this->setWindowTitle(settings.title);


    if(settings.maximized) {
        this->showMaximized();
        ui->maximizePushButton->setChecked(true);
    }else {
        this->resize(settings.size);
        this->move(settings.position);
        ui->maximizePushButton->setChecked(false);
    }

    int columns = settings.headersMeasurementRegisterTable.count();
    if(columns > ui->registerTableWidget->columnCount()) {
        for(int j=ui->registerTableWidget->columnCount();j<columns;j++)
            ui->registerTableWidget->insertColumn(j);
    }else if(columns < ui->registerTableWidget->columnCount()) {
        for(int j=ui->registerTableWidget->columnCount()-1; j>=columns; j--)
            ui->registerTableWidget->removeColumn(j);
    }
    QStringList tmp;
    foreach (QString var, settings.headersMeasurementRegisterTable) {
        tmp << tr(var.toStdString().c_str());
    }
    ui->registerTableWidget->setHorizontalHeaderLabels(tmp);

    columns = settings.headersLastStandardizationTable.count();
    if(columns > ui->lastStandaTableWidget->columnCount()) {
        for(int j=ui->lastStandaTableWidget->columnCount();j<columns;j++)
            ui->lastStandaTableWidget->insertColumn(j);
    }else if(columns < ui->lastStandaTableWidget->columnCount()) {
        for(int j=ui->lastStandaTableWidget->columnCount()-1; j>=columns; j--)
            ui->lastStandaTableWidget->removeColumn(j);
    }
    tmp.clear();
    foreach (QString var, settings.headersLastStandardizationTable) {
        tmp << tr(var.toStdString().c_str());
    }
    ui->lastStandaTableWidget->setHorizontalHeaderLabels(tmp);

    QHeaderView *headerView = ui->lastStandaTableWidget->horizontalHeader();
    headerView->setSectionResizeMode(0,QHeaderView::Stretch);
    headerView->setSectionResizeMode(1,QHeaderView::Stretch);

    QCompleter *completer = new QCompleter(settings.completerMeasurementSearch,this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->searchLineEdit->setCompleter(completer);
}

void MainWindow::setDisplayDateTime(DISPLAYDATETIME dateTime)
{
    ui->curreTimeLabel->setText(dateTime.currentTime);
    ui->currDateLabel->setText(dateTime.currentDate);
    ui->zoneTimeLabel->setText(dateTime.timeZone);
}

void MainWindow::setDisplayMeasurementStatistics(DISPLAYSTATISTICS statistics)
{
    ui->measurementCountLabel->setText(statistics.measurementCount);
    ui->solutionCountLabel->setText(statistics.solutionCount);
    ui->nuclideCountLabel->setText(statistics.nuclideCount);
    ui->totalTimeCountLabel->setText(statistics.totalTimeCount);
}

void MainWindow::setMeasurementRegisterTable(QList<QStringList> records)
{
    int rows = ui->registerTableWidget->rowCount();
    for(int i=0;i<rows;i++)
        ui->registerTableWidget->removeRow(0);

    rows = records.count();
    QList<QStringList> deletedRecords;
    for(int i=0;i<rows;i++) {
        QStringList record = records.at(i);
        if(record.at(M_STATUS).toInt() == STATUS_DELETED) {
            deletedRecords << record;
        }else {
            int row = i-deletedRecords.count();
            ui->registerTableWidget->insertRow(row);
            for(int j=0;j<record.count();j++) {
                if(j == M_ID || j == M_BACKGROUNDTIME || j == M_SOURCETIME || j == M_REPS) {
                    TableWidgetNumberItem *item = new TableWidgetNumberItem(record.at(j));
                    item->setTextAlignment(Qt::AlignCenter);
                    QFont font;
                    if(record.at(M_STATUS).toInt() != STATUS_FINISHED) {
                       item->setForeground(QColor::fromRgb(150,150,150)); //color
                       font.setItalic(true);
                    }else {
                       font.setBold(false);
                    }
                    item->setFont(font);
                    ui->registerTableWidget->setItem(row,j,item);
                }else {
                    QTableWidgetItem *item;
                    if(j == M_MODE)
                        item =  new QTableWidgetItem(tr(modeMeasurement.at(record.at(j).toInt()).toStdString().c_str()));
                    else if(j == M_STATUS)
                        item = new QTableWidgetItem(tr(statusMeasurement.at(record.at(j).toInt()).toStdString().c_str()));
                    else
                        item = new QTableWidgetItem(record.at(j));

                    item->setTextAlignment(Qt::AlignCenter);
                    QFont font;
                    if(record.at(M_STATUS).toInt() != STATUS_FINISHED) {
                       item->setForeground(QColor::fromRgb(150,150,150)); //color
                       font.setItalic(true);
                    }else {
                       font.setBold(false);
                    }
                    item->setFont(font);
                    ui->registerTableWidget->setItem(row,j,item);
                }

            }
        }
    }
    //deleted reports
    rows = ui->registerTableWidget->rowCount();
    int rowsDeleted = deletedRecords.count();

    for(int i=rows;i<rows+rowsDeleted;i++) {
        QStringList record = deletedRecords.at(i-rows);
        ui->registerTableWidget->insertRow(i);
        for(int j=0;j<record.count();j++) {
            if(j == M_ID || j == M_BACKGROUNDTIME || j == M_SOURCETIME || j == M_REPS) {
                TableWidgetNumberItem *item = new TableWidgetNumberItem(record.at(j));
                item->setTextAlignment(Qt::AlignCenter);
                if(record.at(M_STATUS).toInt() != STATUS_FINISHED) {
                   item->setForeground(QColor::fromRgb(255,0,0)); //color
                   QFont font;
                   font.setItalic(true);
                   //font.setStrikeOut(true);
                   item->setFont(font);
                }
                ui->registerTableWidget->setItem(i,j,item);
            }else {
                QTableWidgetItem *item;
                if(j == M_MODE)
                    item =  new QTableWidgetItem(tr(modeMeasurement.at(record.at(j).toInt()).toStdString().c_str()));
                else if(j == M_STATUS)
                    item = new QTableWidgetItem(tr(statusMeasurement.at(record.at(j).toInt()).toStdString().c_str()));
                else
                    item = new QTableWidgetItem(record.at(j));

                item->setTextAlignment(Qt::AlignCenter);
                if(record.at(M_STATUS).toInt() != STATUS_FINISHED) {
                   item->setForeground(QColor::fromRgb(255,0,0)); //color
                   QFont font;
                   font.setItalic(true);
                   //font.setStrikeOut(true);
                   item->setFont(font);
                }
                    ui->registerTableWidget->setItem(i,j,item);
            }

        }


    }
    ui->registerTableWidget->resizeColumnsToContents();
    ui->registerTableWidget->resizeRowsToContents();
}

void MainWindow::setLastStandardizationTable(QList<QStringList> records)
{
    int rows = ui->lastStandaTableWidget->rowCount();
    for(int i=0;i<rows;i++)
        ui->lastStandaTableWidget->removeRow(0);

    rows = records.count();
    for(int i=0;i<rows;i++) {
        ui->lastStandaTableWidget->insertRow(i);
        QStringList record = records.at(i);
        for(int j=0;j<record.count();j++) {
            QTableWidgetItem *item = new QTableWidgetItem(record.at(j));
            item->setTextAlignment(Qt::AlignCenter);
            ui->lastStandaTableWidget->setItem(i,j,item);
        }
    }
    ui->lastStandaTableWidget->resizeColumnsToContents();
    ui->lastStandaTableWidget->resizeRowsToContents();
    ui->lastStandaTableWidget->sortItems(0);
}

void MainWindow::showNewMeasurementDialog(MEASUREMENTDATA data)
{
    newMeasDialog.setData(data);
    newMeasDialog.exec();
}

void MainWindow::showMeasurementDialog(MEASUREMENTDIALOGSETTINGS settings)
{
    measDialog.clear();
    measDialog.set(settings);
    measDialog.exec();
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    if(ui->registerTableWidget->selectedItems().count()<1)
        return;

    QMenu myMenu;
    if(ui->registerTableWidget->selectedItems().count() == 1) {
        myMenu.addAction(tr("Open report location"));
        myMenu.addAction(tr("Repeat measurement"));
        myMenu.addAction(tr("Delete"));
    }else {
        myMenu.addAction(tr("Delete ")+QString::number(ui->registerTableWidget->selectedItems().count())+tr(" reports"));
    }

    QPoint globalPos = ui->registerTableWidget->mapToGlobal(pos);
    QAction* selectedItem = myMenu.exec(globalPos);
    if(selectedItem) {
        QString textAction = selectedItem->text();

        if(textAction==tr("Delete") || textAction==tr("Delete ")+QString::number(ui->registerTableWidget->selectedItems().count())+tr(" reports")) {
            QStringList ids;
            foreach(QTableWidgetItem *item, ui->registerTableWidget->selectedItems()) {
                ids << ui->registerTableWidget->item(item->row(),0)->text();
            }
            if(ids.count()>1) {
                if(QMessageBox::question(this,tr("Delete reports"),tr("Are you sure you want to delete the selected reports?"),QMessageBox::YesAll,QMessageBox::No) == QMessageBox::YesAll)
                    emit deleteReport(ids);
            }else {
                if(QMessageBox::question(this,tr("Delete report"),tr("Are you sure you want to delete this report?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
                    emit deleteReport(ids);
            }
        }else if(textAction==tr("Repeat measurement"))
        {
           emit repeatMeasurement(ui->registerTableWidget->item(ui->registerTableWidget->selectedItems().first()->row(),0)->text());

        }else if(textAction==tr("Open report location"))
        {
           emit openReportLocation(ui->registerTableWidget->item(ui->registerTableWidget->selectedItems().first()->row(),0)->text());
        }

    }
}

void MainWindow::selectionChangedLastStandaTableWidget()
{

}

void MainWindow::clickedHideButton()
{
    this->showMinimized();
}

void MainWindow::clickedMaximizeButton()
{
    if(this->isMaximized()) {
        this->showNormal();
        ui->maximizePushButton->setChecked(false);
    }else {
        this->showMaximized();
        ui->maximizePushButton->setChecked(true);
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if(event->type() == QEvent::ActivationChange) {
        if(this->isActiveWindow()) {
            this->setWindowOpacity(1);
        }else{
            this->setWindowOpacity(0.7);
        }
    }
}
