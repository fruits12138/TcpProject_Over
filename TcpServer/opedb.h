#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>

class OpeDB : public QObject
{
    Q_OBJECT
public:
    explicit OpeDB(QObject *parent = nullptr);
    static OpeDB& getInstance();
    void init();

    bool handleRegist(const char* name,const char* pwd);
    bool handleLogin(const char* name,const char* pwd);
    void handleOffline(const char* name);
    QStringList handleAllOnline();
    QStringList handleAllUsr();
    int handleSearchUsr(const char* name);
    int handleAddFriend(const char* pername,const char* name);
    void handleAgreeAddFriend(const char* pername,const char* name);

    QStringList handleFulshFriend(const char* name);

    bool handleDelFriend(const char* name,const char* friendName);

    QStringList handleGroupChat();

    ~OpeDB();
signals:
private:
    QSqlDatabase m_db;
};

#endif // OPEDB_H
