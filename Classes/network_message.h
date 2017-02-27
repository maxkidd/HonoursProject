#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "network_serializable.h"

class MessageFactory;

class NMessage : public Serializable
{
private:
	MessageFactory* _factory;
	int _type;
public:
	NMessage() : _factory(nullptr), _type(0) {}

	int GetType() { return _type; }
protected:
	friend class MessageFactory;

	void SetType(int type) { _type = type; }
	void SetMessageFactory(MessageFactory& factory)
	{_factory = &factory;}

	//virtual bool Serialize(InStream& stream);
	//virtual bool Serialize(OStream& stream);
};


class MessageFactory
{
public:
	MessageFactory(int messageTypes) : _messageTypes(messageTypes) {}
	virtual ~MessageFactory() {}

	NMessage* Create(int type);

protected:
	friend class ChannelPacket;

	void SetMessageType(NMessage* message, int type);
	void SetFactory(NMessage* message);

	virtual NMessage* CreateMessage(int type) { (void)type; return nullptr; }
private:
	int _messageTypes;
};


#define MESSAGE_FACTORY_START(factory_class, base_class, num_messages)				\
class factory_class : public base_class{											\
public:																				\
	factory_class(int messageTypes = num_messages) : base_class(messageTypes) {}	\
	virtual NMessage* CreateMessage(int type){										\
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