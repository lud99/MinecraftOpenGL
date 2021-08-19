#pragma once

#include <enet/enet.h>
#include <Common/json.hpp>

#include <stdint.h>
#include <string>

struct NetworkMessage;

class NetworkClient
{
public:
	NetworkClient() {};
	NetworkClient(ENetPeer* connection, int id = 0);

	void SendJson(json& message);
	void Send(NetworkMessage* message);
	void Send(void* data, size_t length);
	void SendString(const std::string& data);

	~NetworkClient();

public:
	ENetPeer* m_Connection = nullptr;

	int m_Id = 0;
};

