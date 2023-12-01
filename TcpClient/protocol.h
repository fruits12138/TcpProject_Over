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
#define DIR_NO_EXIST "cur dir not exist"
#define FILE_NAME_EXIST "file name exist"

#define DEL_DIR_OK "delete dir ok"
#define DEL_DIR_FAILED "delete dir failed:is file"

#define RENAME_FILE_OK "rename file ok"
#define RENAME_FILE_FAILED "rename file failed"

#define ENTER_DIR_FAILED "enter dir failed:a file"

#define DEL_FILE_OK "delete file ok"
#define DEL_FILE_FAILED "delete file failed:is dir"

#define UPLOAD_FILE_OK "upload file ok"
#define UPLOAD_FILE_FAILED "upload file failed"

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

    ENUM_MSG_TYPE_CREATE_DIR_REQUEST,//创建dir请求
    ENUM_MSG_TYPE_CREATE_DIR_RESPONSE,

    ENUM_MSG_TYPE_FLUSH_FILE_REQUEST,//刷新文件请求
    ENUM_MSG_TYPE_FLUSH_FILE_RESPONSE,

    ENUM_MSG_TYPE_DEL_DIR_REQUEST,//删除目录请求
    ENUM_MSG_TYPE_DEL_DIR_RESPONSE,

    ENUM_MSG_TYPE_RENAME_FILE_REQUEST,  //重命名请求
    ENUM_MSG_TYPE_RENAME_FILE_RESPONSE,

    ENUM_MSG_TYPE_ENTER_DIR_REQUEST,    //进入文件夹请求
    ENUM_MSG_TYPE_ENTER_DIR_RESPONSE,

    ENUM_MSG_TYPE_DEL_FILE_REQUEST, //删除文件请求
    ENUM_MSG_TYPE_DEL_FILE_RESPONSE,

    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST,//上传文件请求
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPONSE,

    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST,//下载请求
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPONSE,

    ENUM_MSG_TYPE_SHARE_FILE_REQUEST, //共享文件请求
    ENUM_MSG_TYPE_SHARE_FILE_RESPONSE, //共享文件回复

    ENUM_MSG_TYPE_SHARE_FILE_NOTE,
    ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPONSE,

    ENUM_MSG_TYPE_MOVE_FILE_REQUEST, //移动文件请求
    ENUM_MSG_TYPE_MOVE_FILE_RESPONSE, //移动文件回复

    ENUM_MSG_TYPE_MAX = 0x00FFFFFF
} ;

struct FileInfo
{
    char caFileName[32];
    int iFileType;
};

struct PDU{
    uint uiPDULen;  //一次数据的总长，总的协议数据单元大小
    uint uiMsgType; //消息的类型
    char caData[64];
    uint uiMsgLen;  //实际消息大小
    int caMsg[];    //实际消息
};

PDU* mkPDU(uint uiMsgLen);




#endif // PROTOCOL_H
