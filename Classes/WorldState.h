#ifndef _WORLDSTATE_H_
#define _WORLDSTATE_H_

#include <vector>

#include "Box2D\Box2D.h"

struct BodyState
{
	/// The world position of the body. Avoid creating bodies at the origin
	/// since this can lead to many overlapping shapes.
	b2Vec2 position;

	/// The world angle of the body in radians.
	float32 angle;

	/// The linear velocity of the body's origin in world co-ordinates.
	b2Vec2 linearVelocity;

	/// The angular velocity of the body.
	float32 angularVelocity;
};

class WorldState
{
public:
	WorldState(b2World* world);
	~WorldState();
	
	void LoadState();
private:
	std::vector<BodyState> m_bodyStates;
	b2World* m_world;
};

#endif
