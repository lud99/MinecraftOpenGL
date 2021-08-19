#include "NetworkClient.h"

#include <iostream>

#include <Common/NetworkMessages.h>

NetworkClient::NetworkClient(ENetPeer* connection, int id)
{
	m_Connection = connection;
	m_Id = id;
}

void NetworkClient::SendJson(json& message)
{
	std::string stringified = message.dump();

	SendString(stringified);
}

void NetworkClient::Send(NetworkMessage* message)
{
	Send(message->Pack().data, message->Pack().length);
}

void NetworkClient::Send(void* data, size_t length)
{
	ENetPacket* packet = enet_packet_create(data, length, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(m_Connection, 0, packet);
}

void NetworkClient::SendString(const std::string& data)
{
	std::cout << "Sending message: " << data << "\n";

	ENetPacket* packet = enet_packet_create(data.c_str(), data.length() + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(m_Connection, 0, packet);

	//Send((void*)data.c_str(), data.length() + 1);
}

NetworkClient::~NetworkClient()
{

}