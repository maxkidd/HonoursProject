#include "network_message.h"

Message* MessageFactory::Create(int type)
{
	Message* message = CreateMessage(type);

	if (!message)
		return nullptr;

	return message;
}

void MessageFactory::SetMessageType(Message* message, int type)
{
	message->SetType(type);
}

void MessageFactory::SetFactory(Message * packet)
{
	packet->SetMessageFactory(*this);
}

