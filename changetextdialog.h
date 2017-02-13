#ifndef CHANGETEXTDIALOG_H
#define CHANGETEXTDIALOG_H

#include <QDialog>
#include <licensecontrol.h>
class LicenseControl;

namespace Ui {
class ChangeTextDialog;
}

class ChangeTextDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeTextDialog(QString textToChange, LicenseControl *par = 0);
    ~ChangeTextDialog();

private slots:
    void on_buttonBox_accepted();

private:
    LicenseControl * parent;
    Ui::ChangeTextDialog *ui;
    QString typeOfNotification;
};

#endif // CHANGETEXTDIALOG_H
