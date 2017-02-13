#include "changetextdialog.h"
#include "ui_changetextdialog.h"

ChangeTextDialog::ChangeTextDialog(QString textToChange, LicenseControl * par) :
    parent(par),
    ui(new Ui::ChangeTextDialog),
    typeOfNotification(textToChange)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags = flags & (~helpFlag);
    setWindowFlags(flags);
    if(typeOfNotification == "License"){
        ui->legendLabel->setText("$SOFT - программный продукт\n$COUNT - количество оставшихся лицензий\n");
        ui->textEdit->setPlainText(parent->licenseNotificationSettings.text);
    }
    else{
        ui->legendLabel->setText("$CUSTOMER - покупатель\n$SOFT - программный продукт\n$DATE - дата окончания лицензии");
        ui->textEdit->setPlainText(parent->customerNotificationSettings.text);
    }
}

ChangeTextDialog::~ChangeTextDialog()
{
    delete ui;
}

void ChangeTextDialog::on_buttonBox_accepted()
{
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;
    settings.beginGroup(typeOfNotification + "_Notification");
    settings.setValue("text", ui->textEdit->toPlainText());
    settings.endGroup();
}
