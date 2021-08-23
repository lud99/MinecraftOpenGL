#pragma once

#include <enet/enet.h>

#include <unordered_map>
#include <stdint.h>
#include <string>

class NetworkConnection;

class NetworkSession
{
public:
	NetworkSession() {};
	NetworkSession(std::string sessionId);

	NetworkConnection* CreateClient(ENetPeer* connection, int id);
	NetworkConnection* GetClient(int id);

	void AddClient(NetworkConnection* client);
	void RemoveClient(NetworkConnection* client);

	~NetworkSession();

public:
	std::string m_SessionId;
	std::unordered_map<int, NetworkConnection*> m_Clients;
};