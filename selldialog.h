#ifndef SELLDIALOG_H
#define SELLDIALOG_H

#include <QDialog>
#include <QCloseEvent>

#include <newcustomerdialog.h>
#include <licensecontrol.h>

class LicenseControl;

namespace Ui {
class SellDialog;
}

class SellDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SellDialog(LicenseControl *par = 0);
    ~SellDialog();

private slots:

    void on_UnlimitedCheckBox_stateChanged(int arg1);

    void on_AddCustomerBtn_clicked();
    void getDataFromDatabase();

    void on_VendorComboBox_currentIndexChanged(int index);

    void showSerials();

    void on_TypeComboBox_currentIndexChanged(int index);

    void on_DurationSpinBox_valueChanged(int arg1);

    void on_CountSpinBox_valueChanged(int arg1);

    void on_CostEdit_textChanged(const QString &arg1);

    void on_SerialComboBox_currentIndexChanged(int index);
    void done(int r);
    bool commitChanges();

    void on_printBtn_clicked();

    void on_SoftComboBox_currentIndexChanged(int index);

private:
    QSqlDatabase db;
    LicenseControl * parent;
    Ui::SellDialog *ui;
};

#endif // SELLDIALOG_H
