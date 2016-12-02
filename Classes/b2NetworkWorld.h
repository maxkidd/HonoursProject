#ifndef _B2NETWORKWORLD_H_
#define _B2NETWORKWORLD_H_

#include "Box2D\Box2D.h"

class b2NetworkWorld : public b2World
{
public:
	b2NetworkWorld(const b2Vec2& gravity);
	~b2NetworkWorld();

	b2Body* CreateSynchronisedBody(const b2BodyDef* def);

	void DestroySynchronisedBody(b2Body* body);


	b2Body* CreateBody(const b2BodyDef* def);

	void DestroyBody(b2Body* body);

private:

};

#endif