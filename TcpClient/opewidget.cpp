#include "opewidget.h"
#include "tcpclient.h"

OpeWidget::OpeWidget(QWidget *parent) : QWidget(parent)
{
    m_pListWidget = new QListWidget(this);
    m_pListWidget->addItem("friend");
    m_pListWidget->addItem("book");

    m_pFriend = new Friend;
    m_pBook = new Book;

    m_pSW = new QStackedWidget;
    m_pSW->addWidget(m_pFriend);
    m_pSW->addWidget(m_pBook);

    QHBoxLayout *pMain1 = new QHBoxLayout;
    pMain1->addWidget(m_pListWidget);
    pMain1->addWidget(m_pSW);

    m_pQL = new QLabel;
    QString label = "ClientName::";
    label.append(TcpClient::getInstance().loginName());
    m_pQL->setText(label);

    QVBoxLayout *pMain = new QVBoxLayout;
    pMain->addWidget(m_pQL);
    pMain->addLayout(pMain1);

    setLayout(pMain);

    connect(m_pListWidget,SIGNAL(currentRowChanged(int))
            ,m_pSW,SLOT(setCurrentIndex(int)));
}

OpeWidget &OpeWidget::getInstance()
{
    static OpeWidget instance;
    return instance;
}

Friend *OpeWidget::getFriend()
{
    return m_pFriend;
}

Book *OpeWidget::getBook()
{
    return m_pBook;
}
