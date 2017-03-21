#include "SnapshotInterpolationSimulation.h"

SnapshotInterpolationSimulation::SnapshotInterpolationSimulation()
{

}

bool SnapshotInterpolationSimulation::ProcessSnapshotMessages(Connection * con)
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

void SnapshotInterpolationSimulation::GenerateSnapshotMessages(MessageFactory* mf, Connection * con)
{

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
	}
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

		_ground->SetUserData(new uint32_t(++id));
	}
	{
		float boxWidth = 0.5f;
		b2PolygonShape shape;
		shape.SetAsBox(boxWidth, boxWidth);

		b2Vec2 x(-7.0f, 100.0f);
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

void S_SnapshotInterpolationSimulation::GenerateSnapshotMessages(MessageFactory * mf, Connection * con)
{
	b2Body* body = _world->GetBodyList();
	b2Body* prevBody = body;

	while (body)
	{
		SnapshotBoxCreate* create = (SnapshotBoxCreate*)mf->Create(SNAPSHOT_MESSAGE_CREATE_BOX);

		uint32_t* id = (uint32_t*)body->GetUserData();
		b2Vec2 pos = body->GetPosition();
		//b2Transform transform = body->GetTransform();
		
		create->id = *id;
		create->x = pos.x;
		create->y = pos.y;
		con->SendMsg(create);

		prevBody = body;
		body = body->GetNext();
		if (prevBody == body)
			break;
	}
	/*for (int i = _world->GetBodyList(); i < 2; i++)
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

void S_SnapshotInterpolationSimulation::draw(cocos2d::Renderer * renderer, const cocos2d::Mat4 & transform, uint32_t flags)
{

	Layer::draw(renderer, transform, flags);

	int32 textLine = 0;
	//_debugDraw.DrawString(5, textLine, "testttttttttttttttttt");
	
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
	//_world->DrawDebugData();


	if (timestep > 0.0f)
	{
		++_stepCount;
	}
}

C_SnapshotInterpolationSimulation::C_SnapshotInterpolationSimulation() : _debugDraw(15.0f)
{
	_boxVertices[0] = b2Vec2(-0.5f, -0.5f);
	_boxVertices[1] = b2Vec2(0.5f, -0.5f);
	_boxVertices[2] = b2Vec2(0.5f, 0.5f);
	_boxVertices[3] = b2Vec2(-0.5f, 0.5f);
}

void C_SnapshotInterpolationSimulation::draw(cocos2d::Renderer * renderer, const cocos2d::Mat4 & transform, uint32_t flags)
{
	// Render boxes
	for (auto box : _boxes)
	{
		b2Transform t = box.second;
		
		b2Vec2 verts[4];
		for (int i = 0; i < 4; ++i)
		{
			verts[i] = b2Mul(t, _boxVertices[i]);
		}
		//vertices[i] = b2Mul(xf, poly->m_vertices[i]);
		_debugDraw.DrawPolygon(verts, 4, b2Color(0.5f, 0.5f, 0.9f));
	}

}

void C_SnapshotInterpolationSimulation::Step()
{
	// Interpolation done here
}

bool C_SnapshotInterpolationSimulation::ProcessSnapshotMessages(Connection * con)
{
	WorldSnapshot newSnapshot;

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

			//if (_boxes.find(id) != _boxes.end())
			//	return false; // Already created

			_boxes[id].Set(b2Vec2(x, y), 0.0f);

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

	return true;
}