#include "changelicensecountdialog.h"
#include "ui_changelicensecountdialog.h"

ChangeLicenseCountDialog::ChangeLicenseCountDialog(LicenseControl *par) :
    ui(new Ui::ChangeLicenseCountDialog),
    parent(par)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags = flags & (~helpFlag);
    setWindowFlags(flags);
    getData();
}

ChangeLicenseCountDialog::~ChangeLicenseCountDialog()
{
    delete ui;
}

void ChangeLicenseCountDialog::on_toolButton_clicked()
{

    QSpinBox * spinBox = new QSpinBox();
    spinBox->setMinimum(0);
    spinBox->setMaximum(32767);

    QComboBox * comboBox = new QComboBox();
    bool connected = parent->connectToDatabase(&db, false, this);
    QVector<QStringList> retrievedData;
    if(connected){
        retrievedData = parent->executeSelectCommand("Software", parent->getColumnsNames("Software"));
        if(!retrievedData.isEmpty()){
            for(int i = 0; i < retrievedData[0].size(); i++){
                QVector<QStringList> vendor = parent->executeSelectCommand("vendors", parent->getColumnsNames("vendors"), "ID=" + retrievedData[parent->getColumnsNames("software").indexOf("Vendor")][i]);
                if(!vendor.isEmpty()){
                    comboBox->addItem(vendor[parent->getColumnsNames("vendors").indexOf("Vendor")][0] + " " + retrievedData[parent->getColumnsNames("Software").indexOf("Title")][i], retrievedData[parent->getColumnsNames("Software").indexOf("ID")][i]);
                }
            }
        }
        if(comboBox->count() != 0){
            ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
            ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 1, spinBox);
            ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 0, comboBox);
        }
        else
            QMessageBox::information(this, "Ошибка", "Нет ПО в базе", QMessageBox::Ok);
        parent->disconnectFromDatabase(&db);
    }
}

void ChangeLicenseCountDialog::done(int r){
    if(QDialog::Accepted == r)
    {
        if(isCorrect()){
            commitChanges();
            QDialog::done(r);
            return;
        }
        else{
            QMessageBox::critical(this, "Ошибка", "В таблице найдены повторяющиеся данные. Исправьте.", QMessageBox::Ok);
            return;
        }
    }
    else
    {
        QDialog::done(r);
        return;
    }
}

bool ChangeLicenseCountDialog::isCorrect(){
    QStringList soft;
    for(int i = 0; i < ui->tableWidget->rowCount(); i++){
        QComboBox * comboBox = (QComboBox*)ui->tableWidget->cellWidget(i, 0);
        if(soft.indexOf(comboBox->currentData().toString()) != -1){
            return false;
        }
        else
            soft.append(comboBox->currentData().toString());
    }
    return true;
}

void ChangeLicenseCountDialog::commitChanges(){
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;
    settings.beginGroup("License_Count");
    parent->minLicenseCount.clear();
    for(int i = 0; i < ui->tableWidget->rowCount(); i++){
        QComboBox * comboBox = (QComboBox*)ui->tableWidget->cellWidget(i, 0);
        QSpinBox * spinBox = (QSpinBox*)ui->tableWidget->cellWidget(i, 1);
        parent->minLicenseCount.resize(parent->minLicenseCount.size() + 1);
        parent->minLicenseCount[i].soft = comboBox->currentData().toInt();
        parent->minLicenseCount[i].count = spinBox->value();
        settings.setValue(QString::number(parent->minLicenseCount[i].soft), parent->minLicenseCount[i].count);
    }
    settings.endGroup();
    parent->defaultMinLicenseCount = ui->defaultSpinBox->value();
    settings.beginGroup("License_Notification");
    settings.setValue("default", parent->defaultMinLicenseCount);
    settings.endGroup();
    parent->checkLicense(true);
}

void ChangeLicenseCountDialog::getData(){
    QSpinBox * spinBox;
    QComboBox * comboBox;
    for(int i = 0; i < parent->minLicenseCount.size(); i++){
        spinBox = new QSpinBox();
        spinBox->setMinimum(0);
        spinBox->setMaximum(32767);
        comboBox = new QComboBox();
        spinBox->setValue(parent->minLicenseCount[i].count);
        bool connected = parent->connectToDatabase(&db, true, this);
        QVector<QStringList> retrievedData;
        if(connected){
            retrievedData = parent->executeSelectCommand("Software", parent->getColumnsNames("Software"));
            if(!retrievedData.isEmpty()){
                for(int k = 0; k < retrievedData[0].size(); k++){
                    comboBox->addItem(retrievedData[parent->getColumnsNames("Software").indexOf("Title")][k], retrievedData[parent->getColumnsNames("Software").indexOf("ID")][k]);
                }
            }
            parent->disconnectFromDatabase(&db);
        }
        for(int j = 0; j < comboBox->count(); j++){
            if(comboBox->itemData(j).toInt() == parent->minLicenseCount[i].soft){
                comboBox->setCurrentIndex(j);
                break;
            }
        }
        ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
        ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 1, spinBox);
        ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 0, comboBox);
    }
    ui->defaultSpinBox->setValue(parent->defaultMinLicenseCount);
}
