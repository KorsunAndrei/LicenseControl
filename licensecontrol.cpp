#include "licensecontrol.h"

LicenseControl::LicenseControl(QApplication * app){
    application = app;
    trayIcon = new QSystemTrayIcon(this);
    setTrayIconActions();
    QIcon trayImage(":/images/favicon.ico");
    trayIcon->setIcon(trayImage);
    QObject::connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    QObject::connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(notifications()));
    trayIcon->setToolTip("Программа управления продажами лицензий ПО");
    getSettings();
    setSettings();
    logoFolder = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + QCoreApplication::organizationName() + "/" + QCoreApplication::applicationName() + "/logos/";
    QSettings settings;
    customerTimer = new QTimer();
    licenseTimer = new QTimer();
    customerTimer->stop();
    licenseTimer->stop();
    if(!settings.contains("First_Launch"))
        configAction->activate(QAction::ActionEvent());
    settings.setValue("First_Launch", "0");
    checkCustomer(false);
    checkLicense(false);
    connect(customerTimer, SIGNAL(timeout()), this, SLOT(checkCustomer()));
    customerTimer->setInterval(1000 * 60 * customerNotificationSettings.interval);
    customerTimer->start();
    connect(licenseTimer, SIGNAL(timeout()), this, SLOT(checkLicense()));
    licenseTimer->setInterval(1000 * 60 * licenseNotificationSettings.interval);
    licenseTimer->start();
    setPrintFields();
}

LicenseControl::~LicenseControl()
{
    trayIcon->~QSystemTrayIcon();
}

void LicenseControl::trayIconActivated(QSystemTrayIcon::ActivationReason reason){
    switch (reason){
    case QSystemTrayIcon::Trigger:
        trayIcon->contextMenu()->show();
        break;
    default:
        break;
    }
}

void LicenseControl::setTrayIconActions(){
    exitAction = new QAction("Выход", this);
    QObject::connect(exitAction, SIGNAL(triggered()), application, SLOT(quit()));
    addLicenseAction = new QAction("Добавить лицензию...", this);
    QObject::connect(addLicenseAction, SIGNAL(triggered()), this, SLOT(addLicense()));
    sellAction = new QAction("Продать лицензию...", this);
    QObject::connect(sellAction, SIGNAL(triggered()), this, SLOT(sellLicense()));
    cancelAction = new QAction("Отменить продажу...", this);
    QObject::connect(cancelAction, SIGNAL(triggered()), this, SLOT(cancelOrder()));
    catalogAction = new QAction("Справочники", this);
    QObject::connect(catalogAction, SIGNAL(triggered()), this, SLOT(showCatalog()));
    customersAction = new QAction("Клиенты и лицензии", this);
    QObject::connect(customersAction, SIGNAL(triggered()), this, SLOT(showClients()));
    softAction = new QAction("Добавить/изменить ПО...", this);
    QObject::connect(softAction, SIGNAL(triggered()), this, SLOT(showSoftEdit()));
    configAction = new QAction("Настройка", this);
    QObject::connect(configAction, SIGNAL(triggered()), this, SLOT(configuration()));
    aboutAction = new QAction("О программе...", this);
    QObject::connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));
    reportAction = new QAction("Отчет по продажам", this);
    QObject::connect(reportAction, SIGNAL(triggered()), this, SLOT(generateReport()));
    licensesReportAction = new QAction("Отчет по оставшимся лицензиям", this);
    QObject::connect(licensesReportAction, SIGNAL(triggered()), this, SLOT(reportLicenses()));
    notificationAction = new QAction("Уведомления", this);
    QObject::connect(notificationAction, SIGNAL(triggered()), this, SLOT(notifications()));
    //Создание меню
    trayIconMenu = new QMenu();
    trayIconMenu->addAction(addLicenseAction);
    trayIconMenu->addAction(sellAction);
    trayIconMenu->addAction(cancelAction);
    trayIconMenu->addAction(reportAction);
    trayIconMenu->addAction(licensesReportAction);
    trayIconMenu->addAction(softAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(notificationAction);
    trayIconMenu->addAction(customersAction);
    trayIconMenu->addAction(catalogAction);
    trayIconMenu->addAction(configAction);
    trayIconMenu->addAction(aboutAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(exitAction);
    //Устанавливаем данное меню для иконки в трее
    trayIcon->setContextMenu(trayIconMenu);
}

void LicenseControl::addLicense(){
    AddLicenseDialog dialog(this);
    dialog.setModal(true);
    dialog.exec();
}

void LicenseControl::sellLicense(){
    SellDialog dialog(this);
    dialog.setModal(true);
    dialog.exec();
}

void LicenseControl::cancelLicense(){
    CancelDialog dialog(this);
    dialog.setModal(true);
    dialog.exec();
}

bool LicenseControl::connectToDatabase(QSqlDatabase * db, bool toClose, QDialog * dialog){
    *db = QSqlDatabase::addDatabase("QMYSQL");
    (*db).setHostName(hostname);
    (*db).setDatabaseName("licensecontrolDB");
    (*db).setUserName(username);
    (*db).setPassword(password);
    while(!(*db).open()){
        qDebug() << (*db).lastError().text();
        int clickedBtn = QMessageBox::critical(0, "Ошибка", "Невозможно подключиться к базе данных\n" + (*db).lastError().text(), QMessageBox::Retry | QMessageBox::Cancel, QMessageBox::Cancel);
        switch(clickedBtn){
        case QMessageBox::Cancel:
            qDebug() << (*db).lastError().text();
            if(toClose)
                QApplication::postEvent(dialog, new QCloseEvent());
            return false;
            break;
        case QMessageBox::Retry:
            continue;
            break;
        }
    }
    return true;
}

void LicenseControl::configuration(){
    ConfigDialog dialog(this);
    dialog.setModal(true);
    dialog.exec();
}

void LicenseControl::cancelOrder(){
    CancelDialog dialog(this);
    dialog.setModal(true);
    dialog.exec();
}

void LicenseControl::showCatalog(){
    CatalogDialog dialog(this);
    dialog.setModal(true);
    dialog.exec();
}

void LicenseControl::showTrayIcon(){
    trayIcon->show();
    if(lNotifications.size() + cNotifications.size() > 0){
        if(customerNotificationSettings.sound || licenseNotificationSettings.sound){
            QSound::play(soundPath);
        }
        showNotification("Уведомления", "Количество: " + QString::number(lNotifications.size() + cNotifications.size()), QSystemTrayIcon::Information, 5);
    }
}

void LicenseControl::disconnectFromDatabase(QSqlDatabase * db){
    if((*db).isOpen()){
        QString connName = "qt_sql_default_connection";
        db->close();
    }
}

int LicenseControl::commitChangesDialog(){
    return QMessageBox::question(0, "Сохранить изменения", "Вы действительно хотите сохранить изменения?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
}

QStringList LicenseControl::getColumnsNames(QString table){
    QStringList tempStrList;
    QSqlQuery getColumnNamesQuery;
    bool success_query;
    success_query = getColumnNamesQuery.exec("SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS WHERE table_name = '" + table + "'");
    if(success_query){
        while(getColumnNamesQuery.next())
            tempStrList.append(getColumnNamesQuery.value(0).toString());
        return tempStrList;
    }
    else
        return tempStrList;
}

bool LicenseControl::executeInsertCommand(QString table, QStringList columnNames, QStringList values){
    if(columnNames.isEmpty())
        return false;
    QSqlQuery insertQuery;
    QString queryString;
    queryString = "INSERT INTO `" + table + "` (";
    queryString.append("`" + columnNames[0] + "`");
    for(int i = 1; i < columnNames.size(); i++){
        queryString.append(", `" + columnNames[i] + "`");
    }
    queryString.append(") VALUES (");
    queryString.append("'" + values[0] + "'");
    for(int i = 1; i < values.size(); i++){
        queryString.append(", '" + values[i] + "'");
    }
    queryString.append(");");
    qDebug() << queryString;
    return insertQuery.exec(queryString);
}

QVector<QStringList> LicenseControl::executeSelectCommand(QString table, QStringList columnNames){
    QVector<QStringList> temp;
    if(columnNames.isEmpty())
        return temp;
    QSqlQuery selectQuery;
    QString queryString = "SELECT ";
    queryString.insert(queryString.size(), "`" + columnNames[0] + "`");
    for(int i = 1; i < columnNames.size(); i++){
        queryString.insert(queryString.size(), ", `" + columnNames[i] + "`");
    }
    queryString.insert(queryString.size(), " FROM " + table);
    qDebug() << queryString;
    bool success_query = selectQuery.exec(queryString);
    if(success_query){
        temp.resize(columnNames.size());
        while(selectQuery.next()){
            for(int columnSize = 0; columnSize < temp.size(); columnSize++)
                temp[columnSize].append(selectQuery.value(columnSize).toString());
        }
    }
    return temp;
}

QVector<QStringList> LicenseControl::executeSelectCommand(QString table, QStringList columnNames, QString condition){
    QVector<QStringList> temp;
    if(columnNames.isEmpty())
        return temp;
    QSqlQuery selectQuery;
    QString queryString = "SELECT ";
    queryString.insert(queryString.size(), "`" + columnNames[0] + "`");
    for(int i = 1; i < columnNames.size(); i++){
        queryString.insert(queryString.size(), ", `" + columnNames[i] + "`");
    }
    queryString.insert(queryString.size(), " FROM " + table);
    if(!condition.isEmpty()){
        queryString.insert(queryString.size(), " WHERE " + condition);
    }
    qDebug() << queryString;
    bool success_query = selectQuery.exec(queryString);
    if(success_query){
        for(int i = 0; i < columnNames.size(); i++)
            temp.resize(temp.size() + 1);
        while(selectQuery.next()){
            for(int columnSize = 0; columnSize < temp.size(); columnSize++)
                temp[columnSize].append(selectQuery.value(columnSize).toString());
        }
    }
    return temp;
}

QString LicenseControl::getIDof(QString table, QString column, QString searchString){
    QVector<QStringList> temp = executeSelectCommand(table, getColumnsNames(table), column + "='" + searchString + "'");
    return temp[0][0];
}

bool LicenseControl::executeUpdateCommand(QString table, QStringList columnNames, QStringList values, QString condition){
    if(columnNames.isEmpty())
        return false;
    QSqlQuery updateQuery;
    QString queryString;
    queryString = "UPDATE `" + table + "`";
    queryString.append(" SET "  + columnNames[0] + "=" + "'" + values[0] + "'");
    for(int i = 1; i < columnNames.size(); i++){
        queryString.append(", " + columnNames[i] + "=" + "'" + values[i] + "'");
    }
    if(!condition.isEmpty())
        queryString.append(" WHERE " + condition);
    qDebug() << queryString;
    return updateQuery.exec(queryString);
}

void LicenseControl::showAbout(){
    AboutDialog dialog(this);
    dialog.setModal(true);
    dialog.exec();
}

QVector<QStringList> LicenseControl::executeSelectCommand(QString table1, QString table2, QStringList columnNames1, QStringList columnNames2, QString joinField, QString condition){
    QVector<QStringList> temp;
    if(columnNames1.isEmpty() || columnNames2.isEmpty())
        return temp;
    QSqlQuery selectQuery;
    QString queryString = "SELECT ";
    queryString.insert(queryString.size(), table1 + "." + columnNames1[0]);
    for(int i = 1; i < columnNames1.size(); i++){
        queryString.insert(queryString.size(), ", " + table1 + "." + columnNames1[i]);
    }
    for(int i = 0; i < columnNames2.size(); i++){
        queryString.insert(queryString.size(), ", " + table2 + "." + columnNames2[i]);
    }
    queryString.insert(queryString.size(), " FROM " + table1);
    queryString.insert(queryString.size(), " JOIN " + table2);
    queryString.insert(queryString.size(), " ON " + table1 + "." + joinField + "=" + table2 + ".ID");
    if(!condition.isEmpty()){
        queryString.insert(queryString.size(), " WHERE " + condition);
    }
    qDebug() << queryString;
    bool success_query = selectQuery.exec(queryString);
    if(success_query){
        temp.resize(columnNames1.size() + columnNames2.size());
        while(selectQuery.next()){
            for(int columnSize = 0; columnSize < temp.size(); columnSize++)
                temp[columnSize].append(selectQuery.value(columnSize).toString());
        }
    }
    return temp;
}

void LicenseControl::showNotification(QString title, QString text, QSystemTrayIcon::MessageIcon icon, int duration){
    trayIcon->showMessage(title, text, icon, duration * 10000);
}

void LicenseControl::getSettings(){
    QCoreApplication::setOrganizationName("IT-Plus");
    QCoreApplication::setApplicationName("License Control");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;
    settings.beginGroup("Database");
    hostname = settings.value("hostname", "localhost").toString();
    username = settings.value("username", "root").toString();
    password = settings.value("password", "").toString();
    settings.endGroup();
    settings.beginGroup("Notifications");
    soundPath = settings.value("sound_file", getApplicationPath().remove(getApplicationPath().lastIndexOf('/'), getApplicationPath().length() - getApplicationPath().lastIndexOf('/')).replace('/', "\\") + "\\notification.wav").toString();
    settings.endGroup();
    settings.beginGroup("Customer_Notification");
    customerNotificationSettings.interval = settings.value("interval", "1").toInt();
    customerNotificationSettings.sound = settings.value("sound", "false").toBool();
    customerNotificationSettings.text = settings.value("text", "Клиент: $CUSTOMER\nПО: $SOFT\nДата окончания лицензии: $DATE").toString();
    daysCount = settings.value("day_count", "3").toInt();
    settings.endGroup();
    settings.beginGroup("License_Notification");
    licenseNotificationSettings.interval = settings.value("interval", "1").toInt();
    licenseNotificationSettings.sound = settings.value("sound", "false").toBool();
    licenseNotificationSettings.text = settings.value("text", "ПО: $SOFT\nКоличество лицензий: $COUNT").toString();
    defaultMinLicenseCount = settings.value("default", "3").toInt();
    settings.endGroup();
    settings.beginGroup("License_Count");
    QStringList softID = settings.allKeys();
    minLicenseCount.resize(softID.size());
    for(int i = 0; i < softID.size(); i++){
        minLicenseCount[i].soft = softID[i].toInt();
        minLicenseCount[i].count = settings.value(softID[i]).toInt();
    }
    settings.endGroup();
}

void LicenseControl::printDocument(QString pathToDocument){
    // Открываем документ
    QPrinter printer;
    QPrintDialog dialog(&printer, 0);
    if(dialog.exec() == QDialog::Accepted){
        QAxObject * excel = new QAxObject("Excel.Application");
        excel->setProperty("DisplayAlerts", "0");
        QAxObject* workbooks = excel->querySubObject( "Workbooks" );
        QAxObject* workbook = workbooks->querySubObject( "Open(const QString&)", pathToDocument );
        QAxObject* sheets = workbook->querySubObject( "Worksheets" );
        // Печатаем
        QVariantList printParameters;
        printParameters << QVariant(printer.fromPage() + 1);
        printParameters << QVariant(printer.toPage() + 1);
        printParameters << QVariant(printer.copyCount());
        printParameters << QVariant(false);
        printParameters << QVariant(printer.printerName());
        printParameters << QVariant(printer.outputFileName() == "" ? false : true);
        printParameters << QVariant(printer.collateCopies());
        printParameters << QVariant(printer.outputFileName());
        bool printSuccessed = sheets->dynamicCall("PrintOut(From:=const QVariant&, To:=const QVariant&, Copies:=const QVariant&, Preview:=const QVariant&, ActivePrinter:=const QVariant&, PrintToFile:=const QVariant&, Collate:=const QVariant&, PrToFileName:=const QVariant&)", printParameters).toBool();
        // Закрываем
        workbook->dynamicCall("Close()");
        excel->dynamicCall("Quit()");
        if(printSuccessed)
            QMessageBox::information(0, "Информация о печати", "Печать завершена успешно", QMessageBox::Ok);
        else
            QMessageBox::critical(0, "Информация о печати", "Произошла ошибка во время печати", QMessageBox::Ok);
    }
}

void LicenseControl::setSettings(){
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;
    settings.beginGroup("Database");
    settings.setValue("hostname", hostname);
    settings.setValue("username", username);
    settings.setValue("password", password);
    settings.endGroup();
    settings.beginGroup("Notifications");
    settings.setValue("sound_file", soundPath);
    settings.endGroup();
    settings.beginGroup("Customer_Notification");
    settings.setValue("interval", customerNotificationSettings.interval);
    settings.setValue("sound", customerNotificationSettings.sound);
    settings.setValue("text", customerNotificationSettings.text);
    settings.setValue("day_count", daysCount);
    settings.endGroup();
    settings.beginGroup("License_Notification");
    settings.setValue("interval", licenseNotificationSettings.interval);
    settings.setValue("sound", licenseNotificationSettings.sound);
    settings.setValue("text", licenseNotificationSettings.text);
    settings.setValue("default", defaultMinLicenseCount);
    settings.endGroup();
    settings.beginGroup("License_Count");
    settings.remove("");
    for(int i = 0; i < minLicenseCount.size(); i++){
        settings.setValue(QString::number(minLicenseCount[i].soft), minLicenseCount[i].count);
    }
    settings.endGroup();
}

QString LicenseControl::getApplicationPath(){
    return application->applicationFilePath();
}

bool LicenseControl::checkStartup(){
    QSettings registrySettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    QString appPath = getApplicationPath().replace('/', "\\");
    for(int i = 0; i < registrySettings.allKeys().size(); i++){
        if(appPath == registrySettings.value(registrySettings.allKeys()[i]).toString()){
            return true;
        }
    }
    return false;
}

QString LicenseControl::getDescription(QString soft){
    QString toReturn = "";
    QSqlDatabase db;
    bool connected = connectToDatabase(&db, false, NULL);
    if(connected){
        QVector<QStringList> retrievedData = executeSelectCommand("Software", getColumnsNames("Software"));
        if(!retrievedData.isEmpty()){
            QStringList columnNames = getColumnsNames("Software");
            int columnNumber = columnNames.indexOf("Description");
            int rowNumber = retrievedData[getColumnsNames("Software").indexOf("Title")].indexOf(soft);
            toReturn = retrievedData[columnNumber][rowNumber];
        }
        disconnectFromDatabase(&db);
    }
    return toReturn;
}

QString LicenseControl::getDescription(int soft){
    QString toReturn = "";
    QSqlDatabase db;
    bool connected = connectToDatabase(&db, false, NULL);
    if(connected){
        QVector<QStringList> retrievedData = executeSelectCommand("Software", getColumnsNames("Software"));
        QVector<QStringList> licenses = executeSelectCommand("licenses", getColumnsNames("licenses"), "ID=" + QString::number(soft));
        if(!retrievedData.isEmpty() && !licenses.isEmpty()){
            QStringList columnNames = getColumnsNames("Software");
            int columnNumber = columnNames.indexOf("Description");
            int rowNumber = retrievedData[getColumnsNames("Software").indexOf("ID")].indexOf(licenses[getColumnsNames("licenses").indexOf("Software")][0]);
            toReturn = retrievedData[columnNumber][rowNumber];
        }
        disconnectFromDatabase(&db);
    }
    return toReturn;
}

void LicenseControl::generateReport(){
    ReportDialog dialog(this);
    dialog.setModal(true);
    dialog.exec();
}

void LicenseControl::checkCustomer(bool show){
    QSqlDatabase db;
    bool connected = connectToDatabase(&db, false, NULL);
    if(connected){
        cNotifications.clear();
        QStringList columnNames = getColumnsNames("sales");
        columnNames.append(getColumnsNames("licenses"));
        int indexOfDate = columnNames.indexOf("ExpirationDate");
        int indexOfCustomer = columnNames.indexOf("Customer");
        int indexOfSoftware = columnNames.indexOf("Software");
        QVector<QStringList> retrievedData = executeSelectCommand("sales", "licenses", getColumnsNames("sales"), getColumnsNames("licenses"), "license", "licenses.Purchased=1 AND sales.Track=1");
        QVector<QStringList> soft = executeSelectCommand("Software", getColumnsNames("Software"));
        int indexOfTitle = getColumnsNames("Software").indexOf("Title");
        int indexOfID = getColumnsNames("Software").indexOf("ID");
        QDate currentDate = QDate::currentDate();
        if(!retrievedData.isEmpty() && !soft.isEmpty()){
            for(int k = 0; k < retrievedData[indexOfDate].size(); k++){
                if(qAbs(currentDate.daysTo(QDate::fromString(retrievedData[indexOfDate][k], Qt::ISODate))) <= daysCount && licenseDoNotTrack.indexOf(retrievedData[columnNames.indexOf("License")][k].toInt()) == -1){
                    QVector<QStringList> vendor = executeSelectCommand("vendors", getColumnsNames("vendors"), "ID=" + soft[getColumnsNames("Software").indexOf("Vendor")][soft[indexOfID].indexOf(retrievedData[indexOfSoftware][k])]);
                    if(!vendor.isEmpty())
                        addCustomerNotification(retrievedData[indexOfCustomer][k].toInt(), vendor[getColumnsNames("vendors").indexOf("Vendor")][0] + " " + soft[indexOfTitle][soft[indexOfID].indexOf(retrievedData[indexOfSoftware][k])], retrievedData[indexOfDate][k], retrievedData[columnNames.indexOf("ID")][k].toInt());
                }
            }
        }
        if(show && cNotifications.size() > 0){
            if(customerNotificationSettings.sound)
                QSound::play(soundPath);
            QString notification;
            QStringList customerColumnNames = getColumnsNames("Customers");
            for(int i = 0; i < cNotifications.size(); i++){
                QVector<QStringList> currentCustomer = executeSelectCommand("Customers", customerColumnNames, "ID=" + QString::number(cNotifications[i].customer));
                if(!currentCustomer.isEmpty()){
                    if(!notification.isEmpty())
                        notification += "\n\n";
                    notification += customerNotificationSettings.text;
                    notification = notification.replace("$CUSTOMER", currentCustomer[customerColumnNames.indexOf("Surname")][0] + " " + currentCustomer[customerColumnNames.indexOf("Name")][0] + " " + currentCustomer[customerColumnNames.indexOf("Patronomic")][0]);
                    notification = notification.replace("$SOFT", cNotifications[i].soft);
                    notification = notification.replace("$DATE", cNotifications[i].date);
                }
            }
            showNotification("Уведомления", notification, QSystemTrayIcon::Information, 10);
        }
        disconnectFromDatabase(&db);
    }

}

void LicenseControl::checkLicense(bool show){
    QSqlDatabase db;
    bool connected = connectToDatabase(&db, false, NULL);
    if(connected){
        lNotifications.clear();
        QStringList licenseColumnNames = getColumnsNames("licenses");
        int indexOfSoftware = licenseColumnNames.indexOf("Software");
        QVector<QStringList> retrievedData = executeSelectCommand("licenses", licenseColumnNames, "Purchased=0");
        QStringList softColumnNames = getColumnsNames("Software");
        QVector<QStringList> soft = executeSelectCommand("Software", softColumnNames);
        int indexOfID = softColumnNames.indexOf("ID");
        if(!retrievedData.isEmpty() && !soft.isEmpty()){
            for(int k = 0; k < soft[indexOfID].size(); k++){
                int count = 0;
                for(int i = 0; i < retrievedData[indexOfSoftware].size(); i++){
                    if(soft[indexOfID][k] == retrievedData[indexOfSoftware][i])
                        count++;
                }
                int m = -1;
                for(int i = 0; i < minLicenseCount.size(); i++){
                    if(minLicenseCount[i].soft == soft[indexOfID][k].toInt())
                        m = i;
                }
                QVector<QStringList> vendor = executeSelectCommand("vendors", getColumnsNames("vendors"), "ID=" + soft[softColumnNames.indexOf("Vendor")][k]);
                if(!vendor.isEmpty()){
                    if(m != -1){
                        if(count <= minLicenseCount[m].count)
                            addLicenseNotification(vendor[getColumnsNames("vendors").indexOf("Vendor")][0] + " " + soft[softColumnNames.indexOf("Title")][k], count);
                    }
                    else{
                        if(count <= defaultMinLicenseCount)
                            addLicenseNotification(vendor[getColumnsNames("vendors").indexOf("Vendor")][0] + " " + soft[softColumnNames.indexOf("Title")][k], count);
                    }
                }
            }
        }
        if(show && lNotifications.size() > 0){
            if(licenseNotificationSettings.sound)
                QSound::play(soundPath);
            QString notification;
            for(int i = 0; i < lNotifications.size(); i++){
                if(!notification.isEmpty())
                    notification += "\n\n";
                notification += licenseNotificationSettings.text;
                notification = notification.replace("$SOFT", lNotifications[i].soft);
                notification = notification.replace("$COUNT", QString::number(lNotifications[i].count));
            }
            showNotification("Уведомления", notification, QSystemTrayIcon::Information, 10);
        }
        disconnectFromDatabase(&db);
    }
}

void LicenseControl::addLicenseNotification(QString soft, int count){
    lNotifications.resize(lNotifications.size() + 1);
    lNotifications[lNotifications.size() - 1].soft = soft;
    lNotifications[lNotifications.size() - 1].count = count;
}

void LicenseControl::addCustomerNotification(int customer, QString soft, QString date, int license){
    cNotifications.resize(cNotifications.size() + 1);
    cNotifications[cNotifications.size() - 1].customer = customer;
    cNotifications[cNotifications.size() - 1].soft = soft;
    cNotifications[cNotifications.size() - 1].date = date;
    cNotifications[cNotifications.size() - 1].license = license;
}

void LicenseControl::notifications(){
    NotificationDialog dialog(this);
    dialog.setModal(true);
    dialog.activateWindow();
    dialog.exec();
}

void LicenseControl::checkCustomer(){
    checkCustomer(true);
}

void LicenseControl::checkLicense(){
    checkLicense(true);
}

void LicenseControl::showSoftEdit(){
    SoftDialog dialog(this);
    dialog.setModal(true);
    dialog.exec();
}

void LicenseControl::showClients(){
    ClientsDialog dialog(this);
    dialog.setModal(true);
    dialog.exec();
}

void LicenseControl::setPrintFields(){
    printFields.clear();
    printFields.resize(10);
    printFields[0].cell = "G12";
    printFields[0].field = "Logo";
    printFields[1].cell = "A13";
    printFields[1].field = "Soft";
    printFields[2].cell = "C14";
    printFields[2].field = "Serial";
    printFields[3].cell = "A16";
    printFields[3].field = "Type";
    printFields[4].cell = "D17";
    printFields[4].field = "Duration";
    printFields[5].cell = "C18";
    printFields[5].field = "PCCount";
    printFields[6].cell = "A31";
    printFields[6].field = "Description";
    printFields[7].cell = "A21";
    printFields[7].field = "Username";
    printFields[8].cell = "A22";
    printFields[8].field = "Password";
}

bool LicenseControl::executeDeleteCommand(QString table, QString condition){
    QSqlQuery deleteQuery;
    QString queryString;
    queryString = "DELETE FROM `" + table + "`";
    if(!condition.isEmpty())
        queryString.append(" WHERE " + condition);
    qDebug() << queryString;
    return deleteQuery.exec(queryString);
}

void LicenseControl::reportLicenses(){
    reportLicensesDialog dialog(this);
    dialog.setModal(true);
    dialog.exec();
}

QVector<QStringList> LicenseControl::executeSelectCommand(QString table, QStringList columnNames, QString aggregationFunction, QString condition, QString groupByColumn){
    QVector<QStringList> temp;
    if(columnNames.isEmpty())
        return temp;
    QSqlQuery selectQuery;
    QString queryString = "SELECT ";
    queryString.insert(queryString.size(), "`" + columnNames[0] + "`");
    for(int i = 1; i < columnNames.size(); i++){
        queryString.insert(queryString.size(), ", `" + columnNames[i] + "`");
    }
    queryString.insert(queryString.size(), ", " + aggregationFunction);
    queryString.insert(queryString.size(), " FROM " + table);
    if(!condition.isEmpty()){
        queryString.insert(queryString.size(), " WHERE " + condition);
    }
    queryString.insert(queryString.size(), " GROUP BY" + groupByColumn);
    qDebug() << queryString;
    bool success_query = selectQuery.exec(queryString);
    if(success_query){
        temp.resize(columnNames.size());
        while(selectQuery.next()){
            for(int columnSize = 0; columnSize < temp.size(); columnSize++)
                temp[columnSize].append(selectQuery.value(columnSize).toString());
        }
    }
    return temp;
}
