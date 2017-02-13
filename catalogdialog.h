#ifndef CATALOGDIALOG_H
#define CATALOGDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <licensecontrol.h>
class LicenseControl;

namespace Ui {
class CatalogDialog;
}

class CatalogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CatalogDialog(LicenseControl *par = 0);
    ~CatalogDialog();

private slots:
    void getDataFromDatabase();
    void getData();

    void on_CatalogComboBox_currentIndexChanged(int index);

    void on_NewRecordBtn_clicked();

    void on_saveChangesBtn_clicked();

    void on_SearchEdit_textEdited(const QString &arg1);

    void on_deleteRecordBtn_clicked();

private:
    QSqlDatabase db;
    LicenseControl * parent;
    Ui::CatalogDialog *ui;
    QStringList columns;
    QVector<QStringList> retrievedData;
};

#endif // CATALOGDIALOG_H
