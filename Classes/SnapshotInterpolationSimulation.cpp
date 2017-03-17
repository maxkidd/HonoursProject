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
		case SNAPSHOT_MESSAGE_MOVE_BOX:
			// Process move
			SnapshotBoxMove* move = (SnapshotBoxMove*)message;
			break;
		}
	}

	return true;
}

void SnapshotInterpolationSimulation::GenerateSnapshotMessages(MessageFactory* mf, Connection * con)
{
	for (int i = 0; i < 2; i++)
	{
		SnapshotBoxCreate* create = (SnapshotBoxCreate*)mf->Create(SNAPSHOT_MESSAGE_CREATE_BOX);
		create->id = 150;
		create->y = 150.0f;
		create->x = 150.0f;
		con->SendMsg(create);
		SnapshotBoxMove* move = (SnapshotBoxMove*)mf->Create(SNAPSHOT_MESSAGE_MOVE_BOX);
		move->id = 250;
		move->x = 250.0f;
		move->y = 250.0f;
		con->SendMsg(move);
	}
}
