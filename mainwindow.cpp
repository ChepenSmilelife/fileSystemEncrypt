#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "md5.h"
//#include <cstdio>
#include "qencrypt.h"

#include <QDebug>

#include <QDir>
#include <QIcon>
#include <QTime>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include "pwddialog.h"
#include "logindialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dataInit();

    createStatubar();

    buildButtonGroup();

    buildFileTableWidget(ui->tableWidgetFile);

    buildSignalAndSlots();

    updateFileList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::quitAll()
{
    qApp->quit();
}

void MainWindow::dataInit()
{
    curDir = QDir(".").canonicalPath();
    hasLogin = false;
}

void MainWindow::createStatubar()
{
    statusBar()->showMessage(curDir);
}

void MainWindow::buildSignalAndSlots()
{
    connect(ui->actionQuit, SIGNAL(triggered()),
            this, SLOT(quitAll()));
    connect(ui->tableWidgetFile, SIGNAL(cellDoubleClicked(int,int)),
            this, SLOT(fileListDoubleClick(int)));
    connect(ui->pushButtonEncrypt, SIGNAL(clicked()),
            this, SLOT(toEncrypt()));
    connect(ui->pushButtonDecrypt, SIGNAL(clicked()),
            this, SLOT(toDecrypt()));

    connect(ui->actionEncrypt, SIGNAL(triggered()),
            this, SLOT(toEncrypt()));
    connect(ui->actionDecrypt, SIGNAL(triggered()),
            this, SLOT(toDecrypt()));
    connect(ui->actionCheck, SIGNAL(triggered()),
            this, SLOT(toCheck()));

    connect(this, SIGNAL(messageChange(QString)),
            this, SLOT(addLog(QString)));

    connect(ui->actionLogin, SIGNAL(triggered()),
            this, SLOT(toLoginOrLogout()));
}

void MainWindow::buildFileTableWidget(QTableWidget *table)
{
    table->setColumnCount(3);
    table->verticalHeader()->hide();
    table->setSortingEnabled(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setShowGrid(false);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QStringList horizontalHeader;
    horizontalHeader<<tr("Name")<<tr("Date")<<tr("Size");
    table->setHorizontalHeaderLabels(horizontalHeader);
}

void MainWindow::updateFileList()
{
    QDir dir(curDir);
    if(!dir.exists())
        return;
    int count = ui->tableWidgetFile->rowCount();
    for(int i = 0; i < count; i++)
        ui->tableWidgetFile->removeRow(0);
    // get directory list
    dir.setFilter(QDir::AllEntries | QDir::NoDot);
    dir.setSorting(QDir::DirsFirst );
    QFileInfoList fileList = dir.entryInfoList();
    for(int i = 0; i < fileList.count(); ++i)
    {
        ui->tableWidgetFile->insertRow(i);
        ui->tableWidgetFile->setItem(i, 0,
                                     new QTableWidgetItem( QIcon(fileList[i].isDir()?"://resouces/folder.png":"://resouces/file.png"),fileList[i].fileName()));
        ui->tableWidgetFile->setItem(i, 1,
                                     new QTableWidgetItem(fileList[i].created().toString()));
        ui->tableWidgetFile->setItem(i, 2,
                                     new QTableWidgetItem(QString::number(fileList[i].size()) + " " + tr("bytes")));
    }
    ui->tableWidgetFile->scrollToTop();
    emit messageChange(QDateTime::currentDateTime().toString() + '\t'
                       + tr("update file list of directory: ")
                       + curDir );
}

void MainWindow::fileListDoubleClick(int r)
{
    QDir dir(curDir);
    QString name = ui->tableWidgetFile->item(r, 0)->text();
    if( QFileInfo( dir.filePath(name) ).isDir() )
    {
        dir.cd(name);
        curDir = dir.canonicalPath();
        updateFileList();
        statusBar()->showMessage(curDir);
    }else {
        showFileInfo( dir.filePath(name) );
    }
}

void MainWindow::showFileInfo(QString p)
{
    QFileInfo finfo(p);
    QStringList infoList;
    infoList << tr("name: %1").arg(finfo.fileName())
             << tr("path: %1").arg(finfo.canonicalPath())
             << tr("suffix: .%1").arg(finfo.completeSuffix())
             << tr("size: %1 bytes").arg(QString::number( finfo.size() ))
             << tr("created: %1").arg(finfo.created().toString())
             << tr("last read: %1").arg(finfo.lastRead().toString())
             << tr("last modified: %1").arg(finfo.lastModified().toString());
    QMessageBox::information(this, tr("File information"), infoList.join('\n'));
}

void MainWindow::selectedRows(QList<int> &rows, QTableWidget *w)
{
    rows.clear();
    QSet<int> srows;
    QList<QTableWidgetItem*> indexList = w->selectedItems();
    int count = indexList.count();
    for(int i = 0; i < count; ++i)
        srows.insert(indexList[i]->row());
    rows = srows.toList();
}

void MainWindow::toEncrypt()
{
    if(ui->checkBoxSyncServer->isChecked() && !hasLogin)
        return;

    // get password
    QString tmp_pwd;
    pwdDialog dialog;
    if( dialog.exec() == QDialog::Rejected )
        return;
    else{
        tmp_pwd = dialog.pwd();
        qDebug()<<tmp_pwd;
    }

    QList<int> rows;
    selectedRows(rows, ui->tableWidgetFile);
    QStringList list, skipList;
    int count = rows.count();
    for( int i = 0; i < count; ++i)
    {
        QString path = QDir(curDir).filePath(ui->tableWidgetFile->item(rows[i], 0)->text());
        if( QFileInfo(path).isDir() ) {
            skipList << "Dir: " + QFileInfo(path).fileName();
            continue;
        }
        qDebug()<<"encrypt start... "<<path;
        encryptFile(tmp_pwd, path);

        // sync key to remote server
        if(ui->checkBoxSyncServer->isChecked()) {
            QString errorMsg = QDateTime::currentDateTime().toString()
                    + "\t[";
            if(!sendKeyToServer(tmp_pwd, path))
                errorMsg += tr("fail");
            else
                errorMsg += tr("success");
            errorMsg += "]\t" + tr("sync key to server")
                    + "[" + server + "]:" + QFileInfo(path).fileName();
            emit messageChange(errorMsg);
        }

        qDebug()<<"encrypt... end";
        updateFileList();
        list.append(QFileInfo(path).fileName());
    }
    QMessageBox::information(this, tr("Encrypt info"),
                             tr("Encrypted files: ")
                             + "\n\t" + list.join("\n\t")
                             + "\n" + tr("Skip files: ") + "\n\t"
                             + skipList.join("\n\t"));
}

void MainWindow::toDecrypt()
{
    if(ui->checkBoxSyncServer->isChecked() && !hasLogin)
        return;

    // get password
    QString tmp_pwd;
    pwdDialog dialog;
    if( dialog.exec() == QDialog::Rejected )
        return;
    else{
        tmp_pwd = dialog.pwd();
        qDebug()<<tmp_pwd;
    }

    QList<int> rows;
    QStringList skipList, dealedList;
    selectedRows(rows, ui->tableWidgetFile);
    int count = rows.count();
    for( int i = 0; i < count; ++i)
    {
        QString path = QDir(curDir).filePath(ui->tableWidgetFile->item(rows[i], 0)->text());

        if(QFileInfo(path).isDir()) {
            skipList << QFileInfo(path).fileName();
        }

        // sync key from remote server
        if(ui->checkBoxSyncServer->isChecked()) {
            QString errorMsg = QDateTime::currentDateTime().toString() + "\t[";
            if(!getKeyFromServer(tmp_pwd, path))
                errorMsg += tr("fail");
            else {
                errorMsg += tr("success");
                decryptFile(tmp_pwd, path);
            }
            errorMsg += "]\t" + tr("sync key from server")
                    + "[" + server + "]:" + QFileInfo(path).fileName();
            emit messageChange(errorMsg);
        }
        else {
            decryptFile(tmp_pwd, path);
        }

        dealedList << QFileInfo(path).fileName();
        updateFileList();
    }
    QString msg = tr("Decrypted files: ") + "\n\t"
            + dealedList.join("\n\t") + "\n"
            + tr("Skiped files: ") + "\n\t"
            + skipList.join("\n\t");

    QMessageBox::information( this, tr("decrypted info"), msg );
}

void MainWindow::toCheck()
{/*
    QList<int> rows;
    selectedRows(rows, ui->tableWidgetFile);
    QStringList list, skipList;
    int count = rows.count();
    for( int i = 0; i < count; ++i)
    {
        QString path = QDir(curDir).filePath(ui->tableWidgetFile->item(rows[i], 0)->text());
        if(!__checkMagicHead(path.toStdString().c_str()))
            list << QFileInfo(path).fileName();
        else
            skipList << QFileInfo(path).fileName();
    }
    QString msg = tr("Encrypted: ") + "\n\t"
            + list.join("\n\t") + "\n"
            + tr("Not encrypted: ") + "\n\t"
            + skipList.join("\n\t") + "\n";
    QMessageBox::information(this, tr("Check Information"), msg);*/
}

void MainWindow::addLog(QString log)
{
    ui->textEditLog->append(log);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    QString logName = QDateTime::currentDateTime().toString("fe-yyyy-MM-dd-HH-mm-ss.log");
    qDebug()<<logName;
    QFile logFile( logName );
    if( logFile.open(QIODevice::WriteOnly) ) {
        QTextStream out(&logFile);
        out << ui->textEditLog->toPlainText();
    }
    e->accept();
}

void MainWindow::buildButtonGroup()
{
    ui->buttonGroupOld->setId(ui->radioButtonOverlap,   0);
    ui->buttonGroupOld->setId(ui->radioButtonDelete,    1);
    ui->buttonGroupOld->setId(ui->radioButtonKeep,      2);

    /* hide some button */
    ui->radioButtonDelete->hide();
    ui->radioButtonKeep->hide();
}

bool MainWindow::sendKeyToServer(QString pwd, QString file)
{
    return true;
}

bool MainWindow::getKeyFromServer(QString &pwd, QString file)
{
    return true;
}

void MainWindow::toLoginOrLogout()
{
    if(hasLogin) {
        ui->actionLogin->setText(tr("Logout"));
        hasLogin = false;
    }else {
        loginDialog lgDialog;
        lgDialog.setWindowTitle(tr("Login to server"));
        QString account, pwd;
        if(lgDialog.exec() == QDialog::Accepted) {
            QStringList infoList = lgDialog.loginInfo();
            account =   infoList[0];
            pwd     =   infoList[1];
        }else
            return;
        qDebug()<<"account: "<< account<<", pwd: "<<pwd;
        ui->actionLogin->setText(tr("Login"));
        hasLogin = true;
    }
}
