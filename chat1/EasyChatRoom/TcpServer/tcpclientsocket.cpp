#include "tcpclientsocket.h"

TcpClientSocket::TcpClientSocket(QObject *parent)
{
    connect(this,SIGNAL(readyRead()),this,SLOT(dataReceived()));//接收信号
    connect(this,SIGNAL(disconnected()),this,SLOT(slotDisconnected()));
}
//接收客户端消息
void TcpClientSocket::dataReceived()
{
    while(bytesAvailable()>0)
    {
        int length = bytesAvailable();
        char buf[1024];
        read(buf,length);

        QString msg=buf;
        emit updateClients(msg,length);//发送客户端更新信号
    }
}

void TcpClientSocket::slotDisconnected()
{
    emit disconnected(this->socketDescriptor());//发送断开连接信号
}
