#ifndef _SNAPSHOTINTERPOLATIONSIMULATION_H_
#define _SNAPSHOTINTERPOLATIONSIMULATION_H_


#include <stdint.h>
#include <vector>
#include <cocos2d.h>
#include <Box2D\Box2D.h>
#include "Box2DTestBed\GLES-Render.h"

#include "network_common.h"
#include "network_connection.h"
#include "network_simulation.h"


struct WorldSnapshot
{
	std::chrono::time_point<std::chrono::high_resolution_clock> time;
	std::vector<std::pair<uint32_t, b2Transform>> boxes; // B2VEC2  -> BoxData
};



class SnapshotInterpolationSimulation : public NetworkSimulation
{
private:
public:
	SnapshotInterpolationSimulation();

	virtual bool ProcessMessages(Connection * con);
	//virtual void GenerateMessages(MessageFactory* mf, Connection* con);
	virtual void Step() = 0;

	virtual bool MouseDown(const b2Vec2& p) { return false; };
	virtual void MouseMove(const b2Vec2& p) {};
	virtual void MouseUp(const b2Vec2& p) {};

	uint32_t CreateBox();
	bool MoveBox(uint32_t id, b2Vec2 pos);
};

// Server
class S_SnapshotInterpolationSimulation : public SnapshotInterpolationSimulation
{
private:
	b2World* _world; // Box
	GLESDebugDraw _debugDraw;
	b2Body* _ground = nullptr;

	b2MouseJoint* _mouseJoint = nullptr;
	b2Vec2 _mouseWorld;


	int _stepCount;
	const int _count = 12;
	bool _pause = false;
	static uint32_t id;
public:
	S_SnapshotInterpolationSimulation();
	CREATE_FUNC(S_SnapshotInterpolationSimulation);

	virtual void GenerateMessages(MessageFactory* mf, Connection* con);
	virtual bool ProcessMessages(Connection * con);

	virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags);
	virtual void Step();

	bool MouseDown(const b2Vec2& p);
	void MouseMove(const b2Vec2& p);
	void MouseUp(const b2Vec2& p);
};

// Client
class C_SnapshotInterpolationSimulation : public SnapshotInterpolationSimulation
{
private:
	GLESDebugDraw _debugDraw;
	std::deque<WorldSnapshot> snapshots; // Queue of snapshots

	std::map<uint32_t, b2Transform> _boxes;
	std::map<uint32_t, b2Transform> _boxes_interp;

	b2Vec2 _boxVertices[4];
public:
	C_SnapshotInterpolationSimulation();

	virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags);
	virtual void Step();
	virtual bool ProcessMessages(Connection * con);
	virtual void GenerateMessages(MessageFactory* mf, Connection* con);

	CREATE_FUNC(C_SnapshotInterpolationSimulation);
};

#endif