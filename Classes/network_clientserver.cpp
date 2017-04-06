#include "network_clientserver.h"

Client::Client(NetworkSimulation * simulation, BaseTransport* transport) :
	_simulation(simulation), _transport(transport), _state(CLIENT_SLEEP)
{
}

Client::~Client()
{
	// Send disconnect packet

}

void Client::Connect(const char * ip, const char * port)
{
	_state = CLIENT_REQUESTING;
	_serverIP = ip;
	_serverPort = port;

	udp::resolver resolver{ *_transport->GetIOService() };
	_serverEndpoint = *resolver.resolve({ udp::v4(), ip, port });
	_active = true;

	// Start
}

void Client::Disconnect()
{
	// Immediately send disconnect packet
	if (_state == CLIENT_CONNECTED)
	{
		ConnectionDisconnectPacket* packet = (ConnectionDisconnectPacket*)CreateDisconnectPacket();
		_transport->SendPacket(_serverEndpoint, packet);
		_transport->WritePackets();
	}
}

std::string Client::GetNetworkState()
{
	switch (_state)
	{
	case CLIENT_SLEEP:
		return "Sleeping";
	case CLIENT_REQUESTING:
		return "Requesting";
	case CLIENT_REQUEST_DENIED:
		return "Denied";
	case CLIENT_CONNECTED:
		return "Connected";
	default:
		return "ERROR";
	}
}

void Client::ProcessPacket(Packet * packet, const udp::endpoint & endpoint)
{
	switch (packet->GetType())
	{
	case CLIENT_SERVER_PACKET_ACCEPTED:
		ProcessAcceptPacket((ConnectionAcceptPacket*)packet, endpoint);
		break;
	case CLIENT_SERVER_PACKET_DENIED:
		ProcessDeniedPacket((ConnectionDeniedPacket*)packet, endpoint);
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

void Client::ProcessAcceptPacket(ConnectionAcceptPacket * packet, const udp::endpoint & endpoint)
{
	//int test = packet->test;
	//test++;

	_connection = new Connection(endpoint, *GetPacketFactory(), GetMessageFactory());
	_state = CLIENT_CONNECTED;

}

void Client::ProcessDeniedPacket(ConnectionDeniedPacket * packet, const udp::endpoint & endpoint)
{
}

void Client::ProcessConnectionPacket(ConnectionPacket * packet, const udp::endpoint & endpoint)
{
	if(_connection)
		_connection->ProcessPacket(packet);
}

void Client::ProcessDisconnectPacket(ConnectionDisconnectPacket * packet, const udp::endpoint & endpoint)
{
}

Packet* Client::CreateRequestPacket()
{
	return GetPacketFactory()->Create(CLIENT_SERVER_PACKET_REQUEST);
}
Packet* Client::CreateDisconnectPacket()
{
	return GetPacketFactory()->Create(CLIENT_SERVER_PACKET_DISCONNECT);
}

Packet * Client::CreateConnectionPacket()
{
	Packet* packet = (Packet*)_connection->GeneratePacket();

	return packet;
}


void Client::SendPackets()
{
	switch (_state)
	{
	case(CLIENT_REQUESTING) :
	{
		ConnectionRequestPacket* packet = (ConnectionRequestPacket*)CreateRequestPacket();
		_transport->SendPacket(_serverEndpoint, packet);
	}
							break;
	case(CLIENT_REQUEST_DENIED) :
	{
		// ERROR
		//_error = CLIENT_ERROR_REQUEST_DENIED; // TODO: receive error code
		//Stop();
	}
								break;
	case(CLIENT_CONNECTED) :
	{
		// Disconnect if timed out
		if (_connection->HasTimedOut())
		{
			Disconnect();
			_state = CLIENT_TIMEDOUT;
		}

		
		Packet* packet = CreateConnectionPacket();
		_transport->SendPacket(_serverEndpoint, packet);
	}
						   break;
	default:
		break;
	}
}

void Client::ReceivePackets()
{
	while (true)
	{
		udp::endpoint endpoint;

		Packet* packet = _transport->ReceivePacket(endpoint);
		if (!packet)
			break;

		// Process
		//int type = packet->GetType();

		ProcessPacket(packet, endpoint);

		// Destroy
	}
}

void Client::WritePackets()
{
	_transport->WritePackets();
}

void Client::ReadPackets()
{
	_transport->ReadPackets();
}



Server::Server(NetworkSimulation * simulation, BaseTransport * transport)
	: _simulation(simulation), _transport(transport)
{
	_state = SERVER_ALIVE;
}

Server::~Server()
{
}

void Server::DisconnectAll()
{
	for (int i = 0; i < MAX_SLOTS; ++i)
	{
		Disconnect(i);
	}
}

void Server::Disconnect(const int ID)
{
	if (!_clientConnected[ID])
		return;

	if (_connections[ID])
	{
		ConnectionDisconnectPacket* packet = (ConnectionDisconnectPacket*)CreateDisconnectPacket();
		_transport->SendPacket(_connections[ID]->Endpoint(), packet);
		_transport->WritePackets();
	}
}

void Server::ProcessPacket(Packet * packet, udp::endpoint endpoint)
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

void Server::ProcessRequestPacket(ConnectionRequestPacket * packet, const udp::endpoint & endpoint)
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
	_connections[clientID] = new Connection(endpoint, *GetPacketFactory(), GetMessageFactory());
	_clientConnected[clientID] = true;

	//_clientData[clientID] = ClientData();
	//_clientData;

	_connectedClients++;

	ConnectionAcceptPacket* acceptPacket = (ConnectionAcceptPacket*)GetPacketFactory()->Create(CLIENT_SERVER_PACKET_ACCEPTED);
	acceptPacket->test = 1337;
	SendPacketToClient(clientID, acceptPacket);
}

void Server::ProcessConnectionPacket(ConnectionPacket * packet, const udp::endpoint & endpoint)
{
	// Check endpoint is a connected client

	// Process in connection layer

	for (int i = 0; i < MAX_SLOTS; i++)
	{
		if (!_clientConnected[i] || !_connections[i])
			continue;

		if (_connections[i]->Endpoint() == endpoint)
		{ // connection found

			_connections[i]->ProcessPacket(packet);

			return;
		}
	}

	// No connection found error


}

void Server::ProcessDisconnectPacket(ConnectionDisconnectPacket * packet, const udp::endpoint & endpoint)
{
	// Chekc ID and endpoint match and disconnect
	for (uint16_t ID = 0; ID < MAX_SLOTS; ID++)
	{
		if (!_clientConnected[ID])
			continue;
		if (_connections[ID]->Endpoint() == endpoint)// Connected
		{
			delete _connections[ID];
			_connections[ID] = nullptr;
			_clientConnected[ID] = false;
			_connectedClients--;
		}
			
	}
}

Packet * Server::CreateDisconnectPacket()
{
	return GetPacketFactory()->Create(CLIENT_SERVER_PACKET_DISCONNECT);
}

void Server::SendPacketToClient(uint16_t clientID, Packet * packet)
{
	_transport->SendPacket(_connections[clientID]->Endpoint(), packet);
}



void Server::SendPackets()
{
	for (int i = 0; i < MAX_SLOTS; i++)
	{
		if (!_clientConnected[i] || !_connections[i])
			continue;

		// Disconnect if timed out
		if (_connections[i]->HasTimedOut())
			Disconnect(i);

		// Generate packet
		Packet* packet = _connections[i]->GeneratePacket();

		// Send packet
		SendPacketToClient(i, packet);
	}

}

void Server::ReceivePackets()
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

void Server::WritePackets()
{
	_transport->WritePackets();
}

void Server::ReadPackets()
{
	_transport->ReadPackets();
}

std::string Server::GetNetworkState()
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