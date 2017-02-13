
#include "catalogdialog.h"
#include "ui_catalogdialog.h"

CatalogDialog::CatalogDialog(LicenseControl *par) :
    parent(par),
    ui(new Ui::CatalogDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags = flags & (~helpFlag);
    setWindowFlags(flags);
    getDataFromDatabase();
    ui->DialogBtnBox->button(QDialogButtonBox::Ok)->setText("Закрыть");
}

CatalogDialog::~CatalogDialog()
{
    delete ui;
}

void CatalogDialog::getDataFromDatabase(){
    ui->CatalogComboBox->addItem("Типы", "types");
    ui->CatalogComboBox->addItem("Производители", "vendors");
    ui->CatalogComboBox->addItem("ПО", "software");
    ui->CatalogComboBox->addItem("Инженеры", "engineers");
    ui->CatalogComboBox->addItem("Покупатели", "customers");
}

void CatalogDialog::on_CatalogComboBox_currentIndexChanged(int index)
{
    ui->CatalogWidget->setColumnCount(0);
    ui->CatalogWidget->clear();
    columns.clear();
    bool disconnect = false;
    bool connected = true;
    if(!db.isOpen()){
        disconnect = true;
        connected = parent->connectToDatabase(&db, false, this);
    }
    if(connected){
        QSqlQuery showQuery;
        if(showQuery.exec("SHOW CREATE TABLE " + ui->CatalogComboBox->currentData().toString())){
            showQuery.first();
            for(int i = 0; i < showQuery.record().count(); i++){
                if(showQuery.record().fieldName(i) == "Create Table"){
                    QStringList showStrList = showQuery.value(i).toString().split("\n");
                    //QStringList queryResult = showStrList;
                    int columnNumber = 0;
                    for(int j = 1; j < showStrList.size(); j++){
                        if(!showStrList[j].contains("PRIMARY KEY")){
                            showStrList[j] = showStrList[j].trimmed();
                            if(showStrList[j].contains("ID", Qt::CaseSensitive))
                                continue;
                            ui->CatalogWidget->setColumnCount(ui->CatalogWidget->columnCount() + 1);
                            columns.append(showStrList[j].remove("`").split(" ")[0]);
                            QString headerText = showStrList[j].remove(0, showStrList[j].indexOf("COMMENT") + 8).remove("'").remove(",");
                            QTableWidgetItem * item = new QTableWidgetItem(headerText);
                            ui->CatalogWidget->setHorizontalHeaderItem(columnNumber++, item);
                        }
                        else{
                            break;
                        }
                    }
                    getData();
                }
            }
        }
        else{
            QMessageBox::critical(this, "Ошибка", "Ошибка чтения базы данных", QMessageBox::Ok);
            qDebug() << db.lastError().text();
        }
        if(disconnect)
            parent->disconnectFromDatabase(&db);
    }
    else{
        QMessageBox::critical(this, "Ошибка", "Ошибка подключения к базе данных", QMessageBox::Ok);
        qDebug() << db.lastError().text();
    }
}

void CatalogDialog::on_NewRecordBtn_clicked()
{
    ui->CatalogWidget->setRowCount(ui->CatalogWidget->rowCount() + 1);
    QTableWidgetItem * item = new QTableWidgetItem("");
    ui->CatalogWidget->setVerticalHeaderItem(ui->CatalogWidget->rowCount() - 1, item);
    if(ui->CatalogComboBox->currentData().toString() == "software"){
        bool connected = parent->connectToDatabase(&db, false, this);
        if(connected){
            QComboBox * box = new QComboBox();
            QVector<QStringList> vendors = parent->executeSelectCommand("vendors", parent->getColumnsNames("vendors"));
            if(!vendors.isEmpty()){
                for(int i = 0; i < vendors[0].size(); i++){
                    box->addItem(vendors[parent->getColumnsNames("vendors").indexOf("Vendor")][i], vendors[parent->getColumnsNames("vendors").indexOf("ID")][i]);
                }
                ui->CatalogWidget->setCellWidget(ui->CatalogWidget->rowCount() - 1, parent->getColumnsNames("software").indexOf("Vendor") - 1, box);
            }
        }
        parent->disconnectFromDatabase(&db);
    }
}

void CatalogDialog::getData(){
    retrievedData.clear();
    ui->CatalogWidget->setRowCount(0);
    retrievedData = parent->executeSelectCommand(ui->CatalogComboBox->currentData().toString(), parent->getColumnsNames(ui->CatalogComboBox->currentData().toString()));
    if(!retrievedData.isEmpty()){
        if(!retrievedData[0].isEmpty()){
            for(int j = 0; j < retrievedData.size(); j++){
                for(int k = 0; k < retrievedData[0].size(); k++){
                    if(ui->CatalogWidget->rowCount() < k + 1){
                        ui->CatalogWidget->setRowCount(k + 1);
                        QTableWidgetItem * item = new QTableWidgetItem("");
                        ui->CatalogWidget->setVerticalHeaderItem(ui->CatalogWidget->rowCount() - 1, item);
                    }
                    if(parent->getColumnsNames(ui->CatalogComboBox->currentData().toString())[j] == "Vendor" && ui->CatalogComboBox->currentData().toString() == "software"){
                        QComboBox * box = new QComboBox();
                        QVector<QStringList> vendors = parent->executeSelectCommand("vendors", parent->getColumnsNames("vendors"));
                        if(!vendors.isEmpty()){
                            for(int i = 0; i < vendors[0].size(); i++){
                                box->addItem(vendors[parent->getColumnsNames("vendors").indexOf("Vendor")][i], vendors[parent->getColumnsNames("vendors").indexOf("ID")][i]);
                            }
                            for(int i = 0; i < box->count(); i++){
                                if(box->itemData(i).toString() == retrievedData[j][k]){
                                    box->setCurrentIndex(i);
                                    break;
                                }
                            }
                            ui->CatalogWidget->setCellWidget(k, j - 1, box);
                        }
                        continue;
                    }
                    QTableWidgetItem * item = new QTableWidgetItem(retrievedData[j][k]);
                    if(parent->getColumnsNames(ui->CatalogComboBox->currentData().toString())[j] == "ID")
                        ui->CatalogWidget->setVerticalHeaderItem(k, item);
                    else
                        ui->CatalogWidget->setItem(k, j - 1, item);
                }
            }
        }
    }
}

void CatalogDialog::on_saveChangesBtn_clicked()
{
    bool connected = true;
    connected = parent->connectToDatabase(&db, false, this);
    if(connected){
        for(int i = 0; i < ui->CatalogWidget->rowCount(); i++){
            if(ui->CatalogWidget->verticalHeaderItem(i)->text() == ""){
                QStringList columnNames;
                QStringList values;
                for(int j = 0; j < ui->CatalogWidget->columnCount(); j++){
                    columnNames.append(columns[j]);
                    if(ui->CatalogComboBox->currentData().toString() == "software" && j == parent->getColumnsNames("software").indexOf("Vendor") - 1)
                        values.append(((QComboBox*)ui->CatalogWidget->cellWidget(i, j))->currentData().toString());
                    else{
                        if(ui->CatalogWidget->item(i, j) != 0)
                            values.append(ui->CatalogWidget->item(i, j)->text());
                        else
                            values.append("");
                    }
                }
                parent->executeInsertCommand(ui->CatalogComboBox->currentData().toString(), columnNames, values);
            }
            else{
                QStringList columnNames;
                QStringList values;
                for(int j = 0; j < ui->CatalogWidget->columnCount(); j++){
                    columnNames.append(columns[j]);
                    if(ui->CatalogComboBox->currentData().toString() == "software" && j == parent->getColumnsNames("software").indexOf("Vendor") - 1)
                        values.append(((QComboBox*)ui->CatalogWidget->cellWidget(i, j))->currentData().toString());
                    else{
                        if(ui->CatalogWidget->item(i, j) != 0)
                            values.append(ui->CatalogWidget->item(i, j)->text());
                        else
                            values.append("");
                    }
                }
                parent->executeUpdateCommand(ui->CatalogComboBox->currentData().toString(), columnNames, values, "ID=" + ui->CatalogWidget->verticalHeaderItem(i)->text());
            }
        }
        getData();
        parent->disconnectFromDatabase(&db);
    }
    else
        parent->disconnectFromDatabase(&db);
}

void CatalogDialog::on_SearchEdit_textEdited(const QString &arg1)
{
    ui->CatalogWidget->setRowCount(0);
    if(arg1.isEmpty()){
        if(!retrievedData.isEmpty()){
            if(!retrievedData[0].isEmpty()){
                for(int j = 0; j < retrievedData.size(); j++){
                    for(int k = 0; k < retrievedData[0].size(); k++){
                        if(ui->CatalogWidget->rowCount() < k + 1){
                            ui->CatalogWidget->setRowCount(k + 1);
                            QTableWidgetItem * item = new QTableWidgetItem("");
                            ui->CatalogWidget->setVerticalHeaderItem(ui->CatalogWidget->rowCount() - 1, item);
                        }
                        bool connected = parent->connectToDatabase(&db, false, this);
                        if(connected){
                            if(parent->getColumnsNames(ui->CatalogComboBox->currentData().toString())[j] == "Vendor" && ui->CatalogComboBox->currentData().toString() == "software"){
                                QComboBox * box = new QComboBox();
                                QVector<QStringList> vendors = parent->executeSelectCommand("vendors", parent->getColumnsNames("vendors"));
                                if(!vendors.isEmpty()){
                                    for(int i = 0; i < vendors[0].size(); i++){
                                        box->addItem(vendors[parent->getColumnsNames("vendors").indexOf("Vendor")][i], vendors[parent->getColumnsNames("vendors").indexOf("ID")][i]);
                                    }
                                    for(int i = 0; i < box->count(); i++){
                                        if(box->itemData(i).toString() == retrievedData[j][k]){
                                            box->setCurrentIndex(i);
                                            break;
                                        }
                                    }
                                    ui->CatalogWidget->setCellWidget(k, j - 1, box);
                                }
                                continue;
                            }
                            parent->disconnectFromDatabase(&db);
                        }
                        QTableWidgetItem * item = new QTableWidgetItem(retrievedData[j][k]);
                        if(ui->CatalogWidget->columnCount() < retrievedData.size() && j == 0)
                            ui->CatalogWidget->setVerticalHeaderItem(k, item);
                        else
                            ui->CatalogWidget->setItem(k, j - 1, item);
                    }
                }
            }
        }
        return;
    }
    bool isID = false;
    if(!retrievedData.isEmpty()){
        for(int i = 0; i < retrievedData[0].size(); i++)
            for(int j = 0; j < retrievedData.size(); j++){
                if(ui->CatalogWidget->columnCount() < retrievedData.size() && j == 0){
                    isID = true;
                    continue;
                }
                bool connected = parent->connectToDatabase(&db, false, this);
                if(connected){
                    QVector<QStringList> tempData = parent->executeSelectCommand("Vendors", QString("Vendor").split(";"), "ID=" + retrievedData[parent->getColumnsNames("software").indexOf("Vendor")][i]);
                    QString Vendor;
                    if(tempData.isEmpty() || tempData[0].isEmpty())
                        Vendor = "";
                    else
                        Vendor = tempData[0][0];
                    if(((retrievedData[j][i].contains(arg1, Qt::CaseInsensitive)) && \
                            (!(ui->CatalogComboBox->currentData().toString() == "software" && parent->getColumnsNames("software").indexOf("Vendor") == j))) || \
                            (ui->CatalogComboBox->currentData().toString() == "software" && parent->getColumnsNames("software").indexOf("Vendor") == j && Vendor.contains(arg1, Qt::CaseInsensitive))){
                        ui->CatalogWidget->setRowCount(ui->CatalogWidget->rowCount() + 1);
                        int curCol = 0;
                        for(int k = 0; k < retrievedData.size(); k++){
                            if(isID && k == 0){
                                QTableWidgetItem * item = new QTableWidgetItem(retrievedData[k][i]);
                                ui->CatalogWidget->setVerticalHeaderItem(ui->CatalogWidget->rowCount() - 1, item);
                                continue;
                            }
                                if(parent->getColumnsNames(ui->CatalogComboBox->currentData().toString())[k] == "Vendor" && ui->CatalogComboBox->currentData().toString() == "software"){
                                    QComboBox * box = new QComboBox();
                                    QVector<QStringList> vendors = parent->executeSelectCommand("vendors", parent->getColumnsNames("vendors"));
                                    if(!vendors.isEmpty()){
                                        for(int m = 0; m < vendors[0].size(); m++){
                                            box->addItem(vendors[parent->getColumnsNames("vendors").indexOf("Vendor")][m], vendors[parent->getColumnsNames("vendors").indexOf("ID")][m]);
                                        }
                                        for(int m = 0; m < box->count(); m++){
                                            if(box->itemData(m).toString() == retrievedData[k][i]){
                                                box->setCurrentIndex(m);
                                                break;
                                            }
                                        }
                                        ui->CatalogWidget->setCellWidget(ui->CatalogWidget->rowCount() - 1, curCol++, box);
                                    }
                                    continue;
                                }
                            QTableWidgetItem * item = new QTableWidgetItem(retrievedData[k][i]);
                            ui->CatalogWidget->setItem(ui->CatalogWidget->rowCount() - 1, curCol++, item);
                        }
                        break;
                    }
                parent->disconnectFromDatabase(&db);
                }
            }
    }
    return;
}

void CatalogDialog::on_deleteRecordBtn_clicked()
{
    if(QMessageBox::question(this, "Подтверждение", "Действительно удалить данный элемент? Данное действие невозможно отменить. Все несохраненные изменения будут потеряны.", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes){
        if(ui->CatalogWidget->verticalHeaderItem(ui->CatalogWidget->currentRow())->text() == ""){
            ui->CatalogWidget->removeRow(ui->CatalogWidget->currentRow());
            QMessageBox::information(this, "Завершено", "Запись успешно удалена");
        }
        else{
            bool connected = true;
            connected = parent->connectToDatabase(&db, false, this);
            if(connected){
                if(parent->executeDeleteCommand(ui->CatalogComboBox->currentData().toString(), "ID=" + ui->CatalogWidget->verticalHeaderItem(ui->CatalogWidget->currentRow())->text()))
                    QMessageBox::information(this, "Завершено", "Запись успешно удалена");
                else
                    QMessageBox::information(this, "Ошибка", "Произошла ошибка при удалении записи");
            }
        }
    }
    getData();
}
