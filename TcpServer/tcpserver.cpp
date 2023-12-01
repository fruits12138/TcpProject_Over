#include "tcpserver.h"
#include "ui_tcpserver.h"

#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <QFile>

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);
}

void TcpServer::loadConfig()
{
    QFile file(":/server.config");
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();

        strData.replace("\r\n"," ");
        file.close();


        QStringList stringList = strData.split(" ");

        m_strIP = stringList.at(0);
        m_usPort = stringList.at(1).toUShort();
        qDebug()<<"IP:"<<m_strIP<<"Port:"<<m_usPort;
    }else{
        QMessageBox::critical(this,"open config","open config failed");
    }
}
TcpServer::~TcpServer()
{
    delete ui;
}




