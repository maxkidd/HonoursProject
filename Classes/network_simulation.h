#ifndef _NETWORK_SIMULATION_H_
#define _NETWORK_SIMULATION_H_

#include <cocos2d.h>
#include <Box2D\Box2D.h>


class Connection;
class MessageFactory;

static float CubicHermite(float p0, float p1, float p2, float p3, float t)
{
	float a = -p0 / 2.0f + (3.0f*p1) / 2.0f - (3.0f*p2) / 2.0f + p3 / 2.0f;
	float b = p0 - (5.0f*p1) / 2.0f + 2.0f*p2 - p3 / 2.0f;
	float c = -p0 / 2.0f + p2 / 2.0f;


	return a*t*t*t + b*t*t + c*t + p3;
}

static float Cubic(float p0, float p1, float p2, float p3, float t)
{
	//float t2 = t*t;
	float a = p3 - p2 - p0 + p1;
	float b = p0 - p1 - a;
	float c = p2 - p0;
	//float d = p1;


	return a*t*t*t + b*t*t + c*t + p1;
}
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
	virtual ~NetworkSimulation() {};
	virtual bool ProcessMessages(Connection* con) = 0;
	virtual void GenerateMessages(MessageFactory* mf, Connection* con) = 0;
};

#endif