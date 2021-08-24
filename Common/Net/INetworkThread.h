#pragma once

#include <thread>
#include <mutex>
#include <unordered_map>

#include <Common/json.hpp>

#include "NetworkClient.h"
#include "NetworkSession.h"

struct NetworkPacket
{
	std::string data;
	NetworkClient* client = nullptr;

	NetworkPacket(std::string _data, NetworkClient* _client) : data(_data), client(_client) {};
};

class INetworkThread
{
public:
	void SendJson(json& message, NetworkClient* conn);
	void SendString(const std::string& data, NetworkClient* conn);

	ENetPacket* PullPackets();
	virtual void HandlePacket(json& packet, NetworkClient* conn) = 0;

	void SetupThread();

	~INetworkThread();

public:
	INetworkThread();

public:
	ENetHost* m_EnetClient = nullptr;

	std::string m_Adress;
	int m_Port;

	int m_SendingOnChannel = 0;

	std::unordered_map<std::string, NetworkSession> m_Sessions;

	std::vector<NetworkPacket> m_SendQueue;
	std::mutex m_QueueLock;

	std::thread m_Thread;

	bool m_ShouldExit = false;
};

