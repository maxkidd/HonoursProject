#include "SnapshotInterpolationServer.h"

SnapshotServer::SnapshotServer(NetworkDebugDataSource* _debugData) : _transport(new SocketTransport(&_packetFactory, 1500))
{
	_transport->SetDebugService(_debugData);
}

SnapshotServer::~SnapshotServer()
{
	delete _transport;
	_transport = nullptr;

}

bool SnapshotServer::Start()
{
	_active = true;
	_state = SERVER_ALIVE;

	return true;
}

bool SnapshotServer::Stop()
{
	// Shutdown process
	//_state = SERVER_SLEEP
	// Disconnect all clients safely

	_active = false;

	return true;
}

void SnapshotServer::Reset()
{
}

void SnapshotServer::SendPackets()
{
	for (int i = 0; i < MAX_SLOTS; i++)
	{
		if (!_clientConnected[i] || !_connections[i])
			continue;

		Packet* packet = _connections[i]->GeneratePacket();

		SendPacketToClient(i, packet);
	}

}

void SnapshotServer::ReceivePackets()
{
	while (true)
	{
		udp::endpoint endpoint;

		Packet* packet = _transport->ReceivePacket(endpoint);
		if (!packet)
			break;

		// Process
		int type = packet->GetType();

		ProcessPacket(packet, endpoint);

		// Destroy
	}

}

void SnapshotServer::WritePackets()
{
	_transport->WritePackets();
}

void SnapshotServer::ReadPackets()
{
	_transport->ReadPackets();
}

std::string SnapshotServer::GetNetworkState()
{
	switch (_state)
	{
	case SERVER_SLEEP:
		return "Sleeping";
	case SERVER_ALIVE:
		return "Alive - No Clients";
	case SERVER_CONNECTED:
		return "Connected - Clients: " + _connectedClients;
	default:
		return "ERROR";
	}
}

void SnapshotServer::ProcessPacket(Packet * packet, udp::endpoint endpoint)
{
	switch (packet->GetType())
	{
	case CLIENT_SERVER_PACKET_REQUEST:
		ProcessRequestPacket((ConnectionRequestPacket*)packet, endpoint);
		break;
	case CLIENT_SERVER_PACKET_CONNECTION:
		ProcessConnectionPacket((ConnectionPacket*)packet, endpoint);
		break;
	case CLIENT_SERVER_PACKET_DISCONNECT:
		ProcessDisconnectPacket((ConnectionDisconnectPacket*)packet, endpoint);
		break;
	default:
		break;
	}
}

void SnapshotServer::ProcessRequestPacket(ConnectionRequestPacket * packet, const udp::endpoint & endpoint)
{

	// Case: no slots open
	// Case: alraedy connected
	uint16_t clientID = NULL_CLIENT_ID;
	for (uint16_t ID = 0; ID < MAX_SLOTS; ID++)
	{
		if (_clientConnected[ID])
		{
			if (_connections[ID]->Endpoint() == endpoint)
				return;// Client already connected
		}
		else
		{
			if (clientID == NULL_CLIENT_ID)
				clientID = ID;
		}
	}

	if (clientID == NULL_CLIENT_ID)
		return;// No slots

			   /* Connect client */

			   //_connections.push_back(Connection(endpoint));
	_connections[clientID] = new Connection(endpoint);
	_clientConnected[clientID] = true;

	_clientData[clientID] = ClientData();
	//_clientData;

	_connectedClients++;

	ConnectionAcceptPacket* acceptPacket = (ConnectionAcceptPacket*)_packetFactory.Create(CLIENT_SERVER_PACKET_ACCEPTED);
	acceptPacket->test = 1337;
	SendPacketToClient(clientID, acceptPacket);
}

void SnapshotServer::ProcessConnectionPacket(ConnectionPacket * packet, const udp::endpoint & endpoint)
{
	// Check endpoint is a connected client

	// Process in connection layer
}

void SnapshotServer::ProcessDisconnectPacket(ConnectionDisconnectPacket * packet, const udp::endpoint & endpoint)
{
	// Find clientID for endpoint

	// Remove client from server
}

void SnapshotServer::SendPacketToClient(uint16_t clientID, Packet * packet)
{
	_transport->SendPacket(_connections[clientID]->Endpoint(), packet);
}
