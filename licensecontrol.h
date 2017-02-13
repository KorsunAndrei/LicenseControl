#ifndef LICENSECONTROL_H
#define LICENSECONTROL_H

#include <QObject>

#include <QSystemTrayIcon>
#include <QMenu>
#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QtSql>
#include <QSettings>
#include <QFileDialog>
#include <QAxObject>
#include <QSound>
#include <QLabel>
#include <QtGlobal>
#include <QPrinter>
#include <QPrintDialog>
#include <xlsxdocument.h>
#include <catalogdialog.h>
#include <configdialog.h>
#include <canceldialog.h>
#include <addlicensedialog.h>
#include <selldialog.h>
#include <aboutdialog.h>
#include <changelicensecountdialog.h>
#include <changetextdialog.h>
#include <reportdialog.h>
#include <notificationdialog.h>
#include <softdialog.h>
#include <clientsdialog.h>
#include <trackingdialog.h>
#include <reportlicensesdialog.h>

//Forward declarations
class ConfigDialog;
class CatalogDialog;
class CancelDialog;
class AddLicenseDialog;
class SellDialog;
class AboutDialog;
class ChangeLicenseCountDialog;
class ChangeTextDialog;
class ReportDialog;
class NotificationDialog;
class SoftDialog;
class ClientsDialog;
class TrackingDialog;
class reportLicensesDialog;

class LicenseControl : public QObject
{
    Q_OBJECT
public:
    explicit LicenseControl(QApplication * app);
    ~LicenseControl();
    bool connectToDatabase(QSqlDatabase * db, bool toClose, QDialog * dialog);
    void disconnectFromDatabase(QSqlDatabase * db);
    void showTrayIcon();
    int commitChangesDialog();
    QStringList getColumnsNames(QString table);
    bool executeInsertCommand(QString table, QStringList columnNames, QStringList values);
    bool executeUpdateCommand(QString table, QStringList columnNames, QStringList values, QString condition);
    QVector<QStringList> executeSelectCommand(QString table, QStringList columnNames);
    QVector<QStringList> executeSelectCommand(QString table, QStringList columnNames, QString condition);
    QVector<QStringList> executeSelectCommand(QString table1, QString table2, QStringList columnNames1, QStringList columnNames2, QString joinField, QString condition);\
    QVector<QStringList> executeSelectCommand(QString table, QStringList columnNames, QString aggregationFunction, QString condition, QString groupByColumn);
    bool executeDeleteCommand(QString table, QString condition);
    QString getIDof(QString table, QString column, QString searchString);
    QString getDescription(QString soft);
    QString getDescription(int soft);
    void printDocument(QString pathToDocument);
    QString getApplicationPath();
    bool checkStartup();
    QTimer * customerTimer;
    QTimer * licenseTimer;
    //Settings
    //Database settings
    QString hostname;
    QString username;
    QString password;
    //Notification settings
    struct NotificationSettings{
        int interval;
        bool sound;
        QString text;
    };
    NotificationSettings customerNotificationSettings;
    NotificationSettings licenseNotificationSettings;
    QString soundPath;
    int daysCount;
    struct FieldsToPrint{
        QString cell;
        QString field;
    };
    struct MinimumLicenseCount{
        int soft;
        int count;
    };
    QVector<FieldsToPrint> printFields;
    QVector<MinimumLicenseCount> minLicenseCount;
    int defaultMinLicenseCount;
    struct licenseNotifications{
        QString soft;
        int count;
    };
    struct customerNotifications{
        int customer;
        QString soft;
        QString date;
        int license;
    };
    QVector<licenseNotifications> lNotifications;
    QVector<customerNotifications> cNotifications;
    QString logoFolder;
    QVector<int> licenseDoNotTrack;

public slots:
    void checkCustomer(bool show);
    void checkLicense(bool show);
    void checkCustomer();
    void checkLicense();
private:
    //QAction
    QAction * exitAction;
    QAction * addLicenseAction;
    QAction * catalogAction;
    QAction * sellAction;
    QAction * cancelAction;
    QAction * configAction;
    QAction * aboutAction;
    QAction * reportAction;
    QAction * notificationAction;
    QAction * softAction;
    QAction * customersAction;
    QAction * licensesReportAction;
    //General
    QApplication * application;
    QSystemTrayIcon * trayIcon;
    QMenu * trayIconMenu;
    void setTrayIconActions();
    void setPrintFields();

private slots:
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void addLicense();
    void sellLicense();
    void cancelLicense();
    void configuration();
    void cancelOrder();
    void showCatalog();
    void showAbout();
    void showSoftEdit();
    void showClients();
    void showNotification(QString title, QString text, QSystemTrayIcon::MessageIcon icon, int duration);
    void getSettings();
    void setSettings();
    void generateReport();
    void addLicenseNotification(QString soft, int count);
    void addCustomerNotification(int customer, QString soft, QString date, int license);
    void notifications();
    void reportLicenses();
};

#endif // LICENSECONTROL_H
