#include "b2NetworkWorld.h"

b2NetworkWorld::b2NetworkWorld(const b2Vec2& gravity) : b2World(gravity)
{
}

b2NetworkWorld::~b2NetworkWorld()
{
}

b2Body * b2NetworkWorld::CreateSynchronisedBody(const b2BodyDef* def)
{
	b2Body* b = CreateBody(def);



	return b;
}

void b2NetworkWorld::DestroySynchronisedBody(b2Body * body)
{
	DestroyBody(body);
}

b2Body * b2NetworkWorld::CreateBody(const b2BodyDef * def)
{
	b2Body* b = CreateBody(def);



	return b;
}

void b2NetworkWorld::DestroyBody(b2Body * body)
{
	DestroyBody(body);
}
