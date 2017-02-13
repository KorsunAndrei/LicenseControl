#ifndef NOTIFICATIONDIALOG_H
#define NOTIFICATIONDIALOG_H

#include <QDialog>
#include <licensecontrol.h>
class LicenseControl;

namespace Ui {
class NotificationDialog;
}

class NotificationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NotificationDialog(LicenseControl *par = 0);
    ~NotificationDialog();

private slots:
    void on_tableWidget_cellDoubleClicked(int row, int column);
    void getDataFromDatabase();

private:
    QSqlDatabase db;
    LicenseControl * parent;
    Ui::NotificationDialog *ui;
};

#endif // NOTIFICATIONDIALOG_H
