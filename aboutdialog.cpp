#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(LicenseControl *parent) :
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags = flags & (~helpFlag);
    setWindowFlags(flags);
    QObject::connect(ui->DialogBtnBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(accept()));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
