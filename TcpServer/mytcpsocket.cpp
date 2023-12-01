#include "mytcpsocket.h"
#include"opedb.h"
#include <QDebug>
#include "mytcpserver.h"

MyTcpSocket::MyTcpSocket(QObject *parent) : QTcpSocket(parent)
{
    connect(this,SIGNAL(readyRead()),
            this,SLOT(recvMsg()));
    connect(this,SIGNAL(disconnected()),
            this,SLOT(clientOffline()));

}

QString MyTcpSocket::getName()
{
    return m_strName;
}

void MyTcpSocket::recvMsg()
{
    //接收该socket中的PDU
    qDebug()<<this->bytesAvailable();
    uint uiPDULen = 0;
    this->read((char*)&uiPDULen,sizeof(uint));
    uint uiMsgLen = uiPDULen-sizeof(PDU);
    PDU*pdu = mkPDU(uiMsgLen);
    this->read((char*)pdu+sizeof(uint),uiPDULen);


//  qDebug()<<pdu->uiMsgType<<(char*)(pdu->caMsg);
    //根据PDU的消息类型进行不同的操作
    switch (pdu->uiMsgType)
    {
    case ENUM_MSG_TYPE_REGISTER_REQUEST:
    {
        char caName[32] ={"/0"};
        char caPwd[32] ={"/0"};
        strncpy(caName,pdu->caData,32);
        strncpy(caPwd,pdu->caData+32,32);
        bool ret = OpeDB::getInstance().handleRegist(caName,caPwd);

        PDU *respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_REGISTER_RESPONSE;
        qDebug()<<ret;
        if(ret){
            strcpy(respdu->caData, REGIST_OK);

        }else{
            strcpy(respdu->caData, REGIST_FAILED);
        }

        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
//        qDebug()<<"Name:"<<(char*)caName;
//        qDebug()<<"Pwd:"<<(char*)caPwd;
//        qDebug()<<"Msg_Type:"<<pdu->uiMsgType;
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_REQUEST:
    {
        char caName[32] ={"/0"};
        char caPwd[32] ={"/0"};
        strncpy(caName,pdu->caData,32);
        strncpy(caPwd,pdu->caData+32,32);
        bool ret = OpeDB::getInstance().handleLogin(caName,caPwd);
        PDU *respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPONSE;
        qDebug()<<ret;
        if(ret){
            strcpy(respdu->caData, LOGIN_OK);
            m_strName = caName;

        }else{
            strcpy(respdu->caData, LOGIN_FAILED);
        }

        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
    {
        QStringList ret = OpeDB::getInstance().handleAllOnline();
        uint uiMsgLen = ret.size()*32;
        PDU *respdu = mkPDU(uiMsgLen);
        respdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPONSE;
        qDebug()<<ret;
        for(int i=0;i<ret.size();i++){
            memcpy((char*)(respdu->caMsg)+i*32
                   ,ret.at(i).toStdString().c_str()
                   ,ret.at(i).size());

        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;

    }
    case ENUM_MSG_TYPE_ALL_USR_REQUEST:
    {
        QStringList ret = OpeDB::getInstance().handleAllUsr();
        uint uiMsgLen = ret.size()*32;
        PDU *respdu = mkPDU(uiMsgLen);
        respdu->uiMsgType = ENUM_MSG_TYPE_ALL_USR_RESPONSE;
        qDebug()<<ret;
        for(int i=0;i<ret.size();i++){
            memcpy((char*)(respdu->caMsg)+i*32
                   ,ret.at(i).toStdString().c_str()
                   ,ret.at(i).size());

        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USR_REQUEST:
    {

        int ret = OpeDB::getInstance().handleSearchUsr(pdu->caData);

        PDU *respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_RESPONSE;
        qDebug()<<ret;
        if(ret == -1){
            strcpy(respdu->caData,SEARCH_USR_NO);
            qDebug()<<(char*)(respdu->caData);
        }else if(ret == 1){
            strcpy(respdu->caData,SEARCH_USR_ONLINE);
            qDebug()<<(char*)(respdu->caData);
        }else if(ret == 0){
            strcpy(respdu->caData,SEARCH_USR_OFFLINE);
            qDebug()<<(char*)(respdu->caData);
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;


        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
    {
       char caName[32] ={"/0"};
       char caPerName[32] ={"/0"};
       strncpy(caPerName,pdu->caData,32);
       strncpy(caName,pdu->caData+32,32);
       int ret = OpeDB::getInstance().handleAddFriend(caPerName,caName);

       PDU* respdu = NULL;

       if(ret==-1){
           respdu = mkPDU(0);
           respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE;
           strcpy(respdu->caData,UNKNOW_ERROR);
           write((char*)respdu,respdu->uiPDULen);
           free(respdu);
           respdu = NULL;


       }else if(ret==0){
           respdu = mkPDU(0);
           respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE;
           strcpy(respdu->caData,EXISTED_FRIEND);
           write((char*)respdu,respdu->uiPDULen);
           free(respdu);
           respdu = NULL;




       }else if(ret==1){


           MyTcpServer::getInstance().resend(caPerName,pdu);



       }else if(ret==2){
           respdu = mkPDU(0);
           respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE;
           strcpy(respdu->caData,ADD_FRIEND_OFFLINE);

           write((char*)respdu,respdu->uiPDULen);
           free(respdu);
           respdu = NULL;


       }else if(ret==3){
           respdu = mkPDU(0);
           respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE;
           strcpy(respdu->caData,ADD_FRIEND_NO_EXSIT);
           write((char*)respdu,respdu->uiPDULen);
           free(respdu);
           respdu = NULL;

       }


       break;



    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE:
    {
        char caName[32] ={"/0"};
        char caPerName[32] ={"/0"};
        strncpy(caPerName,pdu->caData,32);
        strncpy(caName,pdu->caData+32,32);
        OpeDB::getInstance().handleAgreeAddFriend(caPerName,caName);




        PDU *respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE;
        strcpy(respdu->caData,ADD_FRIEND_AGREE);

        MyTcpServer::getInstance().resend(caName,respdu);

        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
    {
        char caName[32] ={"/0"};

        strncpy(caName,pdu->caData+32,32);
        PDU *respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE;
        strcpy(respdu->caData,ADD_FRIEND_REFUSE);
        MyTcpServer::getInstance().resend(caName,respdu);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:
    {
        char caName[32] ={"/0"};
        strncpy(caName,pdu->caData,32);
        qDebug()<<caName;
        QStringList ret = OpeDB::getInstance().handleFulshFriend(caName);
        uint uiMsgLen = ret.size()*32;
        PDU *respdu = mkPDU(uiMsgLen);
        respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPONSE;

        for(int i=0;i<ret.size();i++){
            memcpy((char*)(respdu->caMsg)+i*32
                   ,ret.at(i).toStdString().c_str()
                   ,ret.at(i).size());
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
    {

        char caSelfName[32] = {"\0"};
        char caFriendName[32] = {"\0"};
        strncpy(caSelfName,pdu->caData,32);
        strncpy(caFriendName,pdu->caData+32,32);
        OpeDB::getInstance().handleDelFriend(caSelfName,caFriendName);

        PDU *respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPONSE;
        strcpy(respdu->caData,DEL_FRIEND_OK);
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;


        MyTcpServer::getInstance().resend(caFriendName,pdu);


        break;


    }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
    {
        char caPerName[32] ={'\0'};
        memcpy(caPerName,pdu->caData+32,32);
        MyTcpServer::getInstance().resend(caPerName,pdu);
        break;
    }
    case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:
    {
        char caName[32] ={"/0"};
        strncpy(caName,pdu->caData,32);
        qDebug()<<caName;
        QStringList onlineFriend = OpeDB::getInstance().handleGroupChat();
        QString tmp;
        for (int i=0;i<onlineFriend.size();i++) {
            tmp = onlineFriend.at(i);
            MyTcpServer::getInstance().resend(tmp.toStdString().c_str(),pdu);
        }
        break;
    }
    default:
    {
        break;
    }
    free(pdu);
    pdu = NULL;
    }


}

void MyTcpSocket::clientOffline()
{
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());
    emit offline(this);
}
