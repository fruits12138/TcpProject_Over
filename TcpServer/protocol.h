#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef unsigned int uint;

#define REGIST_OK "regist ok"
#define REGIST_FAILED "regist failed:name existed"
#define LOGIN_OK "login ok"
#define LOGIN_FAILED "login failed:name error or pwd error or relogin "

#define SEARCH_USR_NO "No such people"
#define SEARCH_USR_ONLINE "Online"
#define SEARCH_USR_OFFLINE "Offline"

#define UNKNOW_ERROR "unknow error"
#define EXISTED_FRIEND "friend exist"
#define ADD_FRIEND_OFFLINE "usr offline"
#define ADD_FRIEND_NO_EXSIT "usr not exist"

#define ADD_FRIEND_AGREE "add friend agree"
#define ADD_FRIEND_REFUSE "add friend refuse"

#define DEL_FRIEND_OK "delete friend ok"


enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN = 0,
    ENUM_MSG_TYPE_REGISTER_REQUEST,//注册请求
    ENUM_MSG_TYPE_REGISTER_RESPONSE,

    ENUM_MSG_TYPE_LOGIN_REQUEST,//登陆请求
    ENUM_MSG_TYPE_LOGIN_RESPONSE,

    ENUM_MSG_TYPE_ALL_USR_REQUEST,//展示全部的用户
    ENUM_MSG_TYPE_ALL_USR_RESPONSE,

    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,//展示全部的在线用户
    ENUM_MSG_TYPE_ALL_ONLINE_RESPONSE,

    ENUM_MSG_TYPE_SEARCH_USR_REQUEST,//搜索用户请求
    ENUM_MSG_TYPE_SEARCH_USR_RESPONSE,

    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,//添加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE,

    ENUM_MSG_TYPE_ADD_FRIEND_AGREE,//添加好友同意
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,//添加好友拒绝

    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST,//刷新好友请求
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPONSE,//刷新好友回复

    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,//删除好友请求
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPONSE,

    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST,//私聊请求
    ENUM_MSG_TYPE_PRIVATE_CHAT_RESPONSE,


    ENUM_MSG_TYPE_GROUP_CHAT_REQUEST,//群聊请求
    ENUM_MSG_TYPE_GROUP_CHAT_RESPONSE,
    //    ENUM_MSG_TYPE_REQUEST,
    //    ENUM_MSG_TYPE_RESPONSE,
    ENUM_MSG_TYPE_MAX = 0x00FFFFFF
} ;

struct PDU{
    uint uiPDULen;  //一次数据的总长，总的协议数据单元大小
    uint uiMsgType; //消息的类型
    char caData[64];
    uint uiMsgLen;  //实际消息大小
    int caMsg[];    //实际消息
};

PDU* mkPDU(uint uiMsgLen);




#endif // PROTOCOL_H
