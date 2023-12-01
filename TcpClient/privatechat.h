#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include <QWidget>
#include "protocol.h"
namespace Ui {
class PrivateChat;
}

class PrivateChat : public QWidget
{
    Q_OBJECT

public:
    explicit PrivateChat(QWidget *parent = nullptr);
    void setChatName(QString strName);
    static PrivateChat &getInstance();
    void updateMsg(const PDU *pdu);
    ~PrivateChat();

private slots:
    void on_sendMsg_pb_clicked();

    void on_inputMsg_le_returnPressed();

private:
    Ui::PrivateChat *ui;
    QString m_strChatName;
    QString m_strLoginName;
};

#endif // PRIVATECHAT_H
