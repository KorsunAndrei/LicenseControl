#include "selldialog.h"
#include "ui_selldialog.h"

SellDialog::SellDialog(LicenseControl *par) :
    parent(par),
    ui(new Ui::SellDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags = flags & (~helpFlag);
    setWindowFlags(flags);
    ui->ActivationDateSpinBox->setDate(QDate::currentDate());
    ui->ExpirationDateSpinBox->setDate(QDate::currentDate().addMonths(ui->DurationSpinBox->value()));
    ui->DialogBtnBox->buttons()[0]->setEnabled(false);
    ui->printBtn->setEnabled(false);
    QRegExp costRegExp("[1-9][0-9]+");
    QRegExpValidator * costValidator = new QRegExpValidator(costRegExp);
    ui->CostEdit->setValidator(costValidator);
    bool connected = false;
    connected = parent->connectToDatabase(&db, true, this);
    if(connected){
        getDataFromDatabase();
        parent->disconnectFromDatabase(&db);
    }
}

SellDialog::~SellDialog()
{
    delete ui;
}
void SellDialog::on_UnlimitedCheckBox_stateChanged(int arg1)
{
    if(arg1 == 0){
        ui->DurationSpinBox->setEnabled(true);
        ui->ExpirationDateSpinBox->setEnabled(true);
    }
    else{
        ui->DurationSpinBox->setEnabled(false);
        ui->ExpirationDateSpinBox->setEnabled(false);
    }
    showSerials();
}

void SellDialog::on_AddCustomerBtn_clicked()
{
    NewCustomerDialog NCDialog(parent);
    NCDialog.setModal(true);
    if(NCDialog.exec() == 1){
        bool connected = true;
        connected = parent->connectToDatabase(&db, false, this);
        if(connected){
            QVector<QStringList> retrievedData = parent->executeSelectCommand("Customers", parent->getColumnsNames("Customers"));
            if(!retrievedData.isEmpty()){
                ui->CustomerComboBox->clear();
                for(int i = 0; i < retrievedData[0].size(); i++)
                    ui->CustomerComboBox->addItem(retrievedData[parent->getColumnsNames("Customers").indexOf("Surname")][i] + " " + retrievedData[parent->getColumnsNames("Customers").indexOf("Name")][i] + " " + retrievedData[parent->getColumnsNames("Customers").indexOf("Patronomic")][i] + (retrievedData[parent->getColumnsNames("Customers").indexOf("Address")][i].isEmpty() ? "" : ", " + retrievedData[parent->getColumnsNames("Customers").indexOf("Address")][i]), retrievedData[parent->getColumnsNames("Customers").indexOf("ID")][i]);
            }
        }
        parent->disconnectFromDatabase(&db);
        if(ui->CustomerComboBox->count() != 0)
            ui->CustomerComboBox->setCurrentIndex(ui->CustomerComboBox->count() - 1);
    }
}

void SellDialog::getDataFromDatabase(){
    QVector<QStringList> retrievedData;
    retrievedData = parent->executeSelectCommand("Vendors", parent->getColumnsNames("Vendors"));
    if(!retrievedData.isEmpty()){
        for(int i = 0; i < retrievedData[0].size(); i++)
            ui->VendorComboBox->addItem(retrievedData[parent->getColumnsNames("Vendors").indexOf("Vendor")][i], retrievedData[parent->getColumnsNames("Software").indexOf("ID")][i]);
    }
    retrievedData.clear();
    retrievedData = parent->executeSelectCommand("Types", parent->getColumnsNames("Types"));
    if(!retrievedData.isEmpty()){
        for(int i = 0; i < retrievedData[0].size(); i++)
            ui->TypeComboBox->addItem(retrievedData[parent->getColumnsNames("Types").indexOf("Type")][i], retrievedData[parent->getColumnsNames("Software").indexOf("ID")][i]);
    }
    retrievedData.clear();
    retrievedData = parent->executeSelectCommand("Engineers", parent->getColumnsNames("Engineers"));
    if(!retrievedData.isEmpty()){
        for(int i = 0; i < retrievedData[0].size(); i++)
            ui->EngineerComboBox->addItem(retrievedData[parent->getColumnsNames("Engineers").indexOf("Surname")][i] + " " + retrievedData[parent->getColumnsNames("Engineers").indexOf("Name")][i] + " " + retrievedData[parent->getColumnsNames("Engineers").indexOf("Patronomic")][i], retrievedData[parent->getColumnsNames("Engineers").indexOf("ID")][i]);
    }
    retrievedData.clear();
    retrievedData = parent->executeSelectCommand("Customers", parent->getColumnsNames("Customers"));
    if(!retrievedData.isEmpty()){
        for(int i = 0; i < retrievedData[0].size(); i++)
            ui->CustomerComboBox->addItem(retrievedData[parent->getColumnsNames("Customers").indexOf("Surname")][i] + " " + retrievedData[parent->getColumnsNames("Customers").indexOf("Name")][i] + " " + retrievedData[parent->getColumnsNames("Customers").indexOf("Patronomic")][i] + (retrievedData[parent->getColumnsNames("Customers").indexOf("Address")][i].isEmpty() ? "" : ", " + retrievedData[parent->getColumnsNames("Customers").indexOf("Address")][i]), retrievedData[parent->getColumnsNames("Customers").indexOf("ID")][i]);
    }
    if(ui->TypeComboBox->count() != 0)
        ui->TypeComboBox->setCurrentIndex(ui->TypeComboBox->count() - 1);
    retrievedData.clear();
}

void SellDialog::on_VendorComboBox_currentIndexChanged(int index)
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
}

void SellDialog::showSerials(){
    ui->SerialComboBox->setEnabled(false);
    ui->SerialComboBox->clear();
    if(!ui->SoftComboBox->currentText().isEmpty() && !ui->TypeComboBox->currentText().isEmpty()){
        bool disconnect = false;
        bool connected = true;
        if(!db.isOpen()){
            disconnect = true;
            connected = parent->connectToDatabase(&db, false, this);
        }
        if(connected){
            QString condition;
            condition += "Type=" + ui->TypeComboBox->currentData().toString();
            condition += " AND Software=" + ui->SoftComboBox->currentData().toString();
            condition += " AND Duration=" + (ui->UnlimitedCheckBox->checkState() != Qt::Checked ? QString::number(ui->DurationSpinBox->value()) : "0");
            condition += " AND PCCount=" + QString::number(ui->CountSpinBox->value());
            condition += " AND Purchased=0";
            QVector<QStringList> retrievedData = parent->executeSelectCommand("licenses", parent->getColumnsNames("licenses"), condition);
            if(!retrievedData[0].isEmpty()){
                ui->SerialComboBox->setEnabled(true);
                for(int i = 0; i < retrievedData[0].size(); i++)
                    ui->SerialComboBox->addItem(retrievedData[parent->getColumnsNames("licenses").indexOf("Serial")][i], retrievedData[parent->getColumnsNames("licenses").indexOf("ID")][i]);
            }
            if(disconnect)
                parent->disconnectFromDatabase(&db);
        }
    }
}

void SellDialog::on_TypeComboBox_currentIndexChanged(int index)
{
    showSerials();
}

void SellDialog::on_DurationSpinBox_valueChanged(int arg1)
{
    ui->ExpirationDateSpinBox->setDate(QDate::currentDate().addMonths(ui->DurationSpinBox->value()));
    showSerials();
}

void SellDialog::on_CountSpinBox_valueChanged(int arg1)
{
    showSerials();
}

void SellDialog::on_CostEdit_textChanged(const QString &arg1)
{
    ui->DialogBtnBox->buttons()[0]->setEnabled(false);
    ui->printBtn->setEnabled(false);
    if(!ui->CostEdit->text().isEmpty() && !ui->SerialComboBox->currentText().isEmpty())
        if(!ui->SoftComboBox->currentText().isEmpty()){
            ui->DialogBtnBox->buttons()[0]->setEnabled(true);
            ui->printBtn->setEnabled(true);
        }
}

void SellDialog::on_SerialComboBox_currentIndexChanged(int index)
{
    ui->DialogBtnBox->buttons()[0]->setEnabled(false);
    ui->printBtn->setEnabled(false);
    if(!ui->CostEdit->text().isEmpty() && !ui->SerialComboBox->currentText().isEmpty()){
        ui->DialogBtnBox->buttons()[0]->setEnabled(true);
        ui->printBtn->setEnabled(true);
    }
}

void SellDialog::done(int result_code)
{
   if(QDialog::Accepted == result_code)
   {
       if(ui->ExpirationDateSpinBox->date() < ui->ActivationDateSpinBox->date()){
           QMessageBox::critical(this, "Ошибка", "Лицензия заканчивается раньше, чем активируется.", QMessageBox::Ok);
           return;
       }
       switch(parent->commitChangesDialog()){
       case QMessageBox::Cancel:
           return;
           break;
       case QMessageBox::No:
           QDialog::done(result_code);
           return;
           break;
       case QMessageBox::Yes:
           if(commitChanges()){
               parent->checkLicense(true);
               QDialog::done(result_code);
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

bool SellDialog::commitChanges(){
    bool connected = true;
    connected = parent->connectToDatabase(&db, false, this);
    if(connected){
        QStringList columnNames = parent->getColumnsNames("sales");
        QStringList values;
        values.clear();
        for(int i = 0; i < columnNames.size(); i++){
            if(columnNames[i] == "License")
                values.append(ui->SerialComboBox->currentData().toString());
            if(columnNames[i] == "Customer")
                values.append(ui->CustomerComboBox->currentData().toString());
            if(columnNames[i] == "Engineer")
                values.append(ui->EngineerComboBox->currentData().toString());
            if(columnNames[i] == "ActivationDate")
                values.append(ui->ActivationDateSpinBox->date().toString(Qt::ISODate));
            if(columnNames[i] == "ExpirationDate"){
                if(ui->UnlimitedCheckBox->isChecked())
                    values.append(QDate(9999, 12, 31).toString(Qt::ISODate));
                else
                    values.append(ui->ExpirationDateSpinBox->date().toString(Qt::ISODate));
            }
            if(columnNames[i] == "Track")
                values.append(QString::number(1));
            if(columnNames[i] == "Cost")
                values.append(ui->CostEdit->text());
        }
        db.transaction();
        QVector<QStringList> checkForSold = parent->executeSelectCommand("licenses", QString("Purchased").split(";"), "ID=" + ui->SerialComboBox->currentData().toString());
        if(!checkForSold.isEmpty()){
            for(int i = 0; i < checkForSold[0].size(); i++){
                if(checkForSold[0][i] != "0")
                    return false;
            }
        }
        else{
            return false;
        }
        if(!parent->executeInsertCommand("sales", columnNames, values)){
            db.rollback();
            parent->disconnectFromDatabase(&db);
            return false;
        }
        if(!parent->executeUpdateCommand("licenses", QString("Purchased").split(";"), QString("1").split(";"), "ID=" + ui->SerialComboBox->currentData().toString())){
            db.rollback();
            parent->disconnectFromDatabase(&db);
            return false;
        }
        if(!ui->UsernameEdit->text().isEmpty() || !ui->PasswordEdit->text().isEmpty()){
            if(!parent->executeUpdateCommand("licenses", QString("Username;Password").split(";"), QString(ui->UsernameEdit->text() + ";" + ui->PasswordEdit->text()).split(";"), "ID=" + ui->SerialComboBox->currentData().toString())){
                db.rollback();
                parent->disconnectFromDatabase(&db);
                return false;
            }
        }
        db.commit();
        parent->disconnectFromDatabase(&db);
        return true;
    }
    else{
        parent->disconnectFromDatabase(&db);
        return false;
    }
}

void SellDialog::on_printBtn_clicked()
{
    QXlsx::Document docToPrint(parent->getApplicationPath().remove(parent->getApplicationPath().lastIndexOf('/'), parent->getApplicationPath().length() - parent->getApplicationPath().lastIndexOf('/')) + "\\licenseTemplate.xlsx");
    QString tempDirectory = QDir::tempPath();
    QString value;
    QString alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for(int i = 0; i < parent->printFields.size(); i++){
        if(parent->printFields[i].field == "Type")
            value = ui->TypeComboBox->currentText();
        if(parent->printFields[i].field == "Soft")
            value = ui->VendorComboBox->currentText() + " " + ui->SoftComboBox->currentText();
        if(parent->printFields[i].field == "Serial")
            value = ui->SerialComboBox->currentText();
        if(parent->printFields[i].field == "Duration"){
            if(ui->UnlimitedCheckBox->isChecked())
                value = "бессрочная";
            else
                value = QString::number(ui->DurationSpinBox->value()) + " мес.";
        }
        if(parent->printFields[i].field == "PCCount")
            value = QString::number(ui->CountSpinBox->value()) + " шт.";
        if(parent->printFields[i].field == "Username" && !ui->UsernameEdit->text().isEmpty())
            value = "Имя пользователя: " + ui->UsernameEdit->text();
        if(parent->printFields[i].field == "Password" && !ui->PasswordEdit->text().isEmpty())
            value = "Пароль: " + ui->PasswordEdit->text();
        if(parent->printFields[i].field == "Logo"){
            QDirIterator it(parent->logoFolder, QStringList() << "*.*", QDir::Files, QDirIterator::NoIteratorFlags);
            while (it.hasNext()){
                QString file = it.next();
                if(file.contains(ui->VendorComboBox->currentText() + " " + ui->SoftComboBox->currentText(), Qt::CaseInsensitive)){
                    int row = parent->printFields[i].cell.mid(1).toInt() - 1;
                    int col = alphabet.indexOf(parent->printFields[i].cell.left(1));
                    QImage img(file);
                    img = img.scaledToWidth(68 * 3);
                    docToPrint.insertImage(row, col, img);
                    break;
                }
            }
        }
        if(parent->printFields[i].field == "Description")
            value = parent->getDescription(ui->SoftComboBox->currentText());
        if(!value.isEmpty())
            docToPrint.write(parent->printFields[i].cell, value);
        value.clear();
    }
    docToPrint.saveAs(tempDirectory + "\\licenseToPrint.xlsx");
    parent->printDocument(tempDirectory + "\\licenseToPrint.xlsx");
}

void SellDialog::on_SoftComboBox_currentIndexChanged(int index)
{
    showSerials();
}
