#ifndef REPORTDIALOG_H
#define REPORTDIALOG_H

#include <QDialog>
#include <licensecontrol.h>
class LicenseControl;

namespace Ui {
class ReportDialog;
}

class ReportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReportDialog(LicenseControl *par = 0);
    ~ReportDialog();

private slots:
    void on_saveToFileBtn_clicked();

    void on_printBtn_clicked();

private:
    void getDataFromDatabase();
    void generateReport();
    LicenseControl * parent;
    Ui::ReportDialog *ui;
    QSqlDatabase db;
};

#endif // REPORTDIALOG_H
