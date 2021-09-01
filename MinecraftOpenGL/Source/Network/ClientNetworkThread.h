#pragma once

#include <Common/Net/INetworkThread.h>
#include <Common/Net/NetworkClient.h>

class ClientWorld;
class ClientPlayer;

class ClientNetworkThread : public INetworkThread
{
public:
	static ClientNetworkThread& Get();

	bool Connect(const std::string& adress, int port);

	virtual void HandlePacket(json& packet, NetworkClient* me, ENetPeer* peer) override;

	void OnJoinWorld(json& packet, NetworkClient* me, ENetPeer* peer);
	void OnNewPlayerJoined(json& packet, NetworkClient* me, ENetPeer* peer);
	void OnPlayerPosition(json& packet, NetworkClient* me, ENetPeer* peer);
	void OnChunkData(json& packet, NetworkClient* me, ENetPeer* peer);

	ClientPlayer* CreatePlayer(NetworkClient* client);
	NetworkSession& JoinSession(NetworkClient* client, const std::string& sessionName, int clientId = -1);

	NetworkSession* GetThisSession();
	ClientWorld* GetThisWorld();
	ClientPlayer* GetThisPlayer();

private:
	ClientNetworkThread();

public:
	NetworkClient* m_Me = nullptr;
};

typedef ClientNetworkThread NetworkThread;