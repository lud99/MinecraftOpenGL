#include "NetworkConnection.h"

#include <Common/INetworkThread.h>

#include <iostream>

#include <Common/NetworkMessages.h>

#ifdef SERVER_BUILD
#include "../../Network/ServerNetworkThread.h"
#else
#include "../../Network/ClientNetworkThread.h"
#endif

NetworkConnection::NetworkConnection()
{
}

NetworkConnection::NetworkConnection(ENetPeer* conn, int id)
{
	m_Conn = conn;
	m_Id = id;
}

void NetworkConnection::SendJson(json& message)
{
	NetworkThread& net = net.Instance();
	net.SendJson(message, this);
}

void NetworkConnection::SendString(const std::string& data)
{
	NetworkThread& net = net.Instance();
	net.SendString(data, this);
}

NetworkConnection::~NetworkConnection()
{

}