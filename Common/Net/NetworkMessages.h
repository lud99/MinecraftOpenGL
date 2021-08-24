#pragma once

#include <string>
#include <tuple>

enum MessageType {
	JoinWorld = 0,
	LeaveWorld
};

struct PackedMessage
{
	char* data;
	int length;
};

struct NetworkMessage
{
	MessageType m_MessageType;

	virtual PackedMessage Pack() = 0;
	//virtual std::string ToString() = 0;
};

namespace NetworkMessages
{
	struct JoinSession : public NetworkMessage
	{
		struct UnpackedMessage
		{
			MessageType m_MessageType;
			std::string m_SessionName;
		};

		JoinSession() {};

		MessageType m_MessageType = MessageType::JoinWorld;
		std::string m_SessionName;

		PackedMessage Pack() override;
		
		static UnpackedMessage Unpack(char* data);
	};

	struct LeaveSession : public NetworkMessage
	{
		struct UnpackedMessage
		{
			MessageType m_MessageType;
			std::string m_SessionName;
		};

		LeaveSession() {};

		MessageType m_MessageType = MessageType::JoinWorld;
		std::string m_SessionName;

		PackedMessage Pack() override;
		UnpackedMessage Unpack();
	};
}