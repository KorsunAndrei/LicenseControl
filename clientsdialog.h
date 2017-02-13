#ifndef CLIENTSDIALOG_H
#define CLIENTSDIALOG_H

#include <QDialog>
#include <licensecontrol.h>
class LicenseControl;

namespace Ui {
class ClientsDialog;
}

class ClientsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientsDialog(LicenseControl *par);
    ~ClientsDialog();

private slots:
    void on_customerComboBox_currentIndexChanged(int index);

    void on_licenseComboBox_currentIndexChanged(int index);

    void on_closeBtn_clicked();

    void on_printBtn_clicked();

private:
    QSqlDatabase db;
    LicenseControl * parent;
    Ui::ClientsDialog *ui;
    void getDataFromDatabase();
    void setLabels();
};

#endif // CLIENTSDIALOG_H
