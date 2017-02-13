#ifndef REPORTLICENSESDIALOG_H
#define REPORTLICENSESDIALOG_H

#include <QDialog>
#include <licensecontrol.h>
class LicenseControl;

namespace Ui {
class reportLicensesDialog;
}

class reportLicensesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit reportLicensesDialog(LicenseControl *par = 0);
    ~reportLicensesDialog();

private slots:
    void on_toFIleBtn_clicked();

    void on_printBtn_clicked();

private:
    LicenseControl * parent;
    QSqlDatabase db;
    Ui::reportLicensesDialog *ui;
    void generateReport();
};

#endif // REPORTLICENSESDIALOG_H
