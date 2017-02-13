#include "addlicensedialog.h"
#include "ui_addlicensedialog.h"

AddLicenseDialog::AddLicenseDialog(LicenseControl * par) :
    parent(par),
    ui(new Ui::AddLicenseDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags = flags & (~helpFlag);
    setWindowFlags(flags);
    ui->DialogBtnBox->buttons()[0]->setEnabled(false);
    ui->DialogBtnBox->buttons()[0]->setText("Добавить");
    ui->DialogBtnBox->buttons()[1]->setText("Закрыть");
    bool connected = parent->connectToDatabase(&db, true, this);
    if(connected){
        getDataFromDatabase();
        parent->disconnectFromDatabase(&db);
    }
}

AddLicenseDialog::~AddLicenseDialog()
{
    delete ui;
}

void AddLicenseDialog::on_UnlimitedCheckBox_stateChanged(int arg1)
{
    if(arg1 == 0)
        ui->DurationSpinBox->setEnabled(true);
    else
        ui->DurationSpinBox->setEnabled(false);
}

void AddLicenseDialog::getDataFromDatabase(){
    QVector<QStringList> retrievedData;
    retrievedData = parent->executeSelectCommand("Vendors", parent->getColumnsNames("Vendors"));
    if(!retrievedData.isEmpty()){
        for(int i = 0; i < retrievedData[0].size(); i++)
            ui->VendorComboBox->addItem(retrievedData[parent->getColumnsNames("Vendors").indexOf("Vendor")][i], retrievedData[parent->getColumnsNames("Vendors").indexOf("ID")][i]);
    }
    retrievedData.clear();
    retrievedData = parent->executeSelectCommand("Types", parent->getColumnsNames("Types"));
    if(!retrievedData.isEmpty()){
        for(int i = 0; i < retrievedData[0].size(); i++)
            ui->TypeComboBox->addItem(retrievedData[parent->getColumnsNames("Types").indexOf("Type")][i], retrievedData[parent->getColumnsNames("Types").indexOf("ID")][i]);
    }
    if(ui->TypeComboBox->count() != 0)
        ui->TypeComboBox->setCurrentIndex(ui->TypeComboBox->count() - 1);
    retrievedData.clear();
}

void AddLicenseDialog::on_VendorComboBox_currentIndexChanged(int index)
{
    bool disconnect = false;
    bool connected = true;
    if(!db.isOpen()){
        disconnect = true;
        connected = parent->connectToDatabase(&db, false, this);
    }
    if(connected){
        ui->SoftComboBox->clear();
        QVector<QStringList> retrievedData = parent->executeSelectCommand("Software", parent->getColumnsNames("Software"), "Vendor="+ui->VendorComboBox->currentData().toString());
        if(!retrievedData.isEmpty()){
            for(int i = 0; i < retrievedData[0].size(); i++)
                ui->SoftComboBox->addItem(retrievedData[parent->getColumnsNames("Software").indexOf("Title")][i], retrievedData[parent->getColumnsNames("Software").indexOf("ID")][i]);
        }
        ui->SoftComboBox->setEnabled(true);
        if(disconnect)
            parent->disconnectFromDatabase(&db);
    }
    if(ui->SoftComboBox->currentText().isEmpty()){
        ui->DialogBtnBox->buttons()[0]->setEnabled(false);
    }
}

void AddLicenseDialog::on_SerialTextEdit_textChanged()
{
    if(!ui->SerialTextEdit->toPlainText().isEmpty()){
        QString temp = ui->SerialTextEdit->toPlainText();
        QStringList tempList = temp.split("\n");
        for(int i = 0; i < tempList.size(); i++)
            if(!tempList[i].isEmpty() && !ui->SoftComboBox->currentText().isEmpty() && !ui->TypeComboBox->currentText().isEmpty()){
                ui->DialogBtnBox->buttons()[0]->setEnabled(true);
                return;
            }
    }
    ui->DialogBtnBox->buttons()[0]->setEnabled(false);
}

void AddLicenseDialog::done(int result_code)
{
   if(QDialog::Accepted == result_code)
   {
       switch(parent->commitChangesDialog()){
       case QMessageBox::Cancel:
           return;
           break;
       case QMessageBox::No:
           QDialog::done(result_code);
           return;
           break;
       case QMessageBox::Yes:
           int count = 0;
           if(commitChanges(&count)){
               QMessageBox::information(this, "Сообщение", "Запрос выполнен успешно. Добавлено лицензий: " + QString::number(count), QMessageBox::Ok);
               return;
           }
           else{
               QMessageBox::critical(this, "Ошибка", "Ошибка при записи данных в базу данных\n" + db.lastError().text(), QMessageBox::Ok);
               return;
           }
           return;
           break;
       }
   }
   else
   {
       QDialog::done(result_code);
       return;
   }
}

bool AddLicenseDialog::commitChanges(int *count){
    bool connected = parent->connectToDatabase(&db, false, this);
    if(connected){
        QStringList columnNames = parent->getColumnsNames("licenses");
        if(!columnNames.isEmpty())
            columnNames.removeFirst();
        else{
            qDebug() << db.lastError().text();
            return false;
        }
        columnNames.removeAll("Username");
        columnNames.removeAll("Password");
        QStringList values;
        QStringList serialList = ui->SerialTextEdit->toPlainText().split("\n");
        serialList.removeAll("");
        int countLocal = 0;
        for(int currentSerial = 0; currentSerial < serialList.size(); currentSerial++){
            values.clear();
            for(int i = 0; i < columnNames.size(); i++){
                if(columnNames[i] == "Software")
                    values.append(ui->SoftComboBox->currentData().toString());
                if(columnNames[i] == "Type")
                    values.append(ui->TypeComboBox->currentData().toString());
                if(columnNames[i] == "Serial"){
                    values.append(serialList[currentSerial]);
                }
                if(columnNames[i] == "Duration"){
                    if(ui->UnlimitedCheckBox->checkState() == Qt::Checked)
                        values.append("0");
                    else
                        values.append(QString::number(ui->DurationSpinBox->value()));
                }
                if(columnNames[i] == "Purchased")
                    values.append("0");
                if(columnNames[i] == "PCCount")
                    values.append(QString::number(ui->CountSpinBox->value()));
            }
            QString selectQueryCondition;
            selectQueryCondition = "Serial='" + serialList[currentSerial] + "' AND Software=" + ui->SoftComboBox->currentData().toString() + " AND Type=" + ui->TypeComboBox->currentData().toString();
            QVector<QStringList> checkForKeys = parent->executeSelectCommand("licenses", parent->getColumnsNames("licenses"), selectQueryCondition);
            bool exist = false;
            if(!checkForKeys.isEmpty()){
                exist = false;
                for(int m = 0; m < checkForKeys[0].size(); m++){
                    exist = true;
                    break;
                }
                if(exist){
                    QMessageBox::information(this, "Ошибка", "Такой серийный номер уже существует: " + serialList[currentSerial], QMessageBox::Ok);
                }
                else{
                    if(!parent->executeInsertCommand("licenses", columnNames, values))
                        return false;
                    else{
                        *count = ++countLocal;
                    }
                }
            }
        }
        parent->disconnectFromDatabase(&db);
        return true;
    }
    else{
        parent->disconnectFromDatabase(&db);
        return false;
    }
}
