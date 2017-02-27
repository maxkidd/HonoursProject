#include "network_message.h"

NMessage* MessageFactory::Create(int type)
{
	NMessage* message = CreateMessage(type);

	if (!message)
		return nullptr;

	return message;
}

void MessageFactory::SetMessageType(NMessage* message, int type)
{
	message->SetType(type);
}

void MessageFactory::SetFactory(NMessage * packet)
{
	packet->SetMessageFactory(*this);
}

