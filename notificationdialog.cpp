#include "notificationdialog.h"
#include "ui_notificationdialog.h"

NotificationDialog::NotificationDialog(LicenseControl *par) :
    parent(par),
    ui(new Ui::NotificationDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags = flags & (~helpFlag);
    setWindowFlags(flags);
    ui->tableWidget->setColumnWidth(0, ui->tableWidget->width());
    bool connected = parent->connectToDatabase(&db, true, this);
    if(connected){
        getDataFromDatabase();
        parent->disconnectFromDatabase(&db);
    }
    this->activateWindow();
    this->raise();
    this->setFocus();
}

NotificationDialog::~NotificationDialog()
{
    delete ui;
}

void NotificationDialog::getDataFromDatabase(){
    ui->tableWidget->setRowCount(0);
    bool disconnect = false;
    bool connected = true;
    if(!db.isOpen()){
        disconnect = true;
        connected = parent->connectToDatabase(&db, false, this);
    }
    if(connected){
        for(int i = 0; i < parent->cNotifications.size(); i++){
            QLabel * item = new QLabel;
            QStringList columnNames = parent->getColumnsNames("Customers");
            QVector<QStringList> retrievedData = parent->executeSelectCommand("Customers", columnNames, "ID=" + QString::number(parent->cNotifications[i].customer));
            if(!retrievedData.isEmpty()){
                if(!retrievedData[0].isEmpty()){
                    item->setText(retrievedData[columnNames.indexOf("Surname")][0] + " " + retrievedData[columnNames.indexOf("Name")][0] + " " + retrievedData[columnNames.indexOf("Patronomic")][0] + "\nТелефонный номер: (" + retrievedData[columnNames.indexOf("PhoneCode")][0] + ")" + retrievedData[columnNames.indexOf("PhoneNumber")][0] + "\nПО: " + parent->cNotifications[i].soft + "\nДата истечения лицензии: " + parent->cNotifications[i].date);
                    QFontMetrics fontMetrics = item->fontMetrics();
                    QSize textSize = fontMetrics.size(0, item->text());
                    int textWidth = textSize.width() + 30;
                    int textHeight = textSize.height() + 20;
                    item->setMinimumSize(textWidth, textHeight);
                    item->resize(textWidth, textHeight);
                    ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
                    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 0, item);
                    ui->tableWidget->setRowHeight(ui->tableWidget->rowCount() - 1, textHeight);
                }
            }
        }
        for(int i = 0; i < parent->lNotifications.size(); i++){
            QLabel * item = new QLabel;
            item->setText("ПО: " + parent->lNotifications[i].soft + "\nОсталось лицензий: " + QString::number(parent->lNotifications[i].count));
            QFontMetrics fontMetrics = item->fontMetrics();
            QSize textSize = fontMetrics.size(0, item->text());
            int textWidth = textSize.width() + 30;
            int textHeight = textSize.height() + 20;
            item->setMinimumSize(textWidth, textHeight);
            item->resize(textWidth, textHeight);
            ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
            ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 0, item);
            ui->tableWidget->setRowHeight(ui->tableWidget->rowCount() - 1, textHeight);
        }
    }
    if(disconnect)
        parent->disconnectFromDatabase(&db);
}

void NotificationDialog::on_tableWidget_cellDoubleClicked(int row, int column)
{
    QSqlDatabase db;
    bool connected = parent->connectToDatabase(&db, false, this);
    if(connected){
        QLabel * label = (QLabel *)ui->tableWidget->cellWidget(row, column);
        QString text = label->text();
        if(text.contains("Осталось лицензий:")){
            return;
        }
        else{
            TrackingDialog dialog(parent, parent->cNotifications[row].customer, parent->cNotifications[row].date, parent->cNotifications[row].license);
            dialog.setModal(true);
            dialog.exec();
            parent->checkCustomer(false);
            }
    }
    parent->disconnectFromDatabase(&db);
    getDataFromDatabase();
}
