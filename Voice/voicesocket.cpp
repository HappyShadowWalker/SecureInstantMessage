#include "voicesocket.h"

VoiceSocket::VoiceSocket(QObject *parent) :
    QObject(parent)
{
    isEnabled = false;
    connect(&udp, SIGNAL(readyRead()), this, SLOT(udp_readyRead()));
}

void VoiceSocket::connectToHost(QHostAddress ip, quint16 port)
{
    peer_ip = ip;
    peer_port = port;
}

void VoiceSocket::startListen(quint16 port)
{
    udp.bind(QHostAddress::Any, port);//必须调用Listen监听某个端口
}

void VoiceSocket::writeData(QByteArray data)
{
    if(!isEnabled) return;
    QByteArray data2 = qCompress(data, 9);
    udp.writeDatagram(data2, peer_ip, peer_port);
}

void VoiceSocket::udp_readyRead()
{
    int size = udp.pendingDatagramSize();
    QHostAddress host;
    quint16 port;
    QByteArray data;
    data.resize(size);
    udp.readDatagram(data.data(), size, &host, &port);
    if(!isEnabled) return;
    if(host != peer_ip) return;
    QByteArray data2 = qUncompress(data);
    emit readData(data2);
}

void VoiceSocket::setEnabled(bool val)
{
    isEnabled = val;
}
