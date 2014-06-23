#include "logindialog.h"
#include "ui_logindialog.h"

#include <QDebug>

loginDialog::loginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(rejected()),
            this, SLOT(reject()));
    connect(ui->buttonBox, SIGNAL(accepted()),
            this, SLOT(confirmLogin()));
}

loginDialog::~loginDialog()
{
    delete ui;
}

QStringList loginDialog::loginInfo() const
{
    QStringList info;
    info << ui->lineEditAccount->text() << ui->lineEditPassword->text();
    return info;
}

void loginDialog::confirmLogin()
{
    if(!ui->lineEditAccount->text().trimmed().isEmpty()
            && ui->lineEditPassword->text().size() >= 6)
        this->accept();
    else {
        return;
    }
}
