#include "SnapshotInterpolationSimulation.h"

SnapshotInterpolationSimulation::SnapshotInterpolationSimulation() : _world{b2Vec2(0.0f, 5.0f)}
{

}

bool SnapshotInterpolationSimulation::ProcessSnapshotMessages(Connection * con)
{
	NMessage* message;
	while (message = con->ReceiveMsg()) 
	{
		switch (message->GetType())
		{
		case SNAPSHOT_MESSAGE_CREATE_BOX:
			//Process creation
			break;
		case SNAPSHOT_MESSAGE_UPDATE_BOX:
			// Process move
			SnapshotBoxMove* move = (SnapshotBoxMove*)message;
			break;
		}
	}

	return true;
}

void SnapshotInterpolationSimulation::GenerateSnapshotMessages(MessageFactory* mf, Connection * con)
{
	SnapshotBoxMove* move = (SnapshotBoxMove*)mf->Create(SNAPSHOT_MESSAGE_UPDATE_BOX);
	move->id = 250;
	move->x = 0.255f;
	move->y = 255.555f;

	con->SendMsg(move);
}
