#include "Logger.h"

static NetworkLog* instance = nullptr;
NetworkLog * NetworkLog::getInstance()
{
	if (instance == nullptr)
	{
		instance = new (std::nothrow) NetworkLog;
	}

	return instance;
}

void NetworkLog::destroyInstance()
{
	if (instance)
	{
		delete instance;
		instance = nullptr;
	}
}
