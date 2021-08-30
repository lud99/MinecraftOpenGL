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

	virtual void HandlePacket(json& packet, NetworkClient* me) override;

	void OnJoinWorld(json& packet, NetworkClient* me);
	void OnPlayerPosition(json& packet, NetworkClient* me);
	void OnChunkData(json& packet, NetworkClient* me);

	NetworkSession* GetThisSession();
	ClientWorld* GetThisWorld();
	ClientPlayer* GetThisPlayer();

private:
	ClientNetworkThread();

public:
	NetworkClient* m_ThisClient = nullptr;
};

typedef ClientNetworkThread NetworkThread;