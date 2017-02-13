#include "newcustomerdialog.h"
#include "ui_newcustomerdialog.h"

NewCustomerDialog::NewCustomerDialog(LicenseControl *par) :
    parent(par),
    ui(new Ui::NewCustomerDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags = flags & (~helpFlag);
    setWindowFlags(flags);
    ui->DialogBtnBox->buttons()[0]->setEnabled(false);
    ui->DialogBtnBox->buttons()[0]->setText("Добавить");
    QRegExp phoneCodeRegExp("[0-9]{3,5}");
    QRegExp phoneNumberRegExp("[0-9]{5,7}");
    QRegExp textRegExp("^[А-Яа-я]*$");
    QRegExp emailRegExp("^[A-Za-z0-9_.+-]+@[A-Za-z0-9-]+\\.[a-zA-Z0-9-.]+$");
    QRegExp addressRegExp("^[А-Яа-я0-9. -]*$");
    QRegExpValidator * phoneCodeValidator = new QRegExpValidator(phoneCodeRegExp);
    QRegExpValidator * phoneNumberValidator = new QRegExpValidator(phoneNumberRegExp);
    QRegExpValidator * textValidator = new QRegExpValidator(textRegExp);
    QRegExpValidator * emailValidator = new QRegExpValidator(emailRegExp);
    QRegExpValidator * addressValidator = new QRegExpValidator(addressRegExp);
    ui->PhoneCodeEdit->setValidator(phoneCodeValidator);
    ui->PhoneNumberEdit->setValidator(phoneNumberValidator);
    ui->SurnameEdit->setValidator(textValidator);
    ui->NameEdit->setValidator(textValidator);
    ui->PatronomicEdit->setValidator(textValidator);
    ui->EmailEdit->setValidator(emailValidator);
    ui->AddressEdit->setValidator(addressValidator);
    ui->PhoneCodeEdit->setText("42435");
}

NewCustomerDialog::~NewCustomerDialog()
{
    delete ui;
}

void NewCustomerDialog::checkEdits(){
    if(!(ui->SurnameEdit->text().isEmpty()) && !(ui->NameEdit->text().isEmpty()) && !(ui->PhoneCodeEdit->text().isEmpty()) && !(ui->PhoneNumberEdit->text().isEmpty()))
        ui->DialogBtnBox->buttons()[0]->setEnabled(true);
    else
        ui->DialogBtnBox->buttons()[0]->setEnabled(false);
}

void NewCustomerDialog::on_SurnameEdit_textChanged(const QString &arg1)
{
    checkEdits();
    QString tempStr = arg1.toLower();
    if(!tempStr.isEmpty())
        tempStr[0] = tempStr[0].toUpper();
    ui->SurnameEdit->setText(tempStr);
}

void NewCustomerDialog::on_NameEdit_textChanged(const QString &arg1)
{
    checkEdits();
    QString tempStr = arg1.toLower();
    if(!tempStr.isEmpty())
        tempStr[0] = tempStr[0].toUpper();
    ui->NameEdit->setText(tempStr);
}

void NewCustomerDialog::on_PhoneCodeEdit_textChanged(const QString &arg1)
{
    checkEdits();
}

void NewCustomerDialog::on_PhoneNumberEdit_textChanged(const QString &arg1)
{
    checkEdits();
}

 void NewCustomerDialog::done(int result_code)
{
    if(QDialog::Accepted == result_code)
    {
        if(ui->PhoneCodeEdit->text().length() + ui->PhoneNumberEdit->text().length() != 10){
            QMessageBox::critical(this, "Ошибка", "Ошибка в введенных данных", QMessageBox::Ok);
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
            if(commitChanges())
                QDialog::done(result_code);
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

void NewCustomerDialog::on_PatronomicEdit_textChanged(const QString &arg1)
{
    QString tempStr = arg1.toLower();
    if(!tempStr.isEmpty())
        tempStr[0] = tempStr[0].toUpper();
    ui->PatronomicEdit->setText(tempStr);
}

bool NewCustomerDialog::commitChanges()
{
    bool connected = true;
    connected = parent->connectToDatabase(&db, false, this);
    if(connected){
        QStringList columnNames = parent->getColumnsNames("customers");
        if(!columnNames.isEmpty())
            columnNames.removeFirst();
        QStringList values;
        for(int i = 0; i < columnNames.size(); i++){
            if(columnNames[i] == "Surname")
                values.append(ui->SurnameEdit->text());
            if(columnNames[i] == "Name")
                values.append(ui->NameEdit->text());
            if(columnNames[i] == "Patronomic")
                values.append(ui->PatronomicEdit->text());
            if(columnNames[i] == "Address")
                values.append(ui->AddressEdit->text());
            if(columnNames[i] == "Email")
                values.append(ui->EmailEdit->text());
            if(columnNames[i] == "PhoneCode")
                values.append(ui->PhoneCodeEdit->text());
            if(columnNames[i] == "PhoneNumber")
                values.append(ui->PhoneNumberEdit->text());
        }
        if(!parent->executeInsertCommand("customers", columnNames, values))
            return false;
        parent->disconnectFromDatabase(&db);
        return true;
    }
    else{
        parent->disconnectFromDatabase(&db);
        return false;
    }
}
