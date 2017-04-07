#include "SnapshotInterpolationSimulation.h"

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
SnapshotInterpolationSimulation::SnapshotInterpolationSimulation()
{

}

bool SnapshotInterpolationSimulation::ProcessMessages(Connection * con)
{
	return false;
}

uint32_t S_SnapshotInterpolationSimulation::id(0);

S_SnapshotInterpolationSimulation::S_SnapshotInterpolationSimulation()
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
		_ground->SetUserData(new uint32_t(-1));
		_ground->SetTransform(b2Vec2(40.0f, 5.0f), 0.0f);
		
	}
	{
		b2BodyDef bodyDef;
		_ground = _world->CreateBody(&bodyDef);

		b2EdgeShape shape;
		shape.Set(b2Vec2(0.0f, -40.0f), b2Vec2(0.0f, 40.0f));
		_ground->CreateFixture(&shape, 0.0f);
		_ground->SetUserData(new uint32_t(-1));
		_ground->SetTransform(b2Vec2(0.0f, 20.0f), 0.0f);
	}
	{
		b2BodyDef bodyDef;
		_ground = _world->CreateBody(&bodyDef);

		b2EdgeShape shape;
		shape.Set(b2Vec2(0.0f, -40.0f), b2Vec2(0.0f, 40.0f));
		_ground->CreateFixture(&shape, 0.0f);
		_ground->SetUserData(new uint32_t(-1));
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


void S_SnapshotInterpolationSimulation::GenerateMessages(MessageFactory * mf, Connection * con)
{
	// New connection
	if (_connectionSynchronized.find(con) == _connectionSynchronized.end())
	{
		b2Body* body = _world->GetBodyList();
		b2Body* prevBody = body;
		while (body)
		{
			SnapshotBoxCreate* create = (SnapshotBoxCreate*)mf->Create(SNAPSHOT_MESSAGE_CREATE_BOX);

			uint32_t* id = (uint32_t*)body->GetUserData();

			if (*id == -1)
				goto next_body;

			b2Vec2 pos = body->GetPosition();

			float deg = body->GetAngle() * (180.0f / M_PI);
			if (deg > 0.0f)
				create->rot = (uint32_t((deg)) % 360);
			else
				create->rot = 360 - (uint32_t(abs(deg)) % 360);

			create->id = *id;
			create->x = pos.x;
			create->y = pos.y;

			con->SendMsg(create);
			
			next_body:
			// Next body
			prevBody = body;
			body = body->GetNext();
			if (prevBody == body)
				break;
		}
		_connectionSynchronized[con] = true;
	}
	else // Synchronized
	{
		b2Body* body = _world->GetBodyList();
		b2Body* prevBody = body;
		while (body)
		{
			if (!body->IsAwake()) // Continue to next box if sleeping
				goto label_end;

			uint32_t* id = (uint32_t*)body->GetUserData();

			if (*id == -1)
				goto label_end;

			SnapshotBoxMove* move = (SnapshotBoxMove*)mf->Create(SNAPSHOT_MESSAGE_MOVE_BOX);

			b2Vec2 pos = body->GetPosition();

			float deg = body->GetAngle() * (180.0f / M_PI);

			if (deg > 0.0f)
				move->rot = (uint32_t((deg)) % 360);
			else
				move->rot = 360 - (uint32_t(abs(deg)) % 360);

			move->id = *id;
			move->x = pos.x;
			move->y = pos.y;

			con->SendMsg(move);

		label_end:

			prevBody = body;
			body = body->GetNext();
			if (prevBody == body)
				break;
		}
	}
}

bool S_SnapshotInterpolationSimulation::ProcessMessages(Connection * con)
{
	NMessage* message;
	while (message = con->ReceiveMsg())
	{
		switch (message->GetType())
		{
		case SNAPSHOT_MESSAGE_USER_CMD:
		{
			UserCMD* userInput = (UserCMD*)message;

			if (userInput->mDown) // Mouse Down
			{
				b2Vec2 p = b2Vec2(userInput->mX, userInput->mY);
				_playerData[con].mPos = p;
				if (!_playerData[con].mState)// New touch
				{


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

						uint32_t* test = (uint32_t*)body->GetUserData();
						b2MouseJointDef def;
						def.bodyA = _ground;
						def.bodyB = body;
						def.target = p;
						def.maxForce = _mForce * body->GetMass();
						body->SetAwake(true);

						// 
						_playerData[con].body = body;
						_playerData[con].mJoint = (b2MouseJoint*)_world->CreateJoint(&def);
						_playerData[con].mState = true;
						//_mouseJoint = (b2MouseJoint*)_world->CreateJoint(&def);
						return true;
					}
				}
				else // Continueing touch
				{

					_playerData[con].mPos = p;
					if (_playerData[con].mJoint)
					{
						_playerData[con].mJoint->SetTarget(p);
					}
				}
			}
			if (!userInput->mDown)// Release mouse
			{
				if (_playerData[con].mJoint)
				{
					_world->DestroyJoint(_playerData[con].mJoint);
					_playerData[con].mJoint = nullptr;
				}
				_playerData[con].mState = false;
			}


		}
		break;
		default:
			break;
		}
	}
	return true;
}

void S_SnapshotInterpolationSimulation::draw(cocos2d::Renderer * renderer, const cocos2d::Mat4 & transform, uint32_t flags)
{

	Layer::draw(renderer, transform, flags);

	_world->DrawDebugData();
}

void S_SnapshotInterpolationSimulation::Step()
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

bool S_SnapshotInterpolationSimulation::MouseDown(const b2Vec2 & p)
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
		def.maxForce = _mForce * body->GetMass();
		_mouseJoint = (b2MouseJoint*)_world->CreateJoint(&def);
		body->SetAwake(true);
		return true;
	}

	return false;
}

void S_SnapshotInterpolationSimulation::MouseMove(const b2Vec2 & p)
{
	_mouseWorld = p;
	if (_mouseJoint)
	{
		_mouseJoint->SetTarget(p);
	}
}

void S_SnapshotInterpolationSimulation::MouseUp(const b2Vec2 & p)
{
	if (_mouseJoint)
	{
		_world->DestroyJoint(_mouseJoint);
		_mouseJoint = nullptr;
	}

}

C_SnapshotInterpolationSimulation::C_SnapshotInterpolationSimulation() : _debugDraw(15.0f)
{

	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	//flags += b2Draw::e_jointBit;
	//flags += b2Draw::e_aabbBit;
	//flags += b2Draw::e_pairBit;
	//flags += b2Draw::e_centerOfMassBit;
	_debugDraw.SetFlags(flags);

	_boxVertices[0] = b2Vec2(-0.5f, -0.5f);
	_boxVertices[1] = b2Vec2(0.5f, -0.5f);
	_boxVertices[2] = b2Vec2(0.5f, 0.5f);
	_boxVertices[3] = b2Vec2(-0.5f, 0.5f);
}

void C_SnapshotInterpolationSimulation::draw(cocos2d::Renderer * renderer, const cocos2d::Mat4 & transform, uint32_t flags)
{
	Layer::draw(renderer, transform, flags);

	// Render boxes
	for (auto box : _boxes_interp)
	{
		b2Transform t = box.second;
		
		b2Vec2 verts[4];
		for (int i = 0; i < 4; ++i)
		{
			verts[i] = b2Mul(t, _boxVertices[i]);
		}

		_debugDraw.DrawSolidPolygon(verts, 4, b2Color(0.9f, 0.7f, 0.7f));
	}
	/*for (auto box : _boxes_interp2)
	{
		b2Transform t = box.second;

		b2Vec2 verts[4];
		for (int i = 0; i < 4; ++i)
		{
			verts[i] = b2Mul(t, _boxVertices[i]);
		}
		_debugDraw.DrawPolygon(verts, 4, b2Color(0.9f, 0.3f, 0.5f));
	}*/

}

void C_SnapshotInterpolationSimulation::Step()
{// Interpolation done here
	if (snapshots.size() < 4)
		return;


	// Linear interolation test
	WorldSnapshot first = snapshots[0];
	WorldSnapshot second = snapshots[1];
	WorldSnapshot third = snapshots[2];
	WorldSnapshot fourth = snapshots[3];

	std::chrono::duration<float> elapsed = std::chrono::high_resolution_clock::now() - first.time; // now - second snapshot time

	float t = std::min(1.0f, elapsed.count()*20.0f);// between 0 and 1 for every 100ms

	auto it1 = first.boxes.begin();
	auto it2 = second.boxes.begin();
	auto it3 = third.boxes.begin();

	for (auto box : _boxes)
	{
		uint32_t id = box.first;
		while (id != it1->first)
		{
			it1++;
		}
		while (id != it2->first)
		{
			it2++;
		}
		while (id != it3->first)
		{
			it3++;
		}
		b2Vec2 firstVec = it1->second.p;
		b2Vec2 secondVec = it2->second.p;
		b2Vec2 thirdVec = it3->second.p;
		float firstRot = it1->second.q.GetAngle();
		float secondRot = it2->second.q.GetAngle();

		float x = Cubic(firstVec.x, secondVec.x, firstVec.x + (firstVec.x - secondVec.x) / 10.0f,
			secondVec.x + (secondVec.x - thirdVec.x) / 10.0f, t);
		float y = Cubic(firstVec.y, secondVec.y, firstVec.y + (firstVec.y - secondVec.y) / 10.0f,
			secondVec.y + (secondVec.y - thirdVec.y) / 10.0f, t);

		//_boxes_interp[box.first].Set(Lerp(secondVec, firstVec, t), LerpRad(secondRot, firstRot, t));
		_boxes_interp[box.first].Set(b2Vec2(x, y), LerpRad(secondRot, firstRot, t));
	}
}

void C_SnapshotInterpolationSimulation::GenerateMessages(MessageFactory * mf, Connection * con)
{
	UserCMD* userCMD = (UserCMD*)mf->Create(SNAPSHOT_MESSAGE_USER_CMD);

	userCMD->mDown = mDown;
	userCMD->mX = mPos.x;
	userCMD->mY = mPos.y;

	con->SendMsg(userCMD);
}

bool C_SnapshotInterpolationSimulation::MouseDown(const b2Vec2 & p)
{
	mDown = true;
	mPos = p;
	return true;
}

void C_SnapshotInterpolationSimulation::MouseMove(const b2Vec2 & p)
{
	mPos = p;
}

void C_SnapshotInterpolationSimulation::MouseUp(const b2Vec2 & p)
{
	mDown = false;
	mPos = p;
}

bool C_SnapshotInterpolationSimulation::ProcessMessages(Connection * con)
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
			float rads = float(create->rot) / (180.0f / M_PI);

			if (_boxes.find(id) != _boxes.end())
				continue; // Already created

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
			float rads = float(move->rot) / (180.0f / M_PI);


			_boxes[id].Set(b2Vec2(x, y), rads);
		}
		break;
		default:
			break;
		}
	}

	// Remove old snapshot
	if (snapshots.size() > 6)
		snapshots.pop_back();

	// Create new snapshot and push to the queue
	WorldSnapshot newSnapshot;
	newSnapshot.time = std::chrono::high_resolution_clock::now();
	for (auto b : _boxes)
	{
		newSnapshot.boxes.push_back(b);
	}
	snapshots.push_front(newSnapshot);

	return true;
}