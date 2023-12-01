#include "opedb.h"
#include <QMessageBox>
#include <QDebug>
#include<tcpserver.h>

OpeDB::OpeDB(QObject *parent) : QObject(parent)
{
    //先告诉数据库，自己是什么数据库，目前是SQlite3数据库
    m_db =QSqlDatabase::addDatabase("QSQLITE");
//    init();
}

OpeDB &OpeDB::getInstance()
{
    static OpeDB instance;
    return instance;
}

void OpeDB::init()
{
    m_db.setHostName("localhost");
    m_db.setDatabaseName("E:\\code_file\\C_file_QT\\My_version\\TcpServer\\cloud.db");
    if(m_db.open()){
        QSqlQuery query;
        query.exec("select * from usrInfo");
        while(query.next()){
            QString data = QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
            qDebug()<<data;
        }
    }else{
        QMessageBox::critical(NULL,"open database","open database failed");
    }

}

bool OpeDB::handleRegist(const char *name, const char *pwd)
{
    if(name==NULL||pwd==NULL){
        qDebug()<<"name|pwd is null";
        return false;
    }

    QString data = QString("insert into usrInfo(name,pwd) values(\'%1\', \'%2\')").arg(name).arg(pwd);

    QSqlQuery query;
//    query.prepare("insert into usrInfo(name,pwd) values(\'name\', \'pwd\')");
//    query.bindValue(":name", name);
//    query.bindValue(":pwd", pwd);
    return query.exec(data);
}

bool OpeDB::handleLogin(const char *name, const char *pwd)
{
    if(name==NULL||pwd==NULL){
        qDebug()<<"name|pwd is null";
        return false;
    }
    QString data = QString("select * from usrInfo where name=\'%1\' and pwd=\'%2\' and online=0").arg(name).arg(pwd);
    qDebug()<<data;
    QSqlQuery query;
    query.exec(data);
    if(query.next()){
        QString data = QString("update usrInfo set online=1 where name=\'%1\' and pwd=\'%2\'").arg(name).arg(pwd);
        qDebug()<<data;
        QSqlQuery query;
        query.exec(data);
        return true;
    }else {
        return false;
    };

}

void OpeDB::handleOffline(const char *name)
{
    if(name==NULL){
        qDebug()<<"name|pwd is null";
        return;

    }
    QString data = QString("update usrInfo set online=0 where name=\'%1\' and online=1").arg(name);
    qDebug()<<data;
    QSqlQuery query;
    query.exec(data);
}

QStringList OpeDB::handleAllOnline()
{
    QString data = QString("select name from usrInfo where online = 1");

    qDebug()<<data;
    QSqlQuery query;
    query.exec(data);

    QStringList result;
    result.clear();

    while(query.next()){
         result.append(query.value(0).toString());
    }
    return result;
}

QStringList OpeDB::handleAllUsr()
{

    QString data = QString("select name from usrInfo");
    qDebug()<<data;
    QSqlQuery query;
    query.exec(data);

    QStringList result;
    result.clear();

    while(query.next()){
         result.append(query.value(0).toString());
    }
    return result;
}

int OpeDB::handleSearchUsr(const char *name)
{
    if(name==NULL){
        return -1;
    }
    QString data = QString("select online from usrInfo where name=\'%1\'").arg(name);
    qDebug()<<data;
    QSqlQuery query;
    query.exec(data);
    if(query.next()){
        int ret = query.value(0).toInt();
        if(ret == 1){
            return 1;
        }else if(ret == 0){
            return 0;
        }
    }else {
        return -1;
    }

}

int OpeDB::handleAddFriend(const char *pername, const char *name)
{
    if(pername == NULL or name ==NULL){
        return -1;

    }
    QString data = QString("select * from friend where (id=(select id from usrInfo where name = \'%1\') and friendId = (select id from usrInfo where name = \'%2\')) or "
                           "(id=(select id from usrInfo where name = \'%3\') and friendId = (select id from usrInfo where name = \'%4\'))").arg(pername).arg(name).arg(name).arg(pername);
    qDebug()<<data;
    QSqlQuery query;
    query.exec(data);
    if(query.next()){
        return 0; //双方已经是好友
    }else {
        //查询对方是否在线
        QString data = QString("select online from usrInfo where name=\'%1\'").arg(pername);
        qDebug()<<data;
        QSqlQuery query;
        query.exec(data);
        if(query.next()){
            int ret = query.value(0).toInt();
            if(ret == 1){
                return 1;//在线



            }else if(ret == 0){
                return 2;//不在线



            }
        }else {
            return 3;//不存在
        }
    }

}

void OpeDB::handleAgreeAddFriend(const char *pername, const char *name)
{
    if(pername == NULL or name ==NULL){
        return;
    }
    QString data = QString("insert into friend(id,friendId) values((select id from usrInfo where name = \'%1\')"
                           ",(select id from usrInfo where name = \'%2\'));").arg(name).arg(pername);
    qDebug()<<data;
    QSqlQuery query;
    query.exec(data);
}

QStringList OpeDB::handleFulshFriend(const char *name)
{

    QStringList strFriendList;
    strFriendList.clear();
    if(NULL == name){
        return strFriendList;
    }

    qDebug()<<"handleFulshFriend::"<<name;
    QString data = QString("select name from usrInfo where online = 1 and id in (select id from friend where friendId = (select id from usrInfo where name =\'%1\'))").arg(name);

    qDebug()<<data;
    QSqlQuery query;
    query.exec(data);

    while(query.next()){
         strFriendList.append(query.value(0).toString());
         qDebug()<<query.value(0).toString();
    }

     data = QString("select name from usrInfo where online = 1 and id in (select friendId from friend where id = (select id from usrInfo where name =\'%1\'))").arg(name);

     qDebug()<<data;

     query.exec(data);
     while(query.next()){
        strFriendList.append(query.value(0).toString());
        qDebug()<<query.value(0).toString();
     }

     return strFriendList;
}

bool OpeDB::handleDelFriend(const char *name, const char *friendName)
{
    if(name == NULL or friendName ==NULL){
        return false;
    }
    qDebug()<<"删除好友";
    QString data = QString("delete from friend where id=(select id from usrInfo where name = \'%1\') and friendId = (select id from usrInfo where name = \'%2\')").arg(name).arg(friendName);
    qDebug()<<data;
    QSqlQuery query;
    query.exec(data);

    data = QString("delete from friend where id=(select id from usrInfo where name = \'%1\') and friendId = (select id from usrInfo where name = \'%2\')").arg(friendName).arg(name);
    qDebug()<<data;

    query.exec(data);

    return true;
}

QStringList OpeDB::handleGroupChat()
{
    QStringList strFriendList;
    strFriendList.clear();


    qDebug()<<"handleFulshFriend::";
    QString data = QString("select name from usrInfo where online = 1") ;

    qDebug()<<data;
    QSqlQuery query;
    query.exec(data);

    while(query.next()){
         strFriendList.append(query.value(0).toString());
         qDebug()<<query.value(0).toString();
    }
    return strFriendList;
}

OpeDB::~OpeDB()
{
    m_db.close();

}


