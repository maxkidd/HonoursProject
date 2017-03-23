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
		;//_simulation->ProcessSnapshotMessages(_connection);
}

void StateSyncClient::GenerateMessages()
{
}


