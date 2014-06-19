#include "pwddialog.h"
#include "ui_pwddialog.h"

pwdDialog::pwdDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::pwdDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()),
            this, SLOT(confirmWithPWD()));
    connect(ui->buttonBox, SIGNAL(rejected()),
            this, SLOT(reject()));
}

void pwdDialog::setContent(QString str)
{
    ui->label->setText(str);
}

void pwdDialog::confirmWithPWD()
{
    if( ui->lineEdit->text().size() < 6 )
        return;
    else
        this->accept();
}

QString pwdDialog::pwd()
{
    return ui->lineEdit->text();
}
