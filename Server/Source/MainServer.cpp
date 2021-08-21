#define SERVER_BUILD
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <enet/enet.h>
#include <iostream>

#include <Common/json.hpp>

#include <Common/NetworkConstants.h>
#include <Common/NetworkMessages.h>

#include "NetworkSession.h"
#include <Common/NetworkClient.h>

#include "Temp/World/Chunk/Chunk.h"
#include "Temp/World/Chunk/ChunkBlock.h"
#include "Temp/World/Player/Player.h"

int main(int argc, char** argv)
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);

	ENetEvent event;
	ENetAddress address;
	ENetHost* server;

	address.host = ENET_HOST_ANY; // This allows connections from anywhere
	address.port = Port;

	server = enet_host_create(&address, MaxClients, Channels, MaxInBandwidth, MaxOutBandwidth);

	if (server == NULL) printf("An error occurred while trying to create an ENet server host.");

	std::unordered_map<std::string, NetworkSession> sessions;

	// gameloop
	while (true)
	{
		ENetEvent event;

		World::Update();

		while (enet_host_service(server, &event, 1000) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT: {
				printf("A new client connected from %x:%u.\n",
					event.peer->address.host,
					event.peer->address.port);

				break;
			}
			case ENET_EVENT_TYPE_RECEIVE: {
				/*printf("A packet of length %u containing %s was received from %s on channel %u.\n",
					event.packet->dataLength,
					event.packet->data,
					event.peer->data,
					event.channelID);*/

				NetworkClient* client = (NetworkClient*)event.peer->data;
				const char* msgStrData = (char*)event.packet->data;

				// Parse the json message
				json incomingMessage = json::parse(msgStrData);
				std::string messageType = incomingMessage["Type"];

				std::cout << "Recieved message " << incomingMessage["Type"] << "\n";

				// Handle all types of messages
				if (messageType == "JoinWorld")
				{
					std::string sessionName = incomingMessage["Data"]["SessionName"];

					// The data is null if it is the first time the client connected
					if (client == nullptr)
					{
						int clientId = rand() % 65535;

						client = new NetworkClient(event.peer, clientId);
						event.peer->data = client; // Store the client for later messages
					}

					// Create the session if it doesn't exist
					if (sessions.count(sessionName) == 0)
					{
						sessions[sessionName] = NetworkSession(sessionName);
						std::cout << "Created session " << sessionName << "\n";
					}

					// gen
					Chunk chunk(glm::ivec2(0, 0));
					chunk.Init();
					chunk.GenerateTerrain();

					chunk.GetBlockAt(glm::vec3(0, 30, 0))->GetBlockType()->id;

					json worldData;
					worldData["Type"] = "WorldData";
					worldData["Data"] = json::array();
					for (int x = 0; x < Chunk::Width; x++)
					{
						for (int z = 0; z < Chunk::Depth; z++)
						{
							worldData["Data"][x + z] = chunk.GetBlockAt(glm::vec3(x, 0, z))->m_BlockId;
						}
					}

					client->SendJson(worldData);

					std::cout << worldData.dump() << "\n";

					NetworkSession& session = sessions[sessionName];

					session.AddClient(client);

					std::cout << "Client " << client->m_Id << " joined session " << sessionName << "\n";
				}

				if (messageType == "PlayerPosition")
				{
					glm::vec3 pos;
					pos.x = incomingMessage["Data"]["X"];
					pos.y = incomingMessage["Data"]["Y"];
					pos.z = incomingMessage["Data"]["Z"];

					World::m_Player.m_Position = pos;
				}

				if (messageType == "GetChunk")
				{
					glm::ivec2 chunkPos(incomingMessage["Data"]["Position"]["X"], incomingMessage["Data"]["Position"]["Z"]);

					json worldData;
					worldData["Type"] = "ChunkData";
					worldData["Data"]["Position"]["X"] = incomingMessage["Data"]["Position"]["X"];
					worldData["Data"]["Position"]["Z"] = incomingMessage["Data"]["Position"]["Z"];

					worldData["Data"]["Blocks"] = json::array();
					for (int x = 0; x < Chunk::Width; x++)
					{
						for (int z = 0; z < Chunk::Depth; z++)
						{
							worldData["Data"]["Blocks"][x + z] = World::GetChunkAt(chunkPos)->GetBlockAt(glm::vec3(x, 0, z))->m_BlockId;
						}
					}

					client->SendJson(worldData);
				}

				enet_packet_destroy(event.packet);
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
				printf("%s disconnected.\n", event.peer->data);
				/* Reset the peer's client information. */
				event.peer->data = NULL;
			}
		}
	}

	enet_host_destroy(server);

	return 0;
}
