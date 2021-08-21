#pragma once

#include <thread>

#include <Common/json.hpp>

#include <Common/NetworkClient.h>

class NetworkThread
{
public:
	static NetworkThread& Instance();

	bool Connect(const std::string& adress, int port);

	void SendJson(json& message);
	void SendString(const std::string& data);

	ENetPacket* PullPackets();

	void HandleJoinWorldPacket(json& packet);
	void HandleChunkDataPacket(json& packet);

	~NetworkThread();

private:
	NetworkThread();

private:
	bool _ConnectThread(bool* success);
	void _SendThread(const char* data, size_t length);

public:
	ENetHost* m_Client;
	ENetPeer* m_Connection;

	int m_ClientId;

	std::string m_Adress;
	int m_Port;

	std::vector<std::string> m_SendQueue;

	std::thread m_Thread;

	bool m_ShouldExit = false;
};

