#ifndef HEADER_QENCRYPT_H
#define HEADER_QENCRYPT_H

#include <QFile>
#include <QFileInfo>

#include <QDebug>

#include "xorencrypt.h"
#include "md5.h"

#define OverlapOld  0
#define DeleteOld   1
#define KeepOld     2


inline bool fileWithMode(QString pwd, QString oldPath, QString newPath = "",
                        int fileMode = KeepOld)
{
    if(fileMode == OverlapOld)
        newPath = oldPath + ".tmp~";
    else
        if( newPath.isEmpty() )
            return false;
    //qDebug()<<"file with bytes";

    unsigned char arr_pwd[MD5_DIGEST_SIZE];
    __md5_buffer(pwd.toStdString().c_str(),
                 strlen(pwd.toStdString().c_str()),
                 arr_pwd);

    if(!__xorbytesEncrypt(arr_pwd,
                          MD5_DIGEST_SIZE,
                          oldPath.toStdString().c_str(),
                          newPath.toStdString().c_str()))
        return false;

    //QString oldname = QFileInfo(oldPath).fileName();
    if(fileMode == OverlapOld || fileMode == DeleteOld)
        QFile(oldPath).remove();
    if(fileMode == OverlapOld)
        QFile(newPath).rename(oldPath);

    return true;
}

inline bool encryptFile(QString pwd, QString oldPath, QString newPath = "",
                        int fileMode = OverlapOld)
{
    return fileWithMode(pwd, oldPath, newPath, fileMode);
}

inline bool decryptFile(QString pwd, QString oldPath, QString newPath = "",
                        int fileMode = OverlapOld)
{
    return fileWithMode(pwd, oldPath, newPath, fileMode);
}

#endif
