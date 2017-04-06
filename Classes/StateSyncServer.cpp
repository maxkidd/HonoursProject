#include "StateSyncServer.h"

StateSyncServer::StateSyncServer(NetworkSimulation* simulation, BaseTransport* transport)
	: Server(simulation, transport)
{


}

StateSyncServer::~StateSyncServer()
{


}


void StateSyncServer::ProcessMessages()
{
	for (int i = 0; i < MAX_SLOTS; i++)
	{
		if (!_clientConnected[i] || !_connections[i])
			continue;

		_simulation->ProcessMessages(_connections[i]);

	}
}
void StateSyncServer::GenerateMessages()
{
	for (int i = 0; i < MAX_SLOTS; i++)
	{
		if (!_clientConnected[i] || !_connections[i])
			continue;

		_simulation->GenerateMessages(&_messageFactory, _connections[i]);

	}
}