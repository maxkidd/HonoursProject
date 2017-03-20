#include "SnapshotInterpolationClient.h"

SnapshotClient::SnapshotClient(NetworkDebugDataSource* debugData, C_SnapshotInterpolationSimulation* simulation) :
	_state(CLIENT_SLEEP), _transport(new SocketTransport(&_packetFactory, &_messageFactory)), _simulation(simulation)
{
	_transport->SetDebugService(debugData);
}


SnapshotClient::~SnapshotClient()
{
	delete _transport;
	_transport = nullptr;

	_state = CLIENT_SLEEP;
}

void SnapshotClient::Init(const char * ip, const char * port)
{
	_serverIP = ip;
	_serverPort = port;

	udp::resolver resolver{ *_transport->GetIOService() };
	_serverEndpoint = *resolver.resolve({ udp::v4(), ip, port });
}

bool SnapshotClient::Start()
{
	// Update state
	_state = CLIENT_REQUESTING;
	_active = true;


	return true;
}

bool SnapshotClient::Stop()
{
	_state = CLIENT_SLEEP;
	_active = false;
	return true;
}

void SnapshotClient::Reset()
{
}

void SnapshotClient::ProcessSnapshots()
{
	if(_state == CLIENT_CONNECTED)
		_simulation->ProcessSnapshotMessages(_connection);
}

void SnapshotClient::SendPackets()
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
		_error = CLIENT_ERROR_REQUEST_DENIED; // TODO: receive error code
		Stop();
	}
		break;
	case(CLIENT_CONNECTED) :
	{
		Packet* packet = CreateConnectionPacket();
		_transport->SendPacket(_serverEndpoint, packet);
	}
		break;
	default:
		break;
	}
}

void SnapshotClient::ReceivePackets()
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

void SnapshotClient::WritePackets()
{
	_transport->WritePackets();
}

void SnapshotClient::ReadPackets()
{
	_transport->ReadPackets();
}

std::string SnapshotClient::GetNetworkState()
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

void SnapshotClient::ProcessPacket(Packet * packet, const udp::endpoint & endpoint)
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

void SnapshotClient::ProcessAcceptPacket(ConnectionAcceptPacket * packet, const udp::endpoint & endpoint)
{

	_state = CLIENT_CONNECTED;

	//int test = packet->test;
	//test++;

	_connection = new Connection(endpoint, _packetFactory, &_messageFactory);

}

void SnapshotClient::ProcessDeniedPacket(ConnectionDeniedPacket * packet, const udp::endpoint & endpoint)
{
}

void SnapshotClient::ProcessConnectionPacket(ConnectionPacket * packet, const udp::endpoint & endpoint)
{
	_connection->ProcessPacket(packet);
}

void SnapshotClient::ProcessDisconnectPacket(ConnectionDisconnectPacket * packet, const udp::endpoint & endpoint)
{
}

Packet* SnapshotClient::CreateRequestPacket()
{
	return _packetFactory.Create(CLIENT_SERVER_PACKET_REQUEST);
}

Packet * SnapshotClient::CreateConnectionPacket()
{
	Packet* packet = (Packet*)_connection->GeneratePacket();

	return packet;
}