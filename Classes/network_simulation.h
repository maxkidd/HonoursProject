#ifndef _NETWORK_SIMULATION_H_
#define _NETWORK_SIMULATION_H_

#include <cocos2d.h>
#include <Box2D\Box2D.h>


class Connection;
class MessageFactory;

static b2Vec2 Lerp(b2Vec2 pos1, b2Vec2 pos2, float t)
{
	// Imprecise method which doesn't guarantee position = pos2 when t = 0
	return b2Vec2(pos1.x + t * (pos2.x - pos1.x), pos1.y + t * (pos2.y - pos1.y));
}
static float Lerp(float a, float b, float t)
{
	// Imprecise method which doesn't guarantee position = pos2 when t = 0
	return a + t * (b - a);
}
static float LerpRad(float a, float b, float t)
{
	// Imprecise method which doesn't guarantee position = pos2 when t = 0
	float difference = abs(b - a);
	const float pi = 3.1415f;
	if (difference > pi)
	{
		if (b > a)
			a += 2 * pi;
		else
			b += 2 * pi;
	}
	return a + t * (b - a);
}

class NetworkSimulation : public cocos2d::Layer
{
public:
	virtual bool ProcessMessages(Connection* con) = 0;
	virtual void GenerateMessages(MessageFactory* mf, Connection* con) = 0;
};

#endif