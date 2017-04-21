#include "StateSyncSimulation.h"


#include "SnapshotInterpolationLayer.h"
#include "StateSyncLayer.h"

StateSyncSimulation::StateSyncSimulation() : _debugDraw(15.0f)
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
}

StateSyncSimulation::~StateSyncSimulation()
{
}

void StateSyncSimulation::draw(cocos2d::Renderer * renderer, const cocos2d::Mat4 & transform, uint32_t flags)
{
	Layer::draw(renderer, transform, flags);

	_world->DrawDebugData();
}

uint32_t StateSyncSimulation::id(1);

S_StateSyncSimulation::S_StateSyncSimulation()
{
	{
		float boxWidth = 0.5f;
		b2PolygonShape shape;
		shape.SetAsBox(boxWidth, boxWidth);

		b2Vec2 x(10.0f, 50.0f);
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
	if (_connectionSynchronized.find(con) == _connectionSynchronized.end())
	{
		b2Body* body = _world->GetBodyList();
		b2Body* prevBody = body;

		while (body)
		{
			
			uint32_t* id = (uint32_t*)body->GetUserData();

			if (*id == uint32_t(-1))
				goto next_body;

			b2Vec2 pos = body->GetPosition();

			float rads = body->GetAngle();
			float deg = rads * (180.0f / M_PI);

			StateSyncBoxCreate* create = (StateSyncBoxCreate*)mf->Create(STATESYNC_MESSAGE_CREATE_BOX);
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

			// Add priority object to connection
			_connectionPriorityQueue[con][body] = 0.0f;

		next_body:
			prevBody = body;
			body = body->GetNext();
			if (prevBody == body)
				break;
		}
		_connectionSynchronized[con] = true;

	}
	else // Synchronized
	{
		int bytesTarget = _kbpsTarget * 10; 

		MeasureStream stream;

		// Priority queue for this connection
		auto priorityQueue = _connectionPriorityQueue[con];

		// Vector to copy to and sort
		std::vector<std::pair<b2Body*, float>> priority;

		// Copy
		for (auto it = priorityQueue.begin(); it != priorityQueue.end(); ++it)
			priority.push_back(*it);
		
		// Sort priority
		std::sort(priority.begin(), priority.end(), [&](auto a, auto  b)
		{
			return a.second > b.second;
		});


		for (auto p : priority)
		{
			b2Body* body = p.first;

			StateSyncBoxMove* move = (StateSyncBoxMove*)mf->Create(STATESYNC_MESSAGE_UPDATE_BOX);

			move->Serialize(stream);
			// Don't send anymore messages once reaching target
			if (stream.BytesUsed() > bytesTarget)
				break;


			b2Vec2 pos = body->GetPosition();
			b2Vec2 velocity = body->GetLinearVelocity();

			float deg = body->GetAngle() * (180.0f / M_PI);

			if (deg > 0.0f)
				move->rot = (uint32_t((deg)) % 360);
			else
				move->rot = 360 - (uint32_t(abs(deg)) % 360);

			uint32_t* id = (uint32_t*)body->GetUserData();

			move->id = *id;
			move->x = pos.x;
			move->y = pos.y;
			move->velocityX = velocity.x;
			move->velocityY = velocity.y;
			move->rotVel = body->GetAngularVelocity();

			con->SendMsg(move);

			_connectionPriorityQueue[con][p.first] = 0.0f; // Reset priority
		}
	}
	
}

bool S_StateSyncSimulation::ProcessMessages(Connection * con)
{
	NMessage* message;
	while (message = con->ReceiveMsg())
	{
		switch (message->GetType())
		{
		case STATESYNC_MESSAGE_USER_CMD:
		{
			UserCMD* userInput = (UserCMD*)message;

			if (userInput->mDown ) // Mouse Down
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

	// For each connection
	for (auto it1 = _connectionPriorityQueue.begin(); it1 != _connectionPriorityQueue.end(); ++it1)
	{
		// For each body-priority map
		for (auto it2 = it1->second.begin(); it2 != it1->second.end(); ++it2)
		{
			it2->second += it2->first->IsAwake() ? 0.1f : 0.0f; // Basic increase of priority over time if awake
		}
	}



}

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

		uint32_t* test = (uint32_t*)body->GetUserData();
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

C_StateSyncSimulation::C_StateSyncSimulation()
{
}


void C_StateSyncSimulation::Step()
{
	// World steps
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

	// Synchronize messages
}

void C_StateSyncSimulation::GenerateMessages(MessageFactory * mf, Connection * con)
{
	// Client data
	UserCMD* userCMD = (UserCMD*)mf->Create(STATESYNC_MESSAGE_USER_CMD);

	userCMD->mDown = mDown;
	userCMD->mX = mPos.x;
	userCMD->mY = mPos.y;

	con->SendMsg(userCMD);
}

bool C_StateSyncSimulation::MouseDown(const b2Vec2 & p)
{
	mDown = true;
	mPos = p;
	return true;
}

void C_StateSyncSimulation::MouseMove(const b2Vec2 & p)
{
	mPos = p;
}

void C_StateSyncSimulation::MouseUp(const b2Vec2 & p)
{
	mDown = false;
	mPos = p;
}

bool C_StateSyncSimulation::ProcessMessages(Connection * con)
{
	// Receive messages

	//if (_stepCount % 6)// To counter jitter
	{
		NMessage* message;
		while (message = con->ReceiveMsg())
		{
			switch (message->GetType())
			{
			case STATESYNC_MESSAGE_CREATE_BOX:
				//Process creation
			{
				StateSyncBoxCreate* create = (StateSyncBoxCreate*)message;

				uint32_t id = create->id;
				float x = create->x;
				float y = create->y;
				float rads = float(create->rot) / (180.0f / M_PI);

				if (_boxes.find(id) != _boxes.end())
					continue; // Already created

				b2BodyDef bodyDef;
				bodyDef.type = b2_dynamicBody;
				bodyDef.position = b2Vec2(x, y);
				bodyDef.angle = rads;

				b2PolygonShape shape;
				shape.SetAsBox(0.5f, 0.5f);

				b2Body* body = _world->CreateBody(&bodyDef);
				body->SetUserData(new uint32_t(id));
				body->CreateFixture(&shape, 5.0f);

				_boxes[id] = body;
			}

			break;
			case STATESYNC_MESSAGE_UPDATE_BOX:
			{
				// Process move
				StateSyncBoxMove* move = (StateSyncBoxMove*)message;

				uint32_t id = move->id;
				float x = move->x;
				float y = move->y;
				float rads = float(move->rot) / (180.0f / M_PI);

				float velocityX = move->velocityX;
				float velocityY = move->velocityY;
				float rotVel = move->rotVel;

				if (_boxes.find(id) == _boxes.end())
					continue; // Not created

				_boxes[id]->SetTransform(b2Vec2(x, y), rads);
				_boxes[id]->SetLinearVelocity(b2Vec2(velocityX, velocityY));
				_boxes[id]->SetAngularVelocity(rotVel);
			}
			break;
			default:
				break;
			}
		}
	}


	return true;
}