#ifndef SOFTDIALOG_H
#define SOFTDIALOG_H

#include <QDialog>
#include <QInputDialog>
#include <licensecontrol.h>
class LicenseControl;

namespace Ui {
class SoftDialog;
}

class SoftDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SoftDialog(LicenseControl *par = 0);
    ~SoftDialog();

private slots:
    void on_vendorComboBox_currentIndexChanged(int index);

    void on_changeLogoBtn_clicked();

    void on_softComboBox_currentIndexChanged(int index);

    void on_buttonBox_accepted();

private:
    QSqlDatabase db;
    LicenseControl * parent;
    Ui::SoftDialog *ui;
    void getDataFromDatabase();
    void getLogo();
    void getLogo(QString filename);
    void setLogo();
    QString newLogoFilePath;
};

#endif // SOFTDIALOG_H
