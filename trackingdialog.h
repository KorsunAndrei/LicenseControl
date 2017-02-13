#ifndef TRACKINGDIALOG_H
#define TRACKINGDIALOG_H

#include <QDialog>
#include <licensecontrol.h>
class LicenseControl;

namespace Ui {
class TrackingDialog;
}

class TrackingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TrackingDialog(LicenseControl *par, int tempCustomer, QString tempDate, int tempLicense);
    ~TrackingDialog();

private slots:
    void on_disableTrackBtn_clicked();

    void on_nothingDoBtn_clicked();

    void on_delayBtn_clicked();

private:
    LicenseControl * parent;
    Ui::TrackingDialog *ui;
    int customer;
    QString date;
    int license;
};

#endif // TRACKINGDIALOG_H
