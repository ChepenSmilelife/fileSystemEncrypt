#ifndef PWDDIALOG_H
#define PWDDIALOG_H

#include <QDialog>

namespace Ui {
class pwdDialog;
}

class pwdDialog : public QDialog
{
    Q_OBJECT
public:
    explicit pwdDialog(QWidget *parent = 0);

signals:

public slots:
    void setContent(QString str);
    QString pwd();

protected slots:
    void confirmWithPWD();

private:
    Ui::pwdDialog *ui;
};

#endif // PWDDIALOG_H
