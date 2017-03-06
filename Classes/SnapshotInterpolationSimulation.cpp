#include "SnapshotInterpolationSimulation.h"

SnapshotInterpolationSimulation::SnapshotInterpolationSimulation() : _world{b2Vec2(0.0f, 5.0f)}
{

}

bool SnapshotInterpolationSimulation::ProcessSnapshotMessage()
{
	return false;
}

void SnapshotInterpolationSimulation::GenerateSnapshotMessages(MessageFactory* mf, Connection * con)
{
	SnapshotBoxMove* move = (SnapshotBoxMove*)mf->Create(SNAPSHOT_MESSAGE_UPDATE_BOX);
	move->id = 250;
	move->x = 0.255f;
	move->y = 255.555f;

	con->SendMsg(move);
}
