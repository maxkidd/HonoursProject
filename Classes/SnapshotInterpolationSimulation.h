#ifndef _SNAPSHOTINTERPOLATIONSIMULATION_H_
#define _SNAPSHOTINTERPOLATIONSIMULATION_H_


#include <stdint.h>
#include <vector>
#include <Box2D\Box2D.h>

#include "network_common.h"
#include "network_connection.h"

struct WorldSnapshot
{
	std::vector<std::pair<uint32_t, b2Vec2>> boxes;
};

class SnapshotInterpolationSimulation
{
private:
	b2World _world;
	std::deque<WorldSnapshot> snapshots;
public:
	SnapshotInterpolationSimulation();

	bool ProcessSnapshotMessage();
	void GenerateSnapshotMessages(MessageFactory* mf, Connection* con);

	uint32_t CreateBox();
	bool MoveBox(uint32_t id, b2Vec2 pos);
};


#endif