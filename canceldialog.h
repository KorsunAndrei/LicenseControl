#ifndef CANCELDIALOG_H
#define CANCELDIALOG_H

#include <QDialog>
#include <licensecontrol.h>

class LicenseControl;

namespace Ui {
class CancelDialog;
}

class CancelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CancelDialog(LicenseControl *par = 0);
    ~CancelDialog();

private slots:
    void on_CustomerComboBox_currentIndexChanged(int index);

    void on_SoftComboBox_currentIndexChanged(int index);

    void on_DialogBtnBox_accepted();

private:
    QSqlDatabase db;
    LicenseControl * parent;
    Ui::CancelDialog *ui;
    void getDataFromDatabase();
    void showSerials();
};

#endif // CANCELDIALOG_H
