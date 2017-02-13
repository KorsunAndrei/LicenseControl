#ifndef CHANGELICENSECOUNTDIALOG_H
#define CHANGELICENSECOUNTDIALOG_H

#include <QDialog>
#include <licensecontrol.h>
class LicenseControl;

namespace Ui {
class ChangeLicenseCountDialog;
}

class ChangeLicenseCountDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeLicenseCountDialog(LicenseControl *par = 0);
    ~ChangeLicenseCountDialog();

private slots:
    void on_toolButton_clicked();

    void done(int r);

    bool isCorrect();
    void commitChanges();

    void getData();

private:
    QSqlDatabase db;
    Ui::ChangeLicenseCountDialog *ui;
    LicenseControl * parent;
};

#endif // CHANGELICENSECOUNTDIALOG_H
