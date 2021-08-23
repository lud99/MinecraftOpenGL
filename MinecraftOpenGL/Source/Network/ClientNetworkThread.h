#pragma once

#include <Common/INetworkThread.h>
#include <Common/NetworkConnection.h>

class ClientWorld;

class ClientNetworkThread : public INetworkThread
{
public:
	static ClientNetworkThread& Instance();

	ClientNetworkThread() {};

	bool Connect(const std::string& adress, int port);

	virtual void HandlePacket(json& packet, NetworkConnection* conn) override;

	void OnJoinWorld(json& packet, NetworkConnection* conn);
	void OnChunkData(json& packet, NetworkConnection* conn);

public:
	NetworkConnection* m_Me = nullptr;;
	ENetPeer* m_Server = nullptr;;
	ClientWorld* m_LocalWorld = nullptr;
};

typedef ClientNetworkThread NetworkThread;