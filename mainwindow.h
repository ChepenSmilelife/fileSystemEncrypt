#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QCloseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void quitAll();
    void toLoginOrLogout();
    void updateFileList();
    void fileListDoubleClick(int r);
    void showFileInfo(QString p);
    void toEncrypt();
    void toDecrypt();
    void toCheck();
    void addLog(QString log);
    bool getKeyFromServer(QString &pwd, QString file);
    bool sendKeyToServer(QString pwd, QString file);

signals:
    void messageChange(QString log);

protected:
    void selectedRows(QList<int> &rows, QTableWidget *w);
    void closeEvent(QCloseEvent *e);

private:
    Ui::MainWindow *ui;
    void dataInit();
    void createStatubar();
    void buildButtonGroup();
    void buildSignalAndSlots();
    void buildFileTableWidget(QTableWidget *);
    QString curDir;
    bool hasLogin;
    QString server;
};

#endif // MAINWINDOW_H
