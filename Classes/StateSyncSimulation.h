#ifndef _STATESYNCSIMULATION_H_
#define _STATESYNCSIMULATION_H_


//#include <stdint.h>
#include <vector>
#include <cocos2d.h>
#include <Box2D\Box2D.h>
#include "Box2DTestBed\GLES-Render.h"

#include "network_common.h"
#include "network_connection.h"

#include "network_simulation.h"


struct StateSyncSnapshot
{
	std::chrono::time_point<std::chrono::high_resolution_clock> time;
	std::vector<std::pair<uint32_t, b2Transform>> boxes; // B2VEC2  -> BoxData
};


class MouseQueryCallback : public b2QueryCallback
{
public:
	MouseQueryCallback(const b2Vec2& point)
	{
		m_point = point;
		m_fixture = nullptr;
	}

	bool ReportFixture(b2Fixture* fixture)
	{
		b2Body* body = fixture->GetBody();
		if (body->GetType() == b2_dynamicBody)
		{
			bool inside = fixture->TestPoint(m_point);
			if (inside)
			{
				m_fixture = fixture;

				// We are done, terminate the query.
				return false;
			}
		}

		// Continue the query.
		return true;
	}

	b2Vec2 m_point;
	b2Fixture* m_fixture;
};

class StateSyncSimulation : public NetworkSimulation
{
protected:
	int _kbpsTarget = 100;

	b2World* _world; // Box
	GLESDebugDraw _debugDraw;
	b2Body* _ground = nullptr;
	b2MouseJoint* _mouseJoint = nullptr;
	b2Vec2 _mouseWorld;

	std::map<Connection*, bool> _connectionSynchronized;
	std::map<uint32_t, b2Body*> _boxes;


	int _stepCount;

	const int _count = 12; // For triangle of boxes
	bool _pause = false;

	static uint32_t id;
public:
	StateSyncSimulation();
	virtual ~StateSyncSimulation();

	virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags);
	virtual void Step() = 0;

	virtual bool MouseDown(const b2Vec2& p) { return false; };
	virtual void MouseMove(const b2Vec2& p) {};
	virtual void MouseUp(const b2Vec2& p) {};

	uint32_t CreateBox();
	bool MoveBox(uint32_t id, b2Vec2 pos);
};

// Server
class S_StateSyncSimulation : public StateSyncSimulation
{
private:
	std::map<Connection*, std::map<b2Body*, float>> _connectionPriorityQueue;

	std::map<Connection*, PlayerData> _playerData;
public:
	CREATE_FUNC(S_StateSyncSimulation);
	S_StateSyncSimulation();

	virtual void GenerateMessages(MessageFactory* mf, Connection* con);
	virtual bool ProcessMessages(Connection * con);

	virtual void Step();

	bool MouseDown(const b2Vec2& p);
	void MouseMove(const b2Vec2& p);
	void MouseUp(const b2Vec2& p);
};

// Client
class C_StateSyncSimulation : public StateSyncSimulation
{
private:
	bool mDown = false;
	b2Vec2 mPos;


	bool isShowDemo;
public:
	CREATE_FUNC(C_StateSyncSimulation);
	C_StateSyncSimulation();

	virtual void Step();
	virtual bool ProcessMessages(Connection * con);
	virtual void GenerateMessages(MessageFactory* mf, Connection* con);


	bool MouseDown(const b2Vec2& p);
	void MouseMove(const b2Vec2& p);
	void MouseUp(const b2Vec2& p);
};

#endif