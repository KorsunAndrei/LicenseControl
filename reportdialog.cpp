#include "reportdialog.h"
#include "ui_reportdialog.h"

ReportDialog::ReportDialog(LicenseControl *par) :
    parent(par),
    ui(new Ui::ReportDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags = flags & (~helpFlag);
    setWindowFlags(flags);
    bool connected = parent->connectToDatabase(&db, true, this);
    if(connected){
        getDataFromDatabase();
        parent->disconnectFromDatabase(&db);
    }
}

ReportDialog::~ReportDialog()
{
    delete ui;
}

void ReportDialog::getDataFromDatabase(){
    QStringList columnNames = parent->getColumnsNames("sales");
    int indexDate = columnNames.indexOf("ActivationDate");
    QDate minDate = ui->dateBeginEdit->date();
    QVector<QStringList> retrievedData = parent->executeSelectCommand("sales", columnNames);
    if(!retrievedData.isEmpty()){
        for(int i = 0; i < retrievedData[0].size(); i++){
            QDate tempDate = QDate::fromString(retrievedData[indexDate][i],"yyyy-MM-dd");
            if(tempDate < minDate || minDate == ui->dateBeginEdit->date())
                minDate = tempDate;
        }
        ui->dateBeginEdit->setDate(minDate);
        ui->dateEndEdit->setDate(QDate::currentDate());
    }
}

void ReportDialog::generateReport(){
    QXlsx::Document report(parent->getApplicationPath().remove(parent->getApplicationPath().lastIndexOf('/'), parent->getApplicationPath().length() - parent->getApplicationPath().lastIndexOf('/')) + "\\reportTemplate.xlsx");
    QStringList sheetList = report.sheetNames();
    report.selectSheet(sheetList[0]);
    QString condition = "(sales.ActivationDate BETWEEN ";
    condition += "'" + ui->dateBeginEdit->date().toString(Qt::ISODate) + "'";
    condition += " AND ";
    condition += "'" + ui->dateEndEdit->date().toString(Qt::ISODate) + "')";
    condition += " AND ";
    condition += "licenses.Purchased=1";
    bool connected = parent->connectToDatabase(&db, true, this);
    if(connected){
        QVector<QStringList> retrievedData = parent->executeSelectCommand("sales", "licenses", parent->getColumnsNames("sales"), parent->getColumnsNames("licenses"), "license", condition);
        QStringList columnNames = parent->getColumnsNames("sales");
        columnNames.append(parent->getColumnsNames("licenses"));
        if(!retrievedData.isEmpty()){
            report.write("B2", ui->dateBeginEdit->date().toString(Qt::ISODate));
            report.write("D2", ui->dateEndEdit->date().toString(Qt::ISODate));
            report.write("C4", retrievedData[0].size());
            QStringList softColumnNames = parent->getColumnsNames("software");
            int indexOfID = softColumnNames.indexOf("ID");
            int indexOfTitle = softColumnNames.indexOf("Title");
            int curRow = 6;
            QVector<QStringList> soft = parent->executeSelectCommand("software", softColumnNames);
            if(!soft.isEmpty()){
                for(int i = 0; i < soft[0].size(); i++){
                    int count = 0;
                    for(int j = 0; j < retrievedData[0].size(); j++){
                        if(soft[indexOfID][i] == retrievedData[columnNames.indexOf("Software")][j])
                            count++;
                    }
                    QVector<QStringList> vendor = parent->executeSelectCommand("vendors", parent->getColumnsNames("vendors"), "ID=" + soft[softColumnNames.indexOf("Vendor")][i]);
                    if(count != 0 && !vendor.isEmpty()){
                        report.write("B" + QString::number(curRow), vendor[parent->getColumnsNames("vendors").indexOf("Vendor")][0] + " " + soft[indexOfTitle][i] + ":");
                        report.write("C" + QString::number(curRow++), count);
                    }
                }
            }
            int rowToStore = curRow;
            curRow = 6;
            QStringList customersColumnNames = parent->getColumnsNames("customers");
            indexOfID = customersColumnNames.indexOf("ID");
            QVector<QStringList> customers = parent->executeSelectCommand("customers", customersColumnNames);
            if(!customers.isEmpty()){
                for(int i = 0; i < customers[0].size(); i++){
                    int count = 0;
                    for(int j = 0; j < retrievedData[0].size(); j++){
                        if(customers[indexOfID][i] == retrievedData[columnNames.indexOf("Customer")][j])
                            count++;
                    }
                    if(count != 0){
                        report.write("D" + QString::number(curRow), customers[customersColumnNames.indexOf("Surname")][i] + " " + customers[customersColumnNames.indexOf("Name")][i].left(1) + "." + customers[customersColumnNames.indexOf("Patronomic")][i].left(1) + (customers[customersColumnNames.indexOf("Patronomic")][i].left(1) == "" ? "" : ".") + ":");
                        report.write("E" + QString::number(curRow++), count);
                    }
                }
            }
            curRow = rowToStore;
            QXlsx::Format format(report.cellAt("B4")->format());
            report.write("B" + QString::number(++curRow), "По инженерам:", format);
            curRow++;
            QStringList engineersColumnNames = parent->getColumnsNames("engineers");
            QVector<QStringList> engineers = parent->executeSelectCommand("engineers", engineersColumnNames);
            indexOfID = engineersColumnNames.indexOf("ID");
            bool transfer = false;
            if(!engineers.isEmpty() && !soft.isEmpty()){
                for(int i = 0; i < engineers[0].size(); i++){
                    bool writeEngineer = true;
                    if(curRow >= 40 && !transfer){
                        curRow = rowToStore + 1;
                        transfer = true;
                    }
                    for(int k = 0; k < soft[0].size(); k++){
                        int count = 0;
                        for(int j = 0; j < retrievedData[0].size(); j++){
                            if(engineers[indexOfID][i] == retrievedData[columnNames.indexOf("Engineer")][j] && retrievedData[columnNames.indexOf("Software")][j] == soft[softColumnNames.indexOf("ID")][k])
                                count++;
                        }
                        if(count != 0){
                            if(writeEngineer){
                                if(!transfer)
                                    report.write("B" + QString::number(curRow++), engineers[engineersColumnNames.indexOf("Surname")][i] + " " + engineers[engineersColumnNames.indexOf("Name")][i].left(1) + "." + engineers[engineersColumnNames.indexOf("Patronomic")][i].left(1) + ".:");
                                else
                                    report.write("D" + QString::number(curRow++), engineers[engineersColumnNames.indexOf("Surname")][i] + " " + engineers[engineersColumnNames.indexOf("Name")][i].left(1) + "." + engineers[engineersColumnNames.indexOf("Patronomic")][i].left(1) + ".:");
                                writeEngineer = false;
                            }
                            QVector<QStringList> vendor = parent->executeSelectCommand("vendors", parent->getColumnsNames("vendors"), "ID=" + soft[softColumnNames.indexOf("Vendor")][k]);
                            if(!transfer){
                                report.write("B" + QString::number(curRow), vendor[parent->getColumnsNames("vendors").indexOf("Vendor")][0] + " " + soft[indexOfTitle][k] + ":");
                                report.write("C" + QString::number(curRow++), count);
                            }
                            else{
                                report.write("D" + QString::number(curRow), vendor[parent->getColumnsNames("vendors").indexOf("Vendor")][0] + " " + soft[indexOfTitle][k] + ":");
                                report.write("E" + QString::number(curRow++), count);
                            }
                        }
                    }
                }
            }

        }
        parent->disconnectFromDatabase(&db);
    }
    QString tempDirectory = QDir::tempPath();
    report.saveAs(tempDirectory + "\\reportToPrint.xlsx");
}

void ReportDialog::on_saveToFileBtn_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить отчет"), "", tr("Excel Workbooks (*.xlsx)"));
    if(!fileName.isEmpty()){
        generateReport();
        QString tempDirectory = QDir::tempPath();
        QXlsx::Document report(tempDirectory + "\\reportToPrint.xlsx");
        report.saveAs(fileName);
    }
}

void ReportDialog::on_printBtn_clicked()
{
    generateReport();
    QString tempDirectory = QDir::tempPath();
    parent->printDocument(tempDirectory + "\\reportToPrint.xlsx");
}
