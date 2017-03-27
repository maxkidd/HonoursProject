#include "StateSyncClient.h"

StateSyncClient::StateSyncClient(NetworkSimulation* simulation, BaseTransport* transport)
: Client(simulation, transport)
{
}


StateSyncClient::~StateSyncClient()
{
}

void StateSyncClient::ProcessMessages()
{
	if (_state == CLIENT_CONNECTED)
		_simulation->ProcessMessages(_connection);
}

void StateSyncClient::GenerateMessages()
{
	if (_state == CLIENT_CONNECTED)
		_simulation->GenerateMessages(&_messageFactory, _connection);
}


