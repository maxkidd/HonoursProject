#include "network_message.h"

NMessage* MessageFactory::Create(uint32_t type)
{
	NMessage* message = CreateMessage(type);

	if (!message)
		return nullptr;

	return message;
}

void MessageFactory::SetMessageType(NMessage* message, uint32_t type)
{
	message->SetType(type);
}

void MessageFactory::SetFactory(NMessage * packet)
{
	packet->SetMessageFactory(*this);
}


bool NMessage::Serialize(InStream & stream)
{
	return true;
}

bool NMessage::Serialize(OStream & stream)
{
	return true;
}