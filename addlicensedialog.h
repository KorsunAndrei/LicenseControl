#ifndef ADDLICENSEDIALOG_H
#define ADDLICENSEDIALOG_H

#include <QDialog>
#include <licensecontrol.h>

class LicenseControl;

namespace Ui {
class AddLicenseDialog;
}

class AddLicenseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddLicenseDialog(LicenseControl *par = 0);
    ~AddLicenseDialog();

private slots:
    void on_UnlimitedCheckBox_stateChanged(int arg1);

    void on_VendorComboBox_currentIndexChanged(int index);

    void on_SerialTextEdit_textChanged();
    void done(int r);

private:
    QSqlDatabase db;
    LicenseControl * parent;
    Ui::AddLicenseDialog *ui; 
    void getDataFromDatabase();
    bool commitChanges(int * count);
};

#endif // ADDLICENSEDIALOG_H
