#include "NetworkMessages.h"

#include <string.h>
#include <iostream>

#include <cstdint>

namespace NetworkMessages
{
	PackedMessage JoinSession::Pack()
	{
		int messageSize = sizeof(uint8_t) + m_SessionName.length() + sizeof('\0');

		char* data = new char[messageSize];
		data[0] = m_MessageType;

		for (int i = 0; i < m_SessionName.length(); i++)
			data[i + 1] = (char)m_SessionName[i];

		data[messageSize - 1] = '\0';

		PackedMessage msg = { data, messageSize };

		return msg;
	}

	JoinSession::UnpackedMessage JoinSession::Unpack(char* data)
	{
		uint8_t messageType = data[0];

		std::string sessionName;
		int i = 0;
		while (true)
		{
			char ch = data[i + 1];

			sessionName += ch;

			if (ch == '\0')
				break;

			i++;
		}

		UnpackedMessage msg;
		msg.m_MessageType = (MessageType)messageType;
		msg.m_SessionName = sessionName;

		return msg;
	}
}