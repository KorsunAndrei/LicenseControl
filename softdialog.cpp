#include "softdialog.h"
#include "ui_softdialog.h"

SoftDialog::SoftDialog(LicenseControl *par) :
    parent(par),
    ui(new Ui::SoftDialog)
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
    if(ui->softComboBox->count() == 1){
        QMessageBox::critical(this, "Ошибка", "Список производителей пуст");
        QApplication::postEvent(this, new QCloseEvent());
    }
}

SoftDialog::~SoftDialog()
{
    delete ui;
}

void SoftDialog::getDataFromDatabase(){
    QVector<QStringList> retrievedData = parent->executeSelectCommand("vendors", parent->getColumnsNames("vendors"));
    if(!retrievedData.isEmpty()){
        for(int i = 0; i < retrievedData[0].size(); i++){
            QString text = retrievedData[parent->getColumnsNames("vendors").indexOf("Vendor")][i];
            QString data = retrievedData[parent->getColumnsNames("vendors").indexOf("ID")][i];
            ui->vendorComboBox->addItem(text, data);
        }
    }
}

void SoftDialog::on_vendorComboBox_currentIndexChanged(int index)
{
    ui->softComboBox->clear();
    bool disconnect = false;
    bool connected = true;
    if(!db.isOpen()){
        disconnect = true;
        connected = parent->connectToDatabase(&db, false, this);
    }
    if(connected){
        QVector<QStringList> retrievedData = parent->executeSelectCommand("software", parent->getColumnsNames("software"), "Vendor=" + ui->vendorComboBox->currentData().toString());
        if(!retrievedData.isEmpty()){
            for(int i = 0; i < retrievedData[0].size(); i++){
                QString text = retrievedData[parent->getColumnsNames("software").indexOf("Title")][i];
                QString data = retrievedData[parent->getColumnsNames("software").indexOf("ID")][i];
                ui->softComboBox->addItem(text, data);
            }
        }
    }
    if(disconnect)
        parent->disconnectFromDatabase(&db);
    ui->softComboBox->addItem("<Добавить...>", -1);
}

void SoftDialog::on_softComboBox_currentIndexChanged(int index)
{
    if((ui->softComboBox->currentData().toInt() == -1)){
        if(!ui->vendorComboBox->currentText().isEmpty() || ui->softComboBox->count() == 1){
            QString softTitle = QInputDialog::getText(this, "Новое ПО", "Введите наименование нового ПО");
            ui->logoLabel->clear();
            ui->logoLabel->setText("Отсутствует");
            if(!softTitle.isEmpty()){
                softTitle.remove(";");
                bool disconnect = false;
                bool connected = true;
                if(!db.isOpen()){
                    disconnect = true;
                    connected = parent->connectToDatabase(&db, false, this);
                }
                if(connected){
                    db.transaction();
                    if(!parent->executeInsertCommand("software", QString("Title;Vendor").split(";"), QString(softTitle + ";" + ui->vendorComboBox->currentData().toString()).split(";"))){
                        QMessageBox::critical(this, "Ошибка", "Ошибка записи в базу данных");
                        db.rollback();
                        return;
                    }
                    QVector<QStringList> retrievedData = parent->executeSelectCommand("software", parent->getColumnsNames("software"), "Title='" + softTitle + "'");
                    if(!retrievedData.isEmpty()){
                        ui->softComboBox->setItemData(index, retrievedData[parent->getColumnsNames("software").indexOf("ID")][0]);
                        ui->softComboBox->setItemText(index, softTitle);
                        ui->softComboBox->addItem("<Добавить...>", -1);
                        db.commit();
                        ui->descriptionTextEdit->clear();
                        ui->logoLabel->clear();
                        ui->logoLabel->setText("Отсутствует");
                    }
                    else{
                        db.rollback();
                        QMessageBox::critical(this, "Ошибка", "Ошибка работы с базой данных");
                        return;
                    }
                }
                if(disconnect)
                    parent->disconnectFromDatabase(&db);
            }
        }
        return;
    }
    bool disconnect = false;
    bool connected = true;
    if(!db.isOpen()){
        disconnect = true;
        connected = parent->connectToDatabase(&db, false, this);
    }
    if(connected){
        QStringList column = QString("Description").split(";");
        QVector<QStringList> retrievedData = parent->executeSelectCommand("software", column, "ID=" + ui->softComboBox->currentData().toString());
        if(!retrievedData.isEmpty()){
            ui->descriptionTextEdit->setPlainText(retrievedData[0][0]);
        }
    }
    if(disconnect)
        parent->disconnectFromDatabase(&db);
    getLogo();
}

void SoftDialog::on_changeLogoBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Открыть файл изображения"), "", tr("Все файлы (*.*)"));
    if(!fileName.isEmpty()){
        newLogoFilePath.clear();
        QPixmap logo(fileName);
        if(logo.isNull()){
            QMessageBox::critical(this, "Ошибка", "Данный файл не является изображением или не поддерживается", QMessageBox::Ok);
        }
        else{
            newLogoFilePath = fileName;
            getLogo(newLogoFilePath);
        }
    }
}

void SoftDialog::getLogo(){
    ui->logoLabel->clear();
    QDirIterator it(parent->logoFolder, QStringList() << "*.*", QDir::Files, QDirIterator::NoIteratorFlags);
    while (it.hasNext()){
        QString file = it.next();
        if(file.contains(ui->vendorComboBox->currentText() + " " + ui->softComboBox->currentText(), Qt::CaseInsensitive)){
            QPixmap logo(file);
            if(logo.isNull())
                break;
            if(logo.width() >= logo.height())
                logo = logo.scaledToWidth(ui->logoLabel->width());
            else
                logo = logo.scaledToHeight(ui->logoLabel->height());
            ui->logoLabel->setPixmap(logo);
            break;
        }
    }
    if(ui->logoLabel->pixmap() == 0)
        ui->logoLabel->setText("Отсутствует");
}

void SoftDialog::setLogo(){
    QFile file;
    file.setFileName(newLogoFilePath);
    QDirIterator it(parent->logoFolder, QStringList() << "*.*", QDir::Files, QDirIterator::NoIteratorFlags);
    while (it.hasNext()){
        QString oldFile = it.next();
        if(oldFile.contains(ui->vendorComboBox->currentText() + " " + ui->softComboBox->currentText(), Qt::CaseInsensitive)){
            QFile::remove(oldFile);
            break;
        }
    }
    QFileInfo fileInfo(file);
    file.copy(parent->logoFolder + ui->vendorComboBox->currentText() + " " + ui->softComboBox->currentText() + "." + fileInfo.suffix());
}

void SoftDialog::getLogo(QString filename){
    ui->logoLabel->clear();
    QPixmap logo(filename);
    if(logo.isNull())
        return;
    if(logo.width() >= logo.height())
        logo = logo.scaledToWidth(ui->logoLabel->width());
    else
        logo = logo.scaledToHeight(ui->logoLabel->height());
    ui->logoLabel->setPixmap(logo);
}

void SoftDialog::on_buttonBox_accepted()
{
    bool connected = parent->connectToDatabase(&db, true, this);
    if(connected){
        QStringList description;
        description.append(ui->descriptionTextEdit->toPlainText());
        parent->executeUpdateCommand("software", QString("Description").split(";"), description, "ID=" + ui->softComboBox->currentData().toString());
        if(!newLogoFilePath.isEmpty()){
            setLogo();
        }
    }
    parent->disconnectFromDatabase(&db);
}
