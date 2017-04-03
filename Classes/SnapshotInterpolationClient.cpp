#include "SnapshotInterpolationClient.h"

SnapshotClient::SnapshotClient(NetworkSimulation* simulation, BaseTransport* transport)
	: Client(simulation, transport)
{
}


SnapshotClient::~SnapshotClient()
{
}

void SnapshotClient::ProcessMessages()
{
	if (_state == CLIENT_CONNECTED)
		_simulation->ProcessMessages(_connection);
}

void SnapshotClient::GenerateMessages()
{
	if (_state == CLIENT_CONNECTED)
		_simulation->GenerateMessages(&_messageFactory, _connection);
}


