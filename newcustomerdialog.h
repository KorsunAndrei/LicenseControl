#ifndef NEWCUSTOMERDIALOG_H
#define NEWCUSTOMERDIALOG_H

#include <QDialog>
#include <QAbstractButton>

#include <licensecontrol.h>
class LicenseControl;

namespace Ui {
class NewCustomerDialog;
}

class NewCustomerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewCustomerDialog(LicenseControl *par = 0);
    ~NewCustomerDialog();

private slots:
    void on_SurnameEdit_textChanged(const QString &arg1);
    void on_NameEdit_textChanged(const QString &arg1);
    void on_PhoneCodeEdit_textChanged(const QString &arg1);
    void on_PhoneNumberEdit_textChanged(const QString &arg1);
    void done(int r);

    void on_PatronomicEdit_textChanged(const QString &arg1);

private:
    QSqlDatabase db;
    LicenseControl * parent;
    Ui::NewCustomerDialog *ui;
    void checkEdits();
    bool commitChanges();
};

#endif // NEWCUSTOMERDIALOG_H
