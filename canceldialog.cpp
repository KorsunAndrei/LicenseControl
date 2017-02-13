#include "canceldialog.h"
#include "ui_canceldialog.h"

CancelDialog::CancelDialog(LicenseControl *par) :
    parent(par),
    ui(new Ui::CancelDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags = flags & (~helpFlag);
    setWindowFlags(flags);
    bool connected = false;
    ui->DialogBtnBox->buttons()[0]->setEnabled(false);
    ui->DialogBtnBox->buttons()[0]->setText("Отменить");
    ui->DialogBtnBox->buttons()[1]->setText("Закрыть");
    connected = parent->connectToDatabase(&db, true, this);
    if(connected){
        getDataFromDatabase();
        parent->disconnectFromDatabase(&db);
    }
}

CancelDialog::~CancelDialog()
{
    delete ui;
}

void CancelDialog::getDataFromDatabase(){
    QVector<QStringList> retrievedData;
    retrievedData = parent->executeSelectCommand("Customers", parent->getColumnsNames("Customers"));
    if(!retrievedData.isEmpty()){
        for(int i = 0; i < retrievedData[0].size(); i++)
            ui->CustomerComboBox->addItem(retrievedData[parent->getColumnsNames("Customers").indexOf("Surname")][i] + " " + retrievedData[parent->getColumnsNames("Customers").indexOf("Name")][i] + " " + retrievedData[parent->getColumnsNames("Customers").indexOf("Patronomic")][i] + (retrievedData[parent->getColumnsNames("Customers").indexOf("Address")][i] == "" ? "" : "," + retrievedData[parent->getColumnsNames("Customers").indexOf("Address")][i]), retrievedData[parent->getColumnsNames("Customers").indexOf("ID")][i]);
    }
    retrievedData.clear();
}

void CancelDialog::showSerials(){
    ui->SoftComboBox->setEnabled(false);
    ui->SoftComboBox->clear();
    if(!ui->CustomerComboBox->currentText().isEmpty()){
        bool disconnect = false;
        bool connected = true;
        if(!db.isOpen()){
            disconnect = true;
            connected = parent->connectToDatabase(&db, false, this);
        }
        if(connected){
            QString condition;
            condition += "sales.Customer=" + ui->CustomerComboBox->currentData().toString();
            condition += " AND licenses.purchased=1";
            QVector<QStringList> retrievedData = parent->executeSelectCommand("sales", "licenses", parent->getColumnsNames("sales"), parent->getColumnsNames("licenses"), "license",  condition);
            if(!retrievedData.isEmpty()){
                if(!retrievedData[0].isEmpty()){
                    ui->SoftComboBox->setEnabled(true);
                    for(int i = 0; i < retrievedData[0].size(); i++){
                        QStringList columnNames = parent->getColumnsNames("sales");
                        columnNames.append(parent->getColumnsNames("licenses"));
                        QVector<QStringList> soft = parent->executeSelectCommand("software", parent->getColumnsNames("software"), "ID=" + retrievedData[columnNames.indexOf("Software")][i]);
                        if(!soft.isEmpty()){
                            QVector<QStringList> vendor = parent->executeSelectCommand("vendors", parent->getColumnsNames("vendors"), "ID=" + soft[parent->getColumnsNames("software").indexOf("Vendor")][0]);
                            if(!vendor.isEmpty())
                                ui->SoftComboBox->addItem(vendor[parent->getColumnsNames("vendors").indexOf("Vendor")][0] + " " + soft[parent->getColumnsNames("software").indexOf("Title")][0], retrievedData[columnNames.indexOf("ID")][i]);
                        }
                    }
                }
            }
            if(disconnect)
                parent->disconnectFromDatabase(&db);
        }
    }
}
void CancelDialog::on_CustomerComboBox_currentIndexChanged(int index)
{
    showSerials();
    if(!ui->SoftComboBox->currentText().isEmpty())
        ui->DialogBtnBox->buttons()[0]->setEnabled(true);
    else
        ui->DialogBtnBox->buttons()[0]->setEnabled(false);
}

void CancelDialog::on_SoftComboBox_currentIndexChanged(int index)
{
    ui->ActDateEdit->clear();
    ui->SerialEdit->clear();
    ui->TypeEdit->clear();
    bool disconnect = false;
    bool connected = true;
    if(!db.isOpen()){
        disconnect = true;
        connected = parent->connectToDatabase(&db, false, this);
    }
    if(connected){
        QVector<QStringList> retrievedData;
        retrievedData = parent->executeSelectCommand("licenses", parent->getColumnsNames("licenses"), "ID=" + ui->SoftComboBox->currentData().toString());
        if(!retrievedData.isEmpty()){
            QStringList columnNames = parent->getColumnsNames("licenses");
            ui->TypeEdit->setText(parent->executeSelectCommand("Types", parent->getColumnsNames("Types"), "ID=" + retrievedData[columnNames.indexOf("Type")][0])[parent->getColumnsNames("Types").indexOf("Type")][0]);
            ui->SerialEdit->setText(retrievedData[parent->getColumnsNames("licenses").indexOf("Serial")][0]);
            retrievedData.clear();
            retrievedData = parent->executeSelectCommand("sales", "licenses", parent->getColumnsNames("sales"), parent->getColumnsNames("licenses"), "license", "sales.License=" + ui->SoftComboBox->currentData().toString() + " AND licenses.Purchased=1");
            ui->ActDateEdit->setText(retrievedData[parent->getColumnsNames("sales").indexOf("ActivationDate")][0]);
        }
    }
    if(disconnect)
        parent->disconnectFromDatabase(&db);
}

void CancelDialog::on_DialogBtnBox_accepted()
{
    if(!ui->SoftComboBox->currentText().isEmpty()){
        bool connected = parent->connectToDatabase(&db, false, this);
        if(connected){
            db.transaction();
            if(!parent->executeUpdateCommand("licenses", QString("Purchased").split(";"), QString("0").split(";"), "ID=" + ui->SoftComboBox->currentData().toString())){
                db.rollback();
                return;
            }
            if(!parent->executeDeleteCommand("sales", "License=" + ui->SoftComboBox->currentData().toString() + " AND Customer=" + ui->CustomerComboBox->currentData().toString())){
                db.rollback();
                return;
            }
            db.commit();
            parent->checkLicense(false);
            parent->checkCustomer(false);
        }
        parent->disconnectFromDatabase(&db);
    }
}
