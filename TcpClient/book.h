#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "protocol.h"
#include<QTimer>

class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void updateFileList(const PDU *pdu);
    void clearEnterDir();
    QString enterDir();
    void setDownloadStatus(bool status);
    bool getDownloadStatus();
    QString getSaveFilePath();

    QString getShareFileName();


    qint64 m_iTotal;    //总的文件大小
    qint64 m_iRecved;   //已收到多少



signals:

public slots:
    void createDir();
    void flushFile();
    void delDir();
    void renameFile();
    void enterDir(const QModelIndex &index);
    void delRegFile();
    void uploadFile();
    void uploadFileData();
    void downloadFile();
    void shareFile();
    void moveFile();
    void selectDestDir();





private:
    QListWidget *m_pBookListW;
    QPushButton *m_pReturnPB;
    QPushButton *m_pCreateDirPB;
    QPushButton *m_pDelDirPB;
    QPushButton *m_pRenamePB;
    QPushButton *m_pFlushPB;
    QPushButton *m_pUploadPB;
    QPushButton *m_pDownLoadPB;
    QPushButton *m_pDelFilePB;
    QPushButton *m_pShareFielPB;
    QPushButton *m_pMoveFilePB;
    QPushButton *m_pSelectDirPB;//选择要移动的文件夹


    QString m_strEnterDir;//进入的dir 的名字
    QString m_strUploadFilePath;//上传文件路径
    QTimer *m_pTimer;

    QString m_strSaveFilePath;//存储的文件路径
    bool m_bDownload;//处于下载文件状态

    QString m_strShareFileName;//要分享的文件名字
    QString m_strMoveFileName;//想要移动的文件名
    QString m_strMoveFilePath;//想要移动的文件地址

    QString m_strDestDir;//用于保存目的dir的名字



};

#endif // BOOK_H
