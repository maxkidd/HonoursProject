#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "network_serializable.h"

class MessageFactory;

/**
	Network Message
	- Inherits serializable for messages to serialize specific message data
	- Message type defined by the message factory as a type of RTTI
*/
class NMessage : public Serializable
{
private:
	MessageFactory* _factory;
	uint32_t _type;
public:
	NMessage() : _factory(nullptr), _type(0) {}

	uint32_t GetType() { return _type; }
protected:
	friend class MessageFactory;

	void SetType(int type) { _type = type; }
	void SetMessageFactory(MessageFactory& factory)
	{_factory = &factory;}

	virtual bool Serialize(InStream& stream);
	virtual bool Serialize(OStream& stream);
};

/**
	Message Factory
	- Factory class for creating messages
	- CreateMessage inherited by the type of message factory and defined by
		the message factory macro
*/
class MessageFactory
{
public:
	MessageFactory(uint32_t messageTypes) : _messageTypes(messageTypes) {}
	virtual ~MessageFactory() {}

	NMessage* Create(uint32_t type);

protected:
	friend class ChannelPacket;

	void SetMessageType(NMessage* message, uint32_t type);
	void SetFactory(NMessage* message);

	virtual NMessage* CreateMessage(uint32_t type) { (void)type; return nullptr; }
private:
	uint32_t _messageTypes;
};

/**
	 Message Factory Macro
*/
#define MESSAGE_FACTORY_START(factory_class, base_class, num_messages)				\
class factory_class : public base_class{											\
public:																				\
	factory_class(uint32_t messageTypes = num_messages) : base_class(messageTypes) {}	\
	virtual NMessage* CreateMessage(uint32_t type){										\
		NMessage* message = base_class::CreateMessage(type);							\
		switch (type){

#define MESSAGE_FACTORY_TYPE(message_type, message_class)						\
		case message_type:														\
			message = new message_class();										\
			if (!message) return nullptr;										\
			SetMessageType(message, message_type);								\
			SetFactory(message);												\
			return message;

#define MESSAGE_FACTORY_END()			\
			default: return nullptr;	\
		}								\
	}									\
};

#endif