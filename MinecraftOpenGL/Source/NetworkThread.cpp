#include "NetworkThread.h"

#include <iostream>

#include <enet/enet.h>
#include <Common/NetworkConstants.h>

NetworkThread::NetworkThread()
{
}

NetworkThread& NetworkThread::Instance()
{
	static NetworkThread instance;
	return instance;
}

bool NetworkThread::Connect(const std::string& adress, int port)
{
	m_Adress = adress;
	m_Port = port;

	bool success = false;
	m_Thread = std::thread(&NetworkThread::_ConnectThread, this, &success);
	m_Thread.join();

	return success;
}

void NetworkThread::SendJson(json& message)
{
	std::string stringified = message.dump();

	SendString(stringified);
}

void NetworkThread::Send(const char* data, size_t length)
{
	
}

void NetworkThread::SendString(const std::string& data)
{
	std::cout << "Sending message: " << data << "\n";

	if (m_Thread.joinable())
		m_Thread.join();

	m_Thread = std::thread([this, data]() {
		ENetPacket* packet = enet_packet_create(data.c_str(), data.length() + 1, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(m_Connection, 0, packet);
	});
}

bool NetworkThread::_ConnectThread(bool* success)
{
	enet_initialize();

	m_Client = enet_host_create(NULL, MaxClients, Channels, MaxInBandwidth, MaxOutBandwidth);

	if (m_Client == NULL) std::cout << "An error occurred while trying to create an ENet client host.\n";

	ENetAddress address;
	ENetEvent event;
	ENetPeer* peer = nullptr;

	enet_address_set_host(&address, m_Adress.c_str());
	address.port = m_Port;

	peer = enet_host_connect(m_Client, &address, Channels, 0 /* to send at connect */);
	if (peer == NULL)
	{
		std::cout << "No available peers for initiating an ENet connection.\n";
		*success = false;
		return false;
	}

	NetworkClient* connection = new NetworkClient(peer);

	/* Wait up to 5 seconds for the connection attempt to succeed. */
	if (enet_host_service(m_Client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		std::cout << "Connection to " << m_Adress << " succeeded \n";

		m_Connection = peer;

		*success = true;
		return true;
	}
	else
	{
		/* Either the 5 seconds are up or a disconnect event was */
		/* received. Reset the peer in the event the 5 seconds   */
		/* had run out without any significant event.            */
		enet_peer_reset(peer);
		std::cout << "Connection to " << m_Adress << " failed \n";

		*success = false;
		return false;
	}

	*success = true;
}

void NetworkThread::_SendThread(const char* data, size_t length)
{
	ENetPacket* packet = enet_packet_create(data, length, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(m_Connection, 0, packet);
}

ENetPacket* NetworkThread::PullPackets()
{
	ENetEvent event;

	// The incoming messages are handled by this function. Needs to be called regularly. A 0 timeout means that it's non-blocking
	int eventStatus = enet_host_service(m_Client, &event, 0);

	if (eventStatus > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
		{
			printf("(Client) Message from server : %s\n", event.packet->data);

			return event.packet;

			// Lets broadcast this message to all
			// enet_host_broadcast(client, 0, event.packet);
			// temp: enet_packet_destroy(event.packet);
			break;
		}

		case ENET_EVENT_TYPE_DISCONNECT:
			printf("(Client) %s disconnected.\n", event.peer->data);
			// Reset client's information
			event.peer->data = NULL;
			break;
		}
	}

	// Send queded packets?
	enet_host_flush(m_Client);

	return nullptr;
}

NetworkThread::~NetworkThread()
{
	enet_deinitialize();
	if (m_Thread.joinable())
		m_Thread.join();
}