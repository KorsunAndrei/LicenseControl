#include "configdialog.h"
#include "ui_configdialog.h"

ConfigDialog::ConfigDialog(LicenseControl *par) :
    parent(par),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags = flags & (~helpFlag);
    setWindowFlags(flags);
    ui->tabWidget->setTabText(0, "Общие");
    ui->tabWidget->setTabText(1, "Оповещения");
    QPushButton * btn = ui->DialogBtnBox->button ( QDialogButtonBox::Apply );
    ui->DialogBtnBox->button ( QDialogButtonBox::Apply )->setEnabled(false);
    connect(btn, SIGNAL(clicked()), this, SLOT(apply()));
    getSettings();
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::on_changeTextLicenseNotificationBtn_clicked()
{
    ChangeTextDialog * dialog = new ChangeTextDialog("License", parent);
    dialog->setModal(true);
    dialog->exec();
    dialog->~ChangeTextDialog();
}

void ConfigDialog::getSettings(){
    ui->StartupCheckBox->setChecked(parent->checkStartup());
    ui->DBAddressEdit->setText(parent->hostname);
    ui->DBUsernameEdit->setText(parent->username);
    ui->DBPasswordEdit->setText(parent->password);
    ui->soundNotificationCustomerCheckBox->setChecked(parent->customerNotificationSettings.sound);
    ui->soundNotificationLicenseCheckBox->setChecked(parent->licenseNotificationSettings.sound);
    ui->pathEdit->setText(parent->soundPath);
    ui->daysSpinBox->setValue(parent->daysCount);
    ui->intervalCustomerSpinBox->setValue(parent->customerNotificationSettings.interval);
    ui->intervalLicenseSpinBox->setValue(parent->licenseNotificationSettings.interval);
}

bool ConfigDialog::apply(){
    bool flag = true;
    QSettings settings;
    settings.beginGroup("Database");
    settings.setValue("hostname", ui->DBAddressEdit->text());
    parent->hostname = ui->DBAddressEdit->text();
    settings.setValue("username", ui->DBUsernameEdit->text());
    parent->username = ui->DBUsernameEdit->text();
    settings.setValue("password", ui->DBPasswordEdit->text());
    parent->password = ui->DBPasswordEdit->text();
    settings.endGroup();
    if(parent->checkStartup() != (ui->StartupCheckBox->isChecked())){
        if(ui->StartupCheckBox->isChecked()){
            QSettings registrySettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
            registrySettings.setValue("License Control", parent->getApplicationPath().replace('/', "\\"));
        }
        else{
            QSettings registrySettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
            QString appPath = parent->getApplicationPath().replace('/', "\\");
            for(int i = 0; i < registrySettings.allKeys().size(); i++){
                if(appPath == registrySettings.value(registrySettings.allKeys()[i]).toString()){
                    registrySettings.remove(registrySettings.allKeys()[i]);
                    break;
                }
            }
        }
    }
    settings.beginGroup("Notifications");
    QFileInfo checkFile(ui->pathEdit->text());
    if(checkFile.exists() && checkFile.isFile()){
        settings.setValue("sound_file", ui->pathEdit->text());
        parent->soundPath = ui->pathEdit->text();
    }
    else{
        settings.setValue("sound_file", "");
        parent->soundPath.clear();
        QMessageBox::critical(this, "Ошибка", "Неверный путь к файлу звукового оповещения", QMessageBox::Ok);
        flag = false;
    }
    {
        settings.beginGroup("License_Notification");
        settings.setValue("sound", (ui->soundNotificationLicenseCheckBox->isChecked() ? 1 : 0));
        parent->licenseNotificationSettings.sound  = (ui->soundNotificationLicenseCheckBox->isChecked() ? true : false);
        settings.setValue("interval", ui->intervalLicenseSpinBox->value());
        parent->licenseNotificationSettings.interval = ui->intervalLicenseSpinBox->value();
        if(parent->licenseTimer->interval() != parent->licenseNotificationSettings.interval){
            parent->licenseTimer->setInterval(parent->licenseNotificationSettings.interval * 60 * 1000);
            if(parent->licenseTimer->isActive())
                parent->licenseTimer->start();
        }
        settings.endGroup();
    }
    {
        settings.beginGroup("Customer_Notification");
        settings.setValue("sound", (ui->soundNotificationCustomerCheckBox->isChecked() ? 1 : 0));
        parent->customerNotificationSettings.sound = (ui->soundNotificationCustomerCheckBox->isChecked() ? true : false);
        settings.setValue("interval", ui->intervalCustomerSpinBox->value());
        parent->customerNotificationSettings.interval = ui->intervalCustomerSpinBox->value();
        if(parent->customerTimer->interval() != parent->customerNotificationSettings.interval){
            parent->customerTimer->setInterval(parent->customerNotificationSettings.interval * 60 * 1000);
            if(parent->customerTimer->isActive())
                parent->customerTimer->start();
        }
        settings.setValue("day_count", ui->daysSpinBox->value());
        parent->daysCount = ui->daysSpinBox->value();
        settings.endGroup();
    }
    ui->DialogBtnBox->button ( QDialogButtonBox::Apply )->setEnabled(false);
    return flag;
}

void ConfigDialog::on_soundNotificationLicenseCheckBox_toggled(bool checked)
{
    ui->DialogBtnBox->button ( QDialogButtonBox::Apply )->setEnabled(true);
}

void ConfigDialog::on_soundNotificationCustomerCheckBox_toggled(bool checked)
{
    ui->DialogBtnBox->button ( QDialogButtonBox::Apply )->setEnabled(true);
}

void ConfigDialog::done(int result_code)
{
   if(QDialog::Accepted == result_code)
   {
        if(apply()){
            parent->checkLicense(false);
            parent->checkCustomer(false);
            QDialog::done(result_code);
        }
        return;
   }
   else
   {
       QDialog::done(result_code);
       return;
   }
}

void ConfigDialog::on_changeTextCustomerNotificationBtn_clicked()
{
    ChangeTextDialog * dialog = new ChangeTextDialog("Customer", parent);
    dialog->setModal(true);
    dialog->exec();
    dialog->~ChangeTextDialog();
}

void ConfigDialog::on_DBAddressEdit_textEdited(const QString &arg1)
{
    ui->DialogBtnBox->button ( QDialogButtonBox::Apply )->setEnabled(true);
}

void ConfigDialog::on_DBUsernameEdit_textEdited(const QString &arg1)
{
    ui->DialogBtnBox->button ( QDialogButtonBox::Apply )->setEnabled(true);
}

void ConfigDialog::on_DBPasswordEdit_textEdited(const QString &arg1)
{
    ui->DialogBtnBox->button ( QDialogButtonBox::Apply )->setEnabled(true);
}

void ConfigDialog::on_StartupCheckBox_toggled(bool checked)
{
    ui->DialogBtnBox->button ( QDialogButtonBox::Apply )->setEnabled(true);
}

void ConfigDialog::on_intervalCustomerSpinBox_valueChanged(int arg1)
{
    ui->DialogBtnBox->button ( QDialogButtonBox::Apply )->setEnabled(true);
}

void ConfigDialog::on_intervalLicenseSpinBox_valueChanged(int arg1)
{
    ui->DialogBtnBox->button ( QDialogButtonBox::Apply )->setEnabled(true);
}

void ConfigDialog::on_daysSpinBox_valueChanged(int arg1)
{
    ui->DialogBtnBox->button ( QDialogButtonBox::Apply )->setEnabled(true);
}

void ConfigDialog::on_countLicenseBtn_clicked()
{
    ChangeLicenseCountDialog * dialog = new ChangeLicenseCountDialog(parent);
    dialog->setModal(true);
    dialog->exec();
    dialog->~ChangeLicenseCountDialog();
}

void ConfigDialog::on_browseSoundBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Открыть файл"), "", tr("Sound files (*.wav)"));
    if(!fileName.isEmpty()){
        ui->pathEdit->setText(fileName);
    }
    else
        ui->pathEdit->clear();
    ui->DialogBtnBox->button ( QDialogButtonBox::Apply )->setEnabled(true);
}

void ConfigDialog::on_pathEdit_textEdited(const QString &arg1)
{
    ui->DialogBtnBox->button ( QDialogButtonBox::Apply )->setEnabled(true);
}
