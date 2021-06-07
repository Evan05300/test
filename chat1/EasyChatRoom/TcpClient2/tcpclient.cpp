#include "tcpclient.h"
#include <QMessageBox>
#include <QHostInfo>

QString IP; //设置默认IP和port，对于本地是127.0.0.1，对于云服务器则使用对应公网IP
int port;   //port注意服务端与客户端一致

TcpClient::TcpClient(QWidget *parent,Qt::WindowFlags f)
    : QDialog(parent,f)
{
    setWindowTitle(tr("TCP Client "));

    contentListWidget = new QListWidget;

    sendLineEdit = new QLineEdit;
    sendBtn = new QPushButton(tr("send"));

    userNameLabel = new QLabel(tr("user"));
    userNameLineEdit = new QLineEdit;

    serverIPLabel = new QLabel(tr("IP"));
    serverIPLineEdit = new QLineEdit;

    portLabel = new QLabel(tr("PORT"));
    portLineEdit = new QLineEdit;

    enterBtn= new QPushButton(tr("Enter chat"));

    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(contentListWidget,0,0,1,2);
    mainLayout->addWidget(sendLineEdit,1,0);
    mainLayout->addWidget(sendBtn,1,1);
    mainLayout->addWidget(userNameLabel,2,0);
    mainLayout->addWidget(userNameLineEdit,2,1);
    mainLayout->addWidget(serverIPLabel,3,0);
    mainLayout->addWidget(serverIPLineEdit,3,1);
    mainLayout->addWidget(portLabel,4,0);
    mainLayout->addWidget(portLineEdit,4,1);
    mainLayout->addWidget(enterBtn,5,0,1,2);

    status = false;
    IP = "127.0.0.1"; //IP地址，对于本地使用127.0.0.1，对于云服务器，使用对应公网IP即可
    port = 8848; //端口号需要与服务端监听的对应
    serverIPLineEdit->setText(IP);

    portLineEdit->setText(QString::number(port));
//    serverIP =new QHostAddress(QHostAddress::Any);
    serverIP =new QHostAddress();
    connect(enterBtn,SIGNAL(clicked()),this,SLOT(slotEnter()));
    connect(sendBtn,SIGNAL(clicked()),this,SLOT(slotSend()));

    sendBtn->setEnabled(false);
}

TcpClient::~TcpClient()
{
    
}

void TcpClient::slotEnter()
{

    if(!status)
    {
        if(userNameLineEdit->text()=="") //防止用户名为空，中文用户名可能乱码
        {
            QMessageBox::information(this,tr("error"),tr("User name error!"));
            return;
        }

        userName=userNameLineEdit->text();

        tcpSocket = new QTcpSocket(this);
        //检测链接信号
        connect(tcpSocket,SIGNAL(connected()),this,SLOT(slotConnected()));
        //检测如果断开
        connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(slotDisconnected()));
        //检测如果有新可以读信号
        connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(dataReceived()));

        tcpSocket->connectToHost(IP,port);
        //连接服务端
        status=true;
    }
    else
    {
        int length=0;
        QString msg=userName+tr(":Leave Chat Room");  //
        if((length=tcpSocket->write(msg.toLatin1(),msg.length()))!=msg. length())
        {
            return;
        }

        tcpSocket->disconnectFromHost();

        status=false;
    }
}
//链接后
void TcpClient::slotConnected()
{
    sendBtn->setEnabled(true);
    enterBtn->setText(tr("quit")); //连接成功后改变按钮状态

    int length=0;
    QString msg=userName+tr(":Enter Chat Room");
    if((length=tcpSocket->write(msg.toLatin1(),msg.length()))!=msg.length()) //数据发送异常则返回
    {
        return;
    }
}

void TcpClient::slotSend()
{
    if(sendLineEdit->text()=="")
    {
        return ;
    }

    QString msg=userName+":"+sendLineEdit->text();

    tcpSocket->write(msg.toLatin1(),msg.length());
    sendLineEdit->clear();
}

void TcpClient::slotDisconnected()
{
    sendBtn->setEnabled(false); //失去连接后还原按钮状态，等待下一次连接
    enterBtn->setText(tr("Enter Chat"));
}

void TcpClient::dataReceived()
{
    while(tcpSocket->bytesAvailable()>0)
    {
        QByteArray datagram;
        datagram.resize(tcpSocket->bytesAvailable());

        tcpSocket->read(datagram.data(),datagram.size()); //读取到的数据存放到datagram中，对于
                                      //字节型数据可以通过  datagram.at(i)查看

        QString msg=datagram.data();
        contentListWidget->addItem(msg.left(datagram.size()));
    }
}
