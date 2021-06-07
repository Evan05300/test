#include "server.h"

Server::Server(QObject *parent,int port)
    :QTcpServer(parent)
{
    listen(QHostAddress::Any,port); //监听端口消息，端口设置在tcpsever中
}
int i =0;
int ip_num = 0;
//incommingConnection函数是针对多客户端连接，采用一个连接开启一个线程的方式建立通信
//优点是多个节点上传不会互相影响，以前没使用这种方式的时候，新连接的客户端会把旧的挤掉
void Server::incomingConnection(qintptr socketDescriptor)
{
    //链接一个会新创建一个
    TcpClientSocket *tcpClientSocket=new TcpClientSocket(this);
    connect(tcpClientSocket,SIGNAL(updateClients(QString,int)),this,SLOT(updateClients(QString,int)));
    connect(tcpClientSocket,SIGNAL(disconnected(int)),this,SLOT(slotDisconnected(int)));

    tcpClientSocket->setSocketDescriptor(socketDescriptor);

    tcpClientSocketList.append(tcpClientSocket);

/*  这一段程序是另一种考虑，假如我只想给最近一次连接服务器的客户端发数据，我只需读他的IP并比较tcpClientSocketList
 *  的对应IP序号i是多少，那么我就可以在更新客户端数据时只更新这个IP的数据
 *  具体现象：客户端A先连接，客户端B后连接，若使用客户端A发数据，那么只有客户端B收到数据
 * QString IP2 = tcpClientSocket->peerAddress().toString();
    for(i=0;i<tcpClientSocketList.count();i++)
    {
        if(IP2 == tcpClientSocketList.at(i)->peerAddress().toString())
        {
            ip_num = i;
            qDebug()<<ip_num;
        }
    }
*/
}
//更新数据，将接收到的信息更新到服务端程序和客户端程序中，
//发送updateClients信号的函数在tcpclientsocket.cpp中
void Server::updateClients(QString msg,int length)
{
    emit updateServer(msg,length); //更新数据到服务端程序
    for(int i=0;i<tcpClientSocketList.count();i++)
    {

        QTcpSocket *item = tcpClientSocketList.at(i);//更新数据到每一个客户端程序
        if(item->write(msg.toLatin1(),length)!=length)//发送数据
        {
            continue;
        }
    }

    /*
    注释掉上面的for循环，结合上一个程序中注释掉的内容就可以实现只给最近连接的客户端发数据
    QTcpSocket *item = tcpClientSocketList.at(i);
    item->write(msg.toLatin1(),length);
    */

}

void Server::slotDisconnected(int descriptor)
{
    for(int i=0;i<tcpClientSocketList.count();i++)
    {
        QTcpSocket *item = tcpClientSocketList.at(i);
        if(item->socketDescriptor()==descriptor)
        {
            tcpClientSocketList.removeAt(i);//移除已断开连接的客户端
            return;
        }
    }
    return;
}
