#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "encrypt.h"
#include "md5.h"
//#include <cstdio>

#include <QDebug>

#include <QDir>
#include <QIcon>
#include <QTime>
#include <QMessageBox>
#include "pwddialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dataInit();

    createStatubar();

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
    sprintf((char *) pwd, "helloworld");
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
    connect(ui->pushButtonCheck, SIGNAL(clicked()),
            this, SLOT(toCheck()));

    connect(ui->actionEncrypt, SIGNAL(triggered()),
            this, SLOT(toEncrypt()));
    connect(ui->actionDecrypt, SIGNAL(triggered()),
            this, SLOT(toDecrypt()));
    connect(ui->actionCheck, SIGNAL(triggered()),
            this, SLOT(toCheck()));
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
    }else{
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
    QString tmp_pwd;
    pwdDialog dialog;
    if( dialog.exec() == QDialog::Rejected )
        return;
    else{
        tmp_pwd = dialog.pwd();
        qDebug()<<tmp_pwd;
    }

    unsigned char arr_pwd[16];
    md5_buffer( tmp_pwd.toStdString().c_str(), tmp_pwd.size(), arr_pwd );

    QList<int> rows;
    selectedRows(rows, ui->tableWidgetFile);
    QStringList list;
    int count = rows.count();
    for( int i = 0; i < count; ++i)
    {
        QDir dir(curDir);
        __encrypt( dir.filePath(ui->tableWidgetFile->item(rows[i], 0)->text()).toStdString().c_str(), arr_pwd, 16 );
        updateRow(rows[i]);
        list.append(ui->tableWidgetFile->item(rows[i], 0)->text());
    }
    QMessageBox::information(this, tr("Encrypt info"),
                             tr("Encrypted file: ")  + '\n' + list.join("\n\t"));
}

void MainWindow::toDecrypt()
{
    QString tmp_pwd;
    pwdDialog dialog;
    if( dialog.exec() == QDialog::Rejected )
        return;
    else{
        tmp_pwd = dialog.pwd();
        qDebug()<<tmp_pwd;
    }

    unsigned char arr_pwd[16];
    md5_buffer( tmp_pwd.toStdString().c_str(), tmp_pwd.size(), arr_pwd );

    QList<int> rows;
    QList<QString> skipList, dealedList;
    selectedRows(rows, ui->tableWidgetFile);
    int count = rows.count();
    for( int i = 0; i < count; ++i)
    {
        QDir dir(curDir);
        QString path = dir.filePath(ui->tableWidgetFile->item(rows[i], 0)->text());
        if( !__checkMagicHead(path.toStdString().c_str()) ) {
            skipList.append( QFileInfo(path).fileName() );
            continue;
        }
        __decrypt( path.toStdString().c_str(), arr_pwd, 16 );
        dealedList.append( QFileInfo(path).fileName() );
        updateRow(rows[i]);
    }
    QString msg = tr("Decrypted files: ") + '\n';
    for( int i = 0; i < dealedList.count(); ++i )
        msg += "\t" + dealedList[i] + '\n';
    msg += tr("Skiped files: ") + '\n';
    for( int i = 0; i < skipList.count(); ++i )
        msg += "\t" + skipList[i] + "    " + tr("had not been encrypted") +'\n';
    QMessageBox::information( this, tr("decrypted info"), msg );
}

void MainWindow::updateRow(int r)
{
    QDir dir(curDir);
    QString path = dir.filePath(ui->tableWidgetFile->item(r, 0)->text());
    QFileInfo info(path);
    ui->tableWidgetFile->item(r, 1)->setText(info.created().toString());
    ui->tableWidgetFile->item(r, 2)->setText(QString::number( info.size() ) + " " + tr("bytes"));
}

void MainWindow::toCheck()
{
    QList<int> rows;
    selectedRows(rows, ui->tableWidgetFile);
    QStringList list, skipList;
    int count = rows.count();
    for( int i = 0; i < count; ++i)
    {
        QString path = QDir(curDir).filePath(ui->tableWidgetFile->item(rows[i], 0)->text());
        if(__checkMagicHead(path.toStdString().c_str()))
            list << QFileInfo(path).fileName();
        else
            skipList << QFileInfo(path).fileName();
    }
    QString msg = tr("Encrypted: ") + "\n\t"
            + list.join("\n\t") + "\n"
            + tr("Not encrypted: ") + "\n\t"
            + skipList.join("\n\t") + "\n";
    QMessageBox::information(this, tr("Check Information"), msg);
}
