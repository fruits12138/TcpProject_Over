#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QInputDialog>
#include "privatechat.h"
#include <QMessageBox>

Friend::Friend(QWidget *parent) : QWidget(parent)
{
    m_pShowMsgTE = new QTextEdit;
    m_pShowMsgTE->setReadOnly(true);
    m_pFriendListWidget = new QListWidget;
    m_pInputMsgLE = new QLineEdit;

    m_pDelFriendPB = new QPushButton("DEL FRIEND");
    m_pFlushFriendPB = new QPushButton("FLUSH FRIEND");
    m_pShowOnlineUsrPB = new QPushButton("SHOW ONLINE FRIEND");
    m_pSearchUsrPB = new QPushButton("SEARCH USR");
    m_pMsgSendPB = new QPushButton("Msg Send");
    m_pPrivateChatPB = new QPushButton("Private Chat");

    QVBoxLayout *pRightPBVBL = new QVBoxLayout;
    pRightPBVBL->addWidget(m_pDelFriendPB);
    pRightPBVBL->addWidget(m_pFlushFriendPB);
    pRightPBVBL->addWidget(m_pShowOnlineUsrPB);
    pRightPBVBL->addWidget(m_pSearchUsrPB);
    pRightPBVBL->addWidget(m_pPrivateChatPB);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRightPBVBL);

    QHBoxLayout *pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);

    m_pOnline = new Online;

    QVBoxLayout *pMain = new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();

    setLayout(pMain);

    connect(m_pShowOnlineUsrPB,SIGNAL(clicked(bool))
            ,this,SLOT(showOnline()));
    connect(m_pSearchUsrPB,SIGNAL(clicked(bool))
            ,this,SLOT(searchUsr()));

    connect(m_pFlushFriendPB,SIGNAL(clicked(bool))
            ,this,SLOT(flushFriend()));

    connect(m_pDelFriendPB,SIGNAL(clicked(bool))
            ,this,SLOT(delFriend()));
    connect(m_pPrivateChatPB,SIGNAL(clicked(bool))
            ,this,SLOT(privateChat()));
    connect(m_pMsgSendPB,SIGNAL(clicked(bool))
            ,this,SLOT(groupChat()));
    connect(m_pInputMsgLE,SIGNAL(returnPressed())
            ,this,SLOT(enterPressed()));

}

void Friend::showAllOnlineUsr(PDU *pdu)
{
    if(pdu == NULL){
        return;
    }
    m_pOnline->showUsr(pdu);
}

void Friend::updateFriendList(PDU *pdu)
{
    if(pdu == NULL){
        return;
    }
    uint uiSize = pdu->uiMsgLen/32;
    qDebug()<<"name size::"<<uiSize;
    char caName[32] = {'\0'};
    m_pFriendListWidget->clear();
    for(uint i=0;i<uiSize;i++){
        memcpy(caName,(char*)pdu->caMsg+i*32,32);
        m_pFriendListWidget->addItem(caName);
    }
}

void Friend::updateGroupMsg(PDU *pdu)
{

    QString strMsg = QString("%1 says: %2").arg(pdu->caData).arg((char*)pdu->caMsg);
    m_pShowMsgTE->append(strMsg);
}

void Friend::showOnline()
{
    if(m_pOnline->isHidden()){
        m_pOnline->show();

        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;

    }
    else{
        m_pOnline->hide();
    }
}

void Friend::searchUsr()
{
    m_strSearchName = QInputDialog::getText(this,"Search","Usr Name:");
    if(!m_strSearchName.isEmpty()){
        qDebug()<<m_strSearchName;
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_REQUEST;


        strncpy(pdu->caData,m_strSearchName.toStdString().c_str(),32);

        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else{

    }
}

void Friend::flushFriend()
{
    QString strName  = TcpClient::getInstance().loginName();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Friend::delFriend()
{
    if(NULL!=m_pFriendListWidget->currentItem()){
        QString strFriendName = m_pFriendListWidget->currentItem()->text();
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
        QString strSelfName = TcpClient::getInstance().loginName();

        memcpy(pdu->caData,strSelfName.toStdString().c_str(),strSelfName.size());
        memcpy(pdu->caData+32,strFriendName.toStdString().c_str(),strFriendName.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }

}

void Friend::privateChat()
{
    if(NULL!=m_pFriendListWidget->currentItem()){
        QString strChatName = m_pFriendListWidget->currentItem()->text();
        PrivateChat::getInstance().setChatName(strChatName);
        if(PrivateChat::getInstance().isHidden()){
            PrivateChat::getInstance().show();
        }


    }else {
        QMessageBox::warning(this,"private chat","please select the person you wanna chat");
    }
}

void Friend::groupChat()
{
    QString strMsg = m_pInputMsgLE->text();
    m_pInputMsgLE->clear();

    if(strMsg!=NULL)
    {
         PDU *pdu = mkPDU(strMsg.size()+1);
         pdu->uiMsgType = ENUM_MSG_TYPE_GROUP_CHAT_REQUEST;
         QString strName = TcpClient::getInstance().loginName();
         strncpy(pdu->caData,strName.toStdString().c_str(),strName.size());
         strncpy((char*)pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());
         TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
         free(pdu);
         pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this,"Group Chat ","send msg is NULL");
    }
}

void Friend::enterPressed()
{
    QString strMsg = m_pInputMsgLE->text();
    m_pInputMsgLE->clear();

    if(strMsg!=NULL)
    {
         PDU *pdu = mkPDU(strMsg.size()+1);
         pdu->uiMsgType = ENUM_MSG_TYPE_GROUP_CHAT_REQUEST;
         QString strName = TcpClient::getInstance().loginName();
         strncpy(pdu->caData,strName.toStdString().c_str(),strName.size());
         strncpy((char*)pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());
         TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
         free(pdu);
         pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this,"Group Chat ","send msg is NULL");
    }
}


QListWidget* Friend::getFriendList()
{
    return m_pFriendListWidget;
}
