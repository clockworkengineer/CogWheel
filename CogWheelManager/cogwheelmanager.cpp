/*
 * File:   cogwheelmanager.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

//
// Class: CogWheelManager
//
// Description:
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelmanager.h"

/**
 * @brief CogWheelManager::CogWheelManager
 * @param parent
 */
CogWheelManager::CogWheelManager(QObject *parent) : QObject(parent)
{

}

/**
 * @brief CogWheelManager::startManager
 * @param socketName
 */
void CogWheelManager::startManager(const QString &socketName)
{

    m_managerSocket = new QLocalSocket();
    if (m_managerSocket==nullptr) {
        qDebug() << "Error in creating manager socket.";
    }

    connect(m_managerSocket,&QLocalSocket::connected, this, &CogWheelManager::connected);
    connect(m_managerSocket,&QLocalSocket::disconnected, this, &CogWheelManager::disconnected);
    connect(m_managerSocket,static_cast<void(QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error), this, &CogWheelManager::error);
    connect(m_managerSocket,&QLocalSocket::readyRead, this, &CogWheelManager::readyRead);
    connect(m_managerSocket,&QLocalSocket::bytesWritten, this, &CogWheelManager::bytesWritten);

    m_managerSocket->connectToServer(socketName);
    m_managerSocket->waitForConnected(-1);

}

/**
 * @brief CogWheelManager::stopManager
 */
void CogWheelManager::stopManager()
{

}

/**
 * @brief CogWheelManager::connected
 */
void CogWheelManager::connected()
{

  qDebug() << "Manager Connected";

}

/**
 * @brief CogWheelManager::disconnected
 */
void CogWheelManager::disconnected()
{

    qDebug() << "Manager disconnected";

    if (m_managerSocket) {
        m_managerSocket->deleteLater();
        m_managerSocket=nullptr;
    }

}

/**
 * @brief CogWheelManager::error
 * @param socketError
 */
void CogWheelManager::error(QLocalSocket::LocalSocketError socketError)
{
    qDebug() << "Manager socket error" << socketError;
}

/**
 * @brief CogWheelManager::readyRead
 */
void CogWheelManager::readyRead()
{
  qDebug() << "Manager readyRead";
}

/**
 * @brief CogWheelManager::bytesWritten
 * @param bytes
 */
void CogWheelManager::bytesWritten(qint64 bytes)
{
  qDebug() << "Manager bytesWritten" << bytes;
}

/**
 * @brief CogWheelManager::writeCommand
 * @param command
 */
void CogWheelManager::writeCommand(const QString &command)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);
    out << (quint32)0;
    out << command;
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));
    m_managerSocket->write(block);
    m_managerSocket->flush();
}
