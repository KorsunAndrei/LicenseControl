#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <licensecontrol.h>

class LicenseControl;

#include <changetextdialog.h>

class ChangeTextDialog;

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(LicenseControl *par = 0);
    ~ConfigDialog();

private slots:
    void on_changeTextLicenseNotificationBtn_clicked();

    void getSettings();

    bool apply();

    void done(int result_code);

    void on_soundNotificationLicenseCheckBox_toggled(bool checked);

    void on_soundNotificationCustomerCheckBox_toggled(bool checked);

    void on_changeTextCustomerNotificationBtn_clicked();

    void on_DBAddressEdit_textEdited(const QString &arg1);

    void on_DBUsernameEdit_textEdited(const QString &arg1);

    void on_DBPasswordEdit_textEdited(const QString &arg1);

    void on_StartupCheckBox_toggled(bool checked);

    void on_intervalCustomerSpinBox_valueChanged(int arg1);

    void on_intervalLicenseSpinBox_valueChanged(int arg1);

    void on_daysSpinBox_valueChanged(int arg1);

    void on_countLicenseBtn_clicked();

    void on_browseSoundBtn_clicked();

    void on_pathEdit_textEdited(const QString &arg1);

private:
    LicenseControl * parent;
    Ui::ConfigDialog *ui;
};

#endif // CONFIGDIALOG_H
