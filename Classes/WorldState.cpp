#include "WorldState.h"

WorldState::WorldState(b2World * world):
	m_world(world)
{


	b2Body* b = world->GetBodyList();
	
	while (b)
	{
		BodyState state;
		state.angle = b->GetAngle();
		state.position = b->GetPosition();
		state.angularVelocity = b->GetAngularVelocity();
		state.linearVelocity = b->GetLinearVelocity();

		m_bodyStates.push_back(state);
		b = b->GetNext();
	}
}

WorldState::~WorldState()
{
}

void WorldState::LoadState()
{
	b2Body* b = m_world->GetBodyList();

	int i = 0;
	while (b)
	{
		b2Body* next = b->GetNext();
		if (i >= m_bodyStates.size())
		{
			m_world->DestroyBody(b);
		}
		else
		{
			BodyState state = m_bodyStates.at(i++);
			b->SetTransform(state.position, state.angle);
			b->SetAngularVelocity(state.angularVelocity);
			b->SetLinearVelocity(state.linearVelocity);
		}

		b = next;
	}
}
