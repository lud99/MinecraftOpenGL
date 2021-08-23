#pragma once

#include <thread>
#include <mutex>
#include <unordered_map>

#include <Common/json.hpp>

#include <Common/NetworkConnection.h>
#include <Common/NetworkSession.h>

struct NetworkPacket
{
	std::string data;
	NetworkConnection* connection;

	NetworkPacket(std::string _data, NetworkConnection* _connection) : data(_data), connection(connection) {};
};

class INetworkThread
{
public:
	INetworkThread();

	bool StartServer(int port);

	void SendJson(json& message, NetworkConnection* conn);
	void SendString(const std::string& data, NetworkConnection* conn);

	ENetPacket* PullPackets();
	virtual void HandlePacket(json& packet, NetworkConnection* conn) = 0;

	// Server functions
	void Server_HandleJoinWorld(json& message, ENetEvent& event);
	void Server_HandleGetChunk(json& message, ENetEvent& event);

	void SetupThread();

	~INetworkThread();

public:
	ENetHost* m_Client = nullptr;
	NetworkConnection* m_Conn = nullptr;

	int m_ClientId;

	std::string m_Adress;
	int m_Port;

	int m_SendingOnChannel = 0;

	std::unordered_map<std::string, NetworkSession> m_Sessions;

	std::vector<NetworkPacket> m_SendQueue;
	std::mutex m_QueueLock;

	std::thread m_Thread;

	bool m_ShouldExit = false;
};

