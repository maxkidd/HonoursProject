#include "SnapshotInterpolationServer.h"

SnapshotServer::SnapshotServer(NetworkSimulation* simulation, BaseTransport* transport)
	: Server(simulation, transport)
{


}

SnapshotServer::~SnapshotServer()
{


}


void SnapshotServer::ProcessMessages()
{
	for (int i = 0; i < MAX_SLOTS; i++)
	{
		if (!_clientConnected[i] || !_connections[i])
			continue;

		_simulation->ProcessMessages(_connections[i]);
	}
}
void SnapshotServer::GenerateMessages()
{
	for (int i = 0; i < MAX_SLOTS; i++)
	{
		if (!_clientConnected[i] || !_connections[i])
			continue;

		_simulation->GenerateMessages(&_messageFactory, _connections[i]);

	}
}