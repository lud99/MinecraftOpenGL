#pragma once

#include <enet/enet.h>
#include <Common/json.hpp>

#include <stdint.h>
#include <string>

class NetworkClient
{
public:
	NetworkClient();
	NetworkClient(ENetPeer* peer, int id = -1);

	void SendJson(json& message);
	void SendString(const std::string& data);

	~NetworkClient();

public:
	ENetPeer* m_Peer = nullptr;

	std::string m_SessionName = "";

	int m_Id = -1;

	bool m_HasJoinedSession = false;
};

