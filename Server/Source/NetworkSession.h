#pragma once

#include <enet/enet.h>

#include <unordered_map>
#include <stdint.h>
#include <string>

class NetworkClient;

class NetworkSession
{
public:
	NetworkSession() {};
	NetworkSession(std::string sessionId);

	NetworkClient* CreateClient(ENetPeer* connection, int id);
	NetworkClient* GetClient(int id);

	void AddClient(NetworkClient* client);
	void RemoveClient(NetworkClient* client);

	~NetworkSession();

public:
	std::string m_SessionId;
	std::unordered_map<int, NetworkClient*> m_Clients;
};