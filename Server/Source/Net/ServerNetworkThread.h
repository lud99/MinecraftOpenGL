#pragma once

#include <Common/Net/INetworkThread.h>
#include <Common/Net/NetworkClient.h>

class ServerWorld;

class ServerNetworkThread : public INetworkThread
{
public:
	static ServerNetworkThread& Get();

	bool StartServer(int port);

	virtual void HandlePacket(json& packet, NetworkClient* client, ENetPeer* peer) override;

	void OnClientJoinWorld(json& packet, NetworkClient* client);
	void OnClientConfirmJoinWorld(json& packet, NetworkClient* client);
	void OnClientPositionUpdate(json& packet, NetworkClient* client);
	void OnChunkDataChanged(json& packet, NetworkClient* client);

private:
	ServerNetworkThread();

public:
};

typedef ServerNetworkThread NetworkThread;