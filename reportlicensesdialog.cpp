#include "reportlicensesdialog.h"
#include "ui_reportlicensesdialog.h"

reportLicensesDialog::reportLicensesDialog(LicenseControl *par) :
    parent(par),
    ui(new Ui::reportLicensesDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags = flags & (~helpFlag);
    setWindowFlags(flags);
}

reportLicensesDialog::~reportLicensesDialog()
{
    delete ui;
}

void reportLicensesDialog::generateReport(){
    QXlsx::Document report(parent->getApplicationPath().remove(parent->getApplicationPath().lastIndexOf('/'), parent->getApplicationPath().length() - parent->getApplicationPath().lastIndexOf('/')) + "\\reportLicensesTemplate.xlsx");
    int curRow = 2;
    int curCol = 1;
    if(parent->connectToDatabase(&db, true, this)){
        QString queryString = "SELECT software.Title, types.Type, licenses.PCCount, licenses.Duration, COUNT(licenses.ID) AS licenseCount\
                FROM licenses\
                JOIN software ON software.ID=licenses.Software\
                JOIN types ON types.ID=licenses.Type\
                WHERE licenses.Purchased=0\
                GROUP BY licenses.Software, licenses.Type, licenses.PCCount, licenses.Duration";
        QSqlQuery selectQuery;
        qDebug() << queryString;
        bool success_query = selectQuery.exec(queryString);
        QVector<QStringList> retrievedData;
        if(success_query){
            for(int i = 0; i < 5; i++)// 5 - количество столбцов в запросе
                retrievedData.resize(retrievedData.size() + 1);
            while(selectQuery.next()){
                for(int columnSize = 0; columnSize < retrievedData.size(); columnSize++)
                    retrievedData[columnSize].append(selectQuery.value(columnSize).toString());
            }
            for(int j = 0; j < retrievedData[0].size(); j++){
                curCol = 1;
                for(int i = 0; i < retrievedData.size(); i++){
                    switch(i){
                    case 0:
                        report.write(curRow, curCol++, retrievedData[i][j]);
                        break;
                    case 1:
                        report.write(curRow, curCol++, retrievedData[i][j]);
                        break;
                    case 2:
                        report.write(curRow, curCol++, retrievedData[i][j]);
                        break;
                    case 3:
                        if(retrievedData[i][j] == "0")
                            report.write(curRow, curCol++, "бессрочная");
                        else
                            report.write(curRow, curCol++, retrievedData[i][j] + " мес.");
                        break;
                    case 4:
                        report.write(curRow, curCol++, retrievedData[i][j] + " шт.");
                        break;
                    }
                }
                curRow++;
            }
        }
    }
    parent->disconnectFromDatabase(&db);
    QString tempDirectory = QDir::tempPath();
    report.saveAs(tempDirectory + "\\reportToPrint.xlsx");
}

void reportLicensesDialog::on_toFIleBtn_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить отчет"), "", tr("Excel Workbooks (*.xlsx)"));
    if(!fileName.isEmpty()){
        generateReport();
        QString tempDirectory = QDir::tempPath();
        QXlsx::Document report(tempDirectory + "\\reportToPrint.xlsx");
        if(!report.saveAs(fileName))
            QMessageBox::critical(this, "Ошибка", "Произошла ошибка при сохранении файла. Возможно файл уже используется или диск защищен от записи.", QMessageBox::Ok);
    }
}

void reportLicensesDialog::on_printBtn_clicked()
{
    generateReport();
    QString tempDirectory = QDir::tempPath();
    parent->printDocument(tempDirectory + "\\reportToPrint.xlsx");
}
