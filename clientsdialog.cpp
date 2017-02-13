#include "clientsdialog.h"
#include "ui_clientsdialog.h"

ClientsDialog::ClientsDialog(LicenseControl *par) :
    parent(par),
    ui(new Ui::ClientsDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags = flags & (~helpFlag);
    setWindowFlags(flags);
    bool connected = parent->connectToDatabase(&db, true, this);
    if(connected){
        getDataFromDatabase();
        parent->disconnectFromDatabase(&db);
    }
}

ClientsDialog::~ClientsDialog()
{
    delete ui;
}

void ClientsDialog::getDataFromDatabase(){
    QStringList columnNames = parent->getColumnsNames("customers");
    QVector<QStringList> retrievedData = parent->executeSelectCommand("customers", columnNames);
    if(!retrievedData.isEmpty()){
        for(int i = 0; i < retrievedData[0].size(); i++){
            ui->customerComboBox->addItem(retrievedData[columnNames.indexOf("Surname")][i] + " " + retrievedData[columnNames.indexOf("Name")][i] + " " + retrievedData[columnNames.indexOf("Patronomic")][i], retrievedData[columnNames.indexOf("ID")][i]);
        }
    }
}

void ClientsDialog::on_customerComboBox_currentIndexChanged(int index)
{
    ui->licenseComboBox->clear();
    bool disconnect = false;
    bool connected = true;
    if(!db.isOpen()){
        disconnect = true;
        connected = parent->connectToDatabase(&db, false, this);
    }
    if(connected){
        QStringList columnNames = parent->getColumnsNames("customers");
        QVector<QStringList> retrievedData = parent->executeSelectCommand("customers", columnNames, "ID=" + ui->customerComboBox->currentData().toString());
        if(!retrievedData.isEmpty()){
            ui->addressLabel->setText("Адрес: " + retrievedData[columnNames.indexOf("Address")][0]);
            ui->emailLabel->setText("E-mail: " + retrievedData[columnNames.indexOf("Email")][0]);
            ui->phoneNumberLabel->setText("Телефонный номер: (" + retrievedData[columnNames.indexOf("PhoneCode")][0] + ") " + retrievedData[columnNames.indexOf("PhoneNumber")][0]);
        }
        retrievedData = parent->executeSelectCommand("sales", "licenses", parent->getColumnsNames("sales"), parent->getColumnsNames("licenses"), "License", "sales.Customer=" + ui->customerComboBox->currentData().toString() + " AND licenses.Purchased=1");
        columnNames = parent->getColumnsNames("sales");
        columnNames.append(parent->getColumnsNames("licenses"));
        if(!retrievedData.isEmpty()){
            for(int i = 0; i < retrievedData[0].size(); i++){
                QVector<QStringList> tempData = parent->executeSelectCommand("software", parent->getColumnsNames("software"), "ID=" + retrievedData[columnNames.indexOf("Software")][i]);
                if(!tempData.isEmpty()){
                    QVector<QStringList> vendor = parent->executeSelectCommand("vendors", parent->getColumnsNames("vendors"), "ID=" + tempData[parent->getColumnsNames("software").indexOf("Vendor")][0]);
                    if(!vendor.isEmpty())
                        ui->licenseComboBox->addItem(vendor[parent->getColumnsNames("vendors").indexOf("Vendor")][0] + " " + tempData[parent->getColumnsNames("software").indexOf("Title")][0], retrievedData[columnNames.indexOf("License")][i]);
                }
            }
        }
    }
    if(disconnect)
        parent->disconnectFromDatabase(&db);
    if(ui->licenseComboBox->count() != 0)
        ui->printBtn->setEnabled(true);
    else
        ui->printBtn->setEnabled(false);
}

void ClientsDialog::on_licenseComboBox_currentIndexChanged(int index)
{
    setLabels();
    bool disconnect = false;
    bool connected = true;
    if(!db.isOpen()){
        disconnect = true;
        connected = parent->connectToDatabase(&db, false, this);
    }
    if(connected){
        QVector<QStringList> retrievedData = parent->executeSelectCommand("sales", "licenses", parent->getColumnsNames("sales"), parent->getColumnsNames("licenses"), "License", "sales.Customer=" + ui->customerComboBox->currentData().toString() + " AND sales.License=" + ui->licenseComboBox->currentData().toString());
        QStringList columnNames = parent->getColumnsNames("sales");
        columnNames.append(parent->getColumnsNames("licenses"));
        if(!retrievedData.isEmpty()){
            for(int i = 0; i < retrievedData[0].size(); i++){
                QVector<QStringList> type = parent->executeSelectCommand("types", QString("Type").split(";"), "ID=" + retrievedData[columnNames.indexOf("Type")][i]);
                ui->trackLabel->setText(QString("Отслеживание: ") + ((retrievedData[columnNames.indexOf("Track")][i] == "1") ? "Да" : "Нет"));
                ui->activationDateLabel->setText("Дата активации: " + retrievedData[columnNames.indexOf("ActivationDate")][i]);
                if(retrievedData[columnNames.indexOf("ExpirationDate")][i] == "9999-12-31")
                    ui->expirationDateLabel->setText("Дата окончания: ");
                else
                    ui->expirationDateLabel->setText("Дата окончания: " + retrievedData[columnNames.indexOf("ExpirationDate")][i]);
                if(!type.isEmpty())
                    for(int m = 0; m < type[0].size(); m++)
                        ui->typeLabel->setText("Тип: " + type[0][m]);
                ui->serialLabel->setText("Серийный номер: " + retrievedData[columnNames.indexOf("Serial")][i]);
                ui->usernameLabel->setText("Имя пользователя: " + retrievedData[columnNames.indexOf("Username")][i]);
                ui->passwordLabel->setText("Пароль: " + retrievedData[columnNames.indexOf("Password")][i]);
                ui->countLabel->setText("Количество ПК: " + retrievedData[columnNames.indexOf("PCCount")][i] + " шт.");
                if(retrievedData[columnNames.indexOf("Duration")][i] == "0")
                    ui->durationLabel->setText("Срок действия: бессрочная");
                else
                    ui->durationLabel->setText("Срок действия: " + retrievedData[columnNames.indexOf("Duration")][i] + " мес.");
                ui->costLabel->setText("Цена: " + retrievedData[columnNames.indexOf("Cost")][i] + " р.");
            }
        }
    }
    if(disconnect)
        parent->disconnectFromDatabase(&db);
}

void ClientsDialog::setLabels(){
    ui->trackLabel->setText("Отслеживание: ");
    ui->activationDateLabel->setText("Дата активации: ");
    ui->expirationDateLabel->setText("Дата окончания: ");
    ui->typeLabel->setText("Тип: ");
    ui->serialLabel->setText("Серийный номер: ");
    ui->usernameLabel->setText("Имя пользователя: ");
    ui->passwordLabel->setText("Пароль: ");
    ui->countLabel->setText("Количество ПК: ");
    ui->durationLabel->setText("Срок действия: ");
    ui->costLabel->setText("Цена: ");
}

void ClientsDialog::on_closeBtn_clicked()
{
    this->close();
}

void ClientsDialog::on_printBtn_clicked()
{
    QXlsx::Document docToPrint(parent->getApplicationPath().remove(parent->getApplicationPath().lastIndexOf('/'), parent->getApplicationPath().length() - parent->getApplicationPath().lastIndexOf('/')) + "\\licenseTemplate.xlsx");
    QString tempDirectory = QDir::tempPath();
    QString value;
    QString alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for(int i = 0; i < parent->printFields.size(); i++){
        if(parent->printFields[i].field == "Type")
            value = ui->typeLabel->text().remove("Тип: ");
        if(parent->printFields[i].field == "Soft")
            value = ui->licenseComboBox->currentText();
        if(parent->printFields[i].field == "Serial")
            value = ui->serialLabel->text().remove("Серийный номер: ");
        if(parent->printFields[i].field == "Duration")
            value = ui->durationLabel->text().remove("Срок действия: ");
        if(parent->printFields[i].field == "PCCount")
            value = ui->countLabel->text().remove("Количество ПК: ");
        if(parent->printFields[i].field == "Username")
            value = ui->usernameLabel->text().remove("Имя пользователя: ");
        if(parent->printFields[i].field == "Password")
            value = ui->passwordLabel->text().remove("Пароль: ");
        if(parent->printFields[i].field == "Logo"){
            QDirIterator it(parent->logoFolder, QStringList() << "*.*", QDir::Files, QDirIterator::NoIteratorFlags);
            while (it.hasNext()){
                QString file = it.next();
                if(file.contains(ui->licenseComboBox->currentText(), Qt::CaseInsensitive)){
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
            value = parent->getDescription(ui->licenseComboBox->currentData().toInt());
        if(!value.isEmpty())
            docToPrint.write(parent->printFields[i].cell, value);
        value.clear();
    }
    docToPrint.saveAs(tempDirectory + "\\licenseToPrint.xlsx");
    parent->printDocument(tempDirectory + "\\licenseToPrint.xlsx");
}
