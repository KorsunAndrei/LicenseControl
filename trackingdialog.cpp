#include "trackingdialog.h"
#include "ui_trackingdialog.h"

TrackingDialog::TrackingDialog(LicenseControl * par, int tempCustomer, QString tempDate, int tempLicense) :
    parent(par),
    ui(new Ui::TrackingDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags = flags & (~helpFlag);
    setWindowFlags(flags);
    customer = tempCustomer;
    date = tempDate;
    license = tempLicense;
}

TrackingDialog::~TrackingDialog()
{
    delete ui;
}

void TrackingDialog::on_disableTrackBtn_clicked()
{
    QString condition = "";
    condition += "Customer=";
    condition += QString::number(customer);
    condition += " AND ";
    condition += "ExpirationDate='";
    condition += date;
    condition += "'";
    condition += " AND ";
    condition += "License=";
    condition += QString::number(license);
    if(parent->executeUpdateCommand("sales", QStringList("Track"), QStringList("0"), condition)){
        setResult(1);
    }
    else
        QMessageBox::critical(this, "Ошибка", "Произошла ошибка при записи данных в базу данных", QMessageBox::Ok);
    this->close();
}

void TrackingDialog::on_nothingDoBtn_clicked()
{
    setResult(0);
    this->close();
}

void TrackingDialog::on_delayBtn_clicked()
{
    parent->licenseDoNotTrack.append(license);
    setResult(2);
    this->close();
}
