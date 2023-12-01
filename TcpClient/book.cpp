#include "book.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>
#include "tcpclient.h"
#include <QFileDialog>
#include "opewidget.h"
#include "sharefile.h"


Book::Book(QWidget *parent) : QWidget(parent)
{
    m_strEnterDir.clear();

    m_pTimer = new QTimer;
    m_bDownload = false;

    m_pBookListW = new QListWidget;
    m_pReturnPB = new QPushButton("Return");
    m_pCreateDirPB = new QPushButton("CreateDir");
    m_pDelDirPB = new QPushButton("DelDir");
    m_pRenamePB = new QPushButton("Rename");
    m_pFlushPB = new QPushButton("Flush");
    m_pMoveFilePB = new QPushButton("移动文件");
    m_pSelectDirPB = new QPushButton("目标目录");
    m_pSelectDirPB->setEnabled(false);//默认无法使用

    QVBoxLayout *pDirVBL = new QVBoxLayout;
    pDirVBL->addWidget(m_pReturnPB);
    pDirVBL->addWidget(m_pCreateDirPB);
    pDirVBL->addWidget(m_pDelDirPB);
    pDirVBL->addWidget(m_pRenamePB);
    pDirVBL->addWidget(m_pFlushPB);
    pDirVBL->addWidget(m_pMoveFilePB);

    m_pUploadPB = new QPushButton("Upload");
    m_pDownLoadPB = new QPushButton("DownLoad");
    m_pDelFilePB = new QPushButton("DelFile");
    m_pShareFielPB = new QPushButton("Share");

    QVBoxLayout *pFileVBL = new QVBoxLayout;
    pFileVBL->addWidget(m_pUploadPB);
    pFileVBL->addWidget(m_pDownLoadPB);
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pShareFielPB);
    pFileVBL->addWidget(m_pSelectDirPB);

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);

    //各个信号与信号槽的关联
    connect(m_pCreateDirPB,SIGNAL(clicked(bool))
            ,this,SLOT(createDir()));
    connect(m_pFlushPB,SIGNAL(clicked(bool))
            ,this,SLOT(flushFile()));
    connect(m_pDelDirPB,SIGNAL(clicked(bool))
            ,this,SLOT(delDir()));
    connect(m_pRenamePB,SIGNAL(clicked(bool))
            ,this,SLOT(renameFile()));

    connect(m_pBookListW,SIGNAL(doubleClicked(QModelIndex))
            ,this,SLOT(enterDir(QModelIndex)));

    connect(m_pDelFilePB,SIGNAL(clicked(bool))
            ,this,SLOT(delRegFile()));

    connect(m_pUploadPB,SIGNAL(clicked(bool))
            ,this,SLOT(uploadFile()));

    connect(m_pTimer,SIGNAL(timeout())
            ,this,SLOT(uploadFileData()));

    connect(m_pDownLoadPB,SIGNAL(clicked(bool))
            ,this,SLOT(downloadFile()));
    connect(m_pShareFielPB,SIGNAL(clicked(bool))
            ,this,SLOT(shareFile()));

    connect(m_pMoveFilePB, SIGNAL(clicked(bool))
            , this, SLOT(moveFile()));

    connect(m_pSelectDirPB, SIGNAL(clicked(bool))
            , this, SLOT(selectDestDir()));
}

void Book::updateFileList(const PDU *pdu)
{
    if(pdu == NULL){
        return;
    }
    FileInfo *pFileInfo = NULL;//通过该指针进行遍历
    int iCount = pdu->uiMsgLen/sizeof(FileInfo);
    m_pBookListW->clear();

    for (int i = 0;i<iCount;i++) {
        pFileInfo = (FileInfo*)(pdu->caMsg)+i;
        qDebug()<<pFileInfo->caFileName<<pFileInfo->iFileType;
        QListWidgetItem *pItem = new QListWidgetItem;
        if(pFileInfo->iFileType == 0){
            pItem->setIcon(QIcon(QPixmap(":/icon/dir.jpg")));
        }else if (pFileInfo->iFileType == 1) {
            pItem->setIcon(QIcon(QPixmap(":/icon/file.jpg")));
        }
        pItem->setText(pFileInfo->caFileName);

        m_pBookListW->addItem(pItem);
    }
}

void Book::createDir()
{//需要三个信息
    QString strNewDir = QInputDialog::getText(this,"Create new dirictory","Please input the name:");
    if(!strNewDir.isEmpty())
    {
        if(strNewDir.size()>32){
            QMessageBox::warning(this,"NAME ERROR","your dir's name is over than 32db.");
        }else {
            QString strName = TcpClient::getInstance().loginName();
            QString strCurPath = TcpClient::getInstance().curPath();
            PDU *pdu = mkPDU(strCurPath.size()+1);//尾椎+/0
            pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
            strncpy(pdu->caData,strName.toStdString().c_str(),strName.size());
            strncpy(pdu->caData+32,strNewDir.toStdString().c_str(),strNewDir.size());
            memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());

            TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu = NULL;
        }
    }
    else
    {
        QMessageBox::warning(this,"NAME ERROR","Please rename your dir.");
    }

}

void Book::flushFile()
{
    QString strCurPath = TcpClient::getInstance().curPath();

    PDU *pdu = mkPDU(strCurPath.size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    strncpy((char*)pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());

    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::delDir()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(pItem == NULL)
    {
        QMessageBox::warning(this,"del file","Please select the dir you want to del");
    }
    else
    {
        QString strDelName = pItem->text();
        PDU *pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_REQUEST;
        strncpy(pdu->caData,strDelName.toStdString().c_str(),strDelName.size());
        memcpy((char*)pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());

        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }


}

void Book::renameFile()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(pItem == NULL)
    {
        QMessageBox::warning(this,"rename file","Please select the file you want to rename");
    }
    else
    {
        QString strOldName = pItem->text();
        QString strNewName = QInputDialog::getText(this,"New name","please input the new name:");
        if(!strNewName.isEmpty())
        {
            PDU *pdu = mkPDU(strCurPath.size()+1);
            pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
            strncpy(pdu->caData,strOldName.toStdString().c_str(),strOldName.size());
            strncpy(pdu->caData+32,strNewName.toStdString().c_str(),strNewName.size());
            memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());

            TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu = NULL;
        }
        else
        {
            QMessageBox::warning(this,"rename file","Please select the file you want to rename");
        }


    }

}

void Book::enterDir(const QModelIndex &index)
{
    QString strDirName = index.data().toString();
//    QString strDirName = item->text();
    m_strEnterDir = strDirName;

    QString strCurPath = TcpClient::getInstance().curPath();
    qDebug()<<"enterDir:";
    qDebug()<<strCurPath;
    PDU *pdu = mkPDU(strCurPath.size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_REQUEST;

    strncpy(pdu->caData,strDirName.toStdString().c_str(),strDirName.size());
    memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());

    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::delRegFile()
{


    QString strCurPath = TcpClient::getInstance().curPath();
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(pItem == NULL)
    {
        QMessageBox::warning(this,"del file","Please select the file you want to del");
    }
    else
    {
        QString strDelName = pItem->text();
        PDU *pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_REQUEST;
        strncpy(pdu->caData,strDelName.toStdString().c_str(),strDelName.size());
        memcpy((char*)pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }


}

void Book::uploadFile()
{


    m_strUploadFilePath = QFileDialog::getOpenFileName();
    qDebug() << m_strUploadFilePath;
    if (!m_strUploadFilePath.isEmpty())
    {
        //  aa/bb/cc  5  8
        int index = m_strUploadFilePath.lastIndexOf('/');
        QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.size()-index-1);
        qDebug() << strFileName;

        QFile file(m_strUploadFilePath);
        qint64 fileSize = file.size();  //获得文件大小

        QString strCurPath = TcpClient::getInstance().curPath();
        PDU *pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
        memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
        sprintf(pdu->caData, "%s %lld", strFileName.toStdString().c_str(), fileSize);

        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;

        m_pTimer->start(1000);
    }
    else
    {
        QMessageBox::warning(this, "上传文件", "上传文件名字不能为空");
    }
}

void Book::uploadFileData()
{
    m_pTimer->stop();
    QFile file(m_strUploadFilePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "上传文件", "打开文件失败");
        return;
    }

    char *pBuffer = new char[4096];
    qint64 ret = 0;
    while (true)
    {
        ret = file.read(pBuffer, 4096);
        if (ret > 0 && ret <= 4096)
        {
            TcpClient::getInstance().getTcpSocket().write(pBuffer, ret);
        }
        else if (0 == ret)
        {
            break;
        }
        else
        {
            QMessageBox::warning(this, "上传文件", "上传文件失败:读文件失败");
            break;
        }
    }
    file.close();
    delete []pBuffer;
    pBuffer = NULL;
}

void Book::clearEnterDir()
{
    m_strEnterDir.clear();
}

QString Book::enterDir()
{
    return m_strEnterDir;
}


void Book::downloadFile()
{
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if (NULL == pItem)
    {
        QMessageBox::warning(this, "下载文件", "请选择要下载的文件");
    }
    else
    {
        QString strSaveFilePath = QFileDialog::getSaveFileName();
        if (strSaveFilePath.isEmpty())
        {
            QMessageBox::warning(this, "下载文件", "请指定要保存的位置");
            m_strSaveFilePath.clear();
        }
        else
        {
            m_strSaveFilePath = strSaveFilePath;
        }

        QString strCurPath = TcpClient::getInstance().curPath();
        PDU *pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
        QString strFileName = pItem->text();
        strcpy(pdu->caData, strFileName.toStdString().c_str());
        memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    }
}



void Book::setDownloadStatus(bool status)
{
    m_bDownload = status;
}

bool Book::getDownloadStatus()
{
    return m_bDownload;
}

QString Book::getSaveFilePath()
{
    return m_strSaveFilePath;
}

void Book::shareFile()
{
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if (NULL == pItem)
    {
        QMessageBox::warning(this, "分享文件", "请选择要分享的文件");
        return;
    }
    else
    {
        //获取分享文件的名字
        m_strShareFileName = pItem->text();
    }


    //主要目的是为了获取好友的列表
    Friend *pFriend = OpeWidget::getInstance().getFriend();
    QListWidget *pFriendList = pFriend->getFriendList();
    ShareFile::getInstance().updateFriend(pFriendList);
    if (ShareFile::getInstance().isHidden())
    {
        ShareFile::getInstance().show();
    }
}

QString Book::getShareFileName()
{
    return m_strShareFileName;
}

void Book::moveFile()
{
    QListWidgetItem *pCurItem = m_pBookListW->currentItem();
    if (NULL != pCurItem)
    {
        m_strMoveFileName = pCurItem->text();
        QString strCutPath = TcpClient::getInstance().curPath();//客户的服务器地址
        m_strMoveFilePath = strCutPath+'/'+m_strMoveFileName;

        m_pSelectDirPB->setEnabled(true);//可以进行点击
    }
    else
    {
        QMessageBox::warning(this, "移动文件", "请选择要移动的文件");
    }
}

void Book::selectDestDir()
{
    //选择目的Dir
    QListWidgetItem *pCurItem = m_pBookListW->currentItem();
    if (NULL != pCurItem)
    {
        QString strDestDir = pCurItem->text();
        QString strCutPath = TcpClient::getInstance().curPath();
        m_strDestDir = strCutPath+'/'+strDestDir;

        //文件的服务器路径和要移动到的路径，用caMsg封存
        int srcLen = m_strMoveFilePath.size();
        int destLen = m_strDestDir.size();
        PDU *pdu = mkPDU(srcLen+destLen+2);//每个路径后面+/0
        pdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
        //caData存储src的路径大小和des的路径大小，以及文件名
        sprintf(pdu->caData, "%d %d %s", srcLen, destLen, m_strMoveFileName.toStdString().c_str());

        memcpy(pdu->caMsg, m_strMoveFilePath.toStdString().c_str(), srcLen);
        memcpy((char*)(pdu->caMsg)+(srcLen+1), m_strDestDir.toStdString().c_str(), destLen);//需要进行偏移

        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this, "移动文件", "请选择要移动的文件");
    }
    m_pSelectDirPB->setEnabled(false);
}




