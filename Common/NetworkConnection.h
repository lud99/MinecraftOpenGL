#pragma once

#include <enet/enet.h>
#include <Common/json.hpp>

#include <stdint.h>
#include <string>

class NetworkConnection
{
public:
	NetworkConnection();
	NetworkConnection(ENetPeer* conn, int id = 0);

	void SendJson(json& message);
	void SendString(const std::string& data);

	~NetworkConnection();

public:
	ENetPeer* m_Conn = nullptr;

	int m_Id = 0;
};

