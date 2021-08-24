#pragma once

#include <Common/Net/INetworkThread.h>
#include <Common/Net/NetworkClient.h>

class ServerWorld;

class ServerNetworkThread : public INetworkThread
{
public:
	static ServerNetworkThread& Get();

	bool StartServer(int port);

	virtual void HandlePacket(json& packet, NetworkClient* client) override;

	void OnClientJoinWorld(json& packet, NetworkClient* client);
	//void OnChunkData(json& packet, NetworkConnection* conn);

private:
	ServerNetworkThread();

public:
};

typedef ServerNetworkThread NetworkThread;