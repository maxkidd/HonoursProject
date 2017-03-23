#include "StateSyncSimulation.h"

StateSyncSimulation::StateSyncSimulation()
{

}

bool StateSyncSimulation::ProcessSnapshotMessages(Connection * con)
{
	NMessage* message;
	while (message = con->ReceiveMsg()) 
	{
		switch (message->GetType())
		{
		case SNAPSHOT_MESSAGE_CREATE_BOX:
			//Process creation
		{
			SnapshotBoxCreate* create = (SnapshotBoxCreate*)message;
			

		}

			break;
		case SNAPSHOT_MESSAGE_MOVE_BOX:
		{
			// Process move
			SnapshotBoxMove* move = (SnapshotBoxMove*)message;
		}
			break;
		default:
			break;
		}
	}

	return true;
}

void StateSyncSimulation::GenerateSnapshotMessages(MessageFactory* mf, Connection * con)
{
	// Default
	/*
	for (int i = 0; i < 2; i++)
	{
		SnapshotBoxCreate* create = (SnapshotBoxCreate*)mf->Create(SNAPSHOT_MESSAGE_CREATE_BOX);
		create->id = 150;
		create->y = 150.0f;
		create->x = 150.0f;
		con->SendMsg(create);
		SnapshotBoxMove* move = (SnapshotBoxMove*)mf->Create(SNAPSHOT_MESSAGE_MOVE_BOX);
		move->id = 250;
		move->x = 250.0f;
		move->y = 250.0f;
		con->SendMsg(move);
	}*/
}

uint32_t S_StateSyncSimulation::id(0);

S_StateSyncSimulation::S_StateSyncSimulation()
	: _debugDraw(15.0f)
{
	b2Vec2 gravity;
	gravity.Set(0.0f, -5.0f);
	_world = new b2World(gravity);

	_world->SetDebugDraw(&_debugDraw);

	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	//flags += b2Draw::e_jointBit;
	//flags += b2Draw::e_aabbBit;
	//flags += b2Draw::e_pairBit;
	//flags += b2Draw::e_centerOfMassBit;
	_debugDraw.SetFlags(flags);

	_stepCount = 0;

	{
		b2BodyDef bodyDef;
		_ground = _world->CreateBody(&bodyDef);

		b2EdgeShape shape;
		shape.Set(b2Vec2(-40.0f, 0.0f), b2Vec2(40.0f, 0.0f));
		_ground->CreateFixture(&shape, 0.0f);
		_ground->SetUserData(new uint32_t(++id));
		_ground->SetTransform(b2Vec2(40.0f, 5.0f), 0.0f);
		
	}
	{
		b2BodyDef bodyDef;
		_ground = _world->CreateBody(&bodyDef);

		b2EdgeShape shape;
		shape.Set(b2Vec2(0.0f, -40.0f), b2Vec2(0.0f, 40.0f));
		_ground->CreateFixture(&shape, 0.0f);
		_ground->SetUserData(new uint32_t(++id));
		_ground->SetTransform(b2Vec2(0.0f, 20.0f), 0.0f);
	}
	{
		b2BodyDef bodyDef;
		_ground = _world->CreateBody(&bodyDef);

		b2EdgeShape shape;
		shape.Set(b2Vec2(0.0f, -40.0f), b2Vec2(0.0f, 40.0f));
		_ground->CreateFixture(&shape, 0.0f);
		_ground->SetUserData(new uint32_t(++id));
		_ground->SetTransform(b2Vec2(70.0f, 20.0f), 0.0f);
	}
	{
		float boxWidth = 0.5f;
		b2PolygonShape shape;
		shape.SetAsBox(boxWidth, boxWidth);

		b2Vec2 x(0.0f, 50.0f);
		b2Vec2 y;
		b2Vec2 deltaX(0.5625f, 1.25f);
		b2Vec2 deltaY(1.125f, 0.0f);

		for (int i = 0; i < _count; ++i)
		{
			y = x;
			for (int j = i; j < _count; ++j)
			{
				b2BodyDef bodyDef;
				bodyDef.type = b2_dynamicBody;
				bodyDef.position = y;
				b2Body* body = _world->CreateBody(&bodyDef);
				body->SetUserData(new uint32_t(++id));
				body->CreateFixture(&shape, 5.0f);

				y += deltaY;
			}
			x += deltaX;
		}
	}
}

void S_StateSyncSimulation::GenerateMessages(MessageFactory * mf, Connection * con)
{
	b2Body* body = _world->GetBodyList();
	b2Body* prevBody = body;

	while (body)
	{
		if (!body->IsAwake()) // Continue to next box if sleeping
			goto label_end;

		SnapshotBoxCreate* create = (SnapshotBoxCreate*)mf->Create(SNAPSHOT_MESSAGE_CREATE_BOX);

		uint32_t* id = (uint32_t*)body->GetUserData();
		b2Vec2 pos = body->GetPosition();
		
		float rads = body->GetAngle();
		float deg = rads * (180.0f / M_PI);
		if (rads > 0.0f)
		{
			create->rot = (uint32_t((deg)) % 360);
		}
		else
		{
			create->rot = 360 - (uint32_t(abs(deg)) % 360);
		}
		create->id = *id;
		create->x = pos.x;
		create->y = pos.y;

		con->SendMsg(create);

	label_end:

		prevBody = body;
		body = body->GetNext();
		if (prevBody == body)
			break;
	}
}

bool S_StateSyncSimulation::ProcessMessages(Connection * con)
{
	return false;
}

void S_StateSyncSimulation::draw(cocos2d::Renderer * renderer, const cocos2d::Mat4 & transform, uint32_t flags)
{

	Layer::draw(renderer, transform, flags);

	_world->DrawDebugData();
}

void S_StateSyncSimulation::Step()
{
	float32 timestep = 1.0f / 60.0f;

	if (_pause)
	{
		timestep = 0.0f;
	}

	_world->Step(timestep, 8, 3);


	if (timestep > 0.0f)
	{
		++_stepCount;
	}
}

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
bool S_StateSyncSimulation::MouseDown(const b2Vec2 & p)
{
	_mouseWorld = p;

	if (_mouseJoint) // If exists
		return false;

	CCLOG("Mousepoint: %f, %f", p.x, p.y);

	b2AABB aabb;
	b2Vec2 d;
	d.Set(0.001f, 0.001f);
	aabb.lowerBound = p - d;
	aabb.upperBound = p + d;

	MouseQueryCallback callback(p);
	_world->QueryAABB(&callback, aabb);

	if (callback.m_fixture)
	{
		b2Body* body = callback.m_fixture->GetBody();
		b2MouseJointDef def;
		def.bodyA = _ground;
		def.bodyB = body;
		def.target = p;
		def.maxForce = 1000.0f * body->GetMass();
		_mouseJoint = (b2MouseJoint*)_world->CreateJoint(&def);
		body->SetAwake(true);
		return true;
	}

	return false;
}

void S_StateSyncSimulation::MouseMove(const b2Vec2 & p)
{
	_mouseWorld = p;
	if (_mouseJoint)
	{
		_mouseJoint->SetTarget(p);
	}
}

void S_StateSyncSimulation::MouseUp(const b2Vec2 & p)
{
	if (_mouseJoint)
	{
		_world->DestroyJoint(_mouseJoint);
		_mouseJoint = nullptr;
	}

}

C_StateSyncSimulation::C_StateSyncSimulation() : _debugDraw(15.0f)
{
	_boxVertices[0] = b2Vec2(-0.5f, -0.5f);
	_boxVertices[1] = b2Vec2(0.5f, -0.5f);
	_boxVertices[2] = b2Vec2(0.5f, 0.5f);
	_boxVertices[3] = b2Vec2(-0.5f, 0.5f);
}

void C_StateSyncSimulation::draw(cocos2d::Renderer * renderer, const cocos2d::Mat4 & transform, uint32_t flags)
{
	// Render boxes
	for (auto box : _boxes_interp)
	{
		b2Transform t = box.second;
		
		b2Vec2 verts[4];
		for (int i = 0; i < 4; ++i)
		{
			verts[i] = b2Mul(t, _boxVertices[i]);
		}
		_debugDraw.DrawPolygon(verts, 4, b2Color(0.5f, 0.5f, 0.9f));
	}

}

void C_StateSyncSimulation::Step()
{
	// Interpolation done here
	if (snapshots.size() < 3)
		return;
	

	// Linear interolation test
	WorldSnapshot first = snapshots.end()[-2];
	WorldSnapshot second = snapshots.end()[-1];

	std::chrono::duration<float> elapsed = std::chrono::high_resolution_clock::now() - second.time; // now - second snapshot time
	float t = std::min(1.0f, elapsed.count() * 10.0f);// between 0 and 1 for every 100ms
	//CCLOG("Lerp time: %4.2f", t);

	auto it1 = first.boxes.begin();
	auto it2 = second.boxes.begin();

	//std::transform(first.boxes.begin(), first.boxes.end(), second.boxes.begin(), second.boxes.begin(), std::plus<int>());
	for (auto box : _boxes)
	{
		uint32_t id = box.first;
		while(id != it1->first)
		{
			it1++;
		}
		while (id != it2->first)
		{
			it2++;
		}
		b2Vec2 firstVec = it1->second.p;
		b2Vec2 secondVec = it2->second.p;
		float firstRot = it1->second.q.GetAngle();
		float secondRot = it2->second.q.GetAngle();

		_boxes_interp[box.first].Set(Lerp(firstVec, secondVec, t), LerpRad(firstRot, secondRot, t));
	}
}

void C_StateSyncSimulation::GenerateMessages(MessageFactory * mf, Connection * con)
{
}

bool C_StateSyncSimulation::ProcessMessages(Connection * con)
{
	// Receive messages
	NMessage* message;
	while (message = con->ReceiveMsg())
	{
		switch (message->GetType())
		{
		case SNAPSHOT_MESSAGE_CREATE_BOX:
			//Process creation
		{
			SnapshotBoxCreate* create = (SnapshotBoxCreate*)message;
			uint32_t id = create->id;
			float x = create->x;
			float y = create->y;
			float deg = float(create->rot);
			float rads = deg / (180.0f / M_PI);

			//if (_boxes.find(id) != _boxes.end())
			//	return false; // Already created

			_boxes[id].Set(b2Vec2(x, y), rads);

		}

		break;
		case SNAPSHOT_MESSAGE_MOVE_BOX:
		{
			// Process move
			SnapshotBoxMove* move = (SnapshotBoxMove*)message;
			uint32_t id = move->id;
			float x = move->x;
			float y = move->y;


			if (auto box = _boxes.find(id) != _boxes.end())
			{
				_boxes[box].Set(b2Vec2(x, y), 0.0f);
			}
		}
		break;
		default:
			break;
		}
	}

	// Remove old snapshot
	if (snapshots.size() > 4)
		snapshots.pop_front();

	// Create new snapshot and push to the queue
	WorldSnapshot newSnapshot;
	newSnapshot.time = std::chrono::high_resolution_clock::now();
	for (auto b : _boxes)
	{
		newSnapshot.boxes.push_back(b);
	}
	snapshots.push_back(newSnapshot);

	return true;
}