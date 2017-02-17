#include "network_channel.h"

#include "network_serializable.h"

template<typename Stream> bool ChannelPacket::Serialize(Stream& stream, MessageFactory& mf, int channels)
{
	// Serialize bool if has messages
	// Serialize number of messages
	// For each

	bool hasMessages = Stream::IsWriting && numMessages > 0;

	SerializeBool(stream, hasMessages);

	if (hasMessages)
	{
		stream.SerializeInteger(numMessages);

		for (int i = 0; i < numMessages; i++)
		{
			int type;
			if (Stream::IsWriting)
				type = messages[i]->GetType();

			stream.SerializeInteger(type, 0, mf._messageTypes);

			if (Stream::IsReading) 
			{
				Message* message = mf.Create(type);
				messages.push_back(message);
			}
			if(messages[i])
				messages[i].SerializeInternal(stream)
		}
	}
	
	return true;
}

Channel::Channel(MessageFactory * mf, int id) : _mf(mf), _id(id)
{
}

void Channel::SendMessage(Message * message)
{
	_sendQueue.push(message);
}

Message * Channel::ReceiveMessage()
{
	Message* returnMsg = _recvQueue.front();
	_recvQueue.pop();
	return returnMsg;
}

ChannelPacket * Channel::GeneratePacketData(int freeBits)
{
	return nullptr;
}

int Channel::GetPacketData(ChannelPacket & data, int bitsFree)
{
	if (_sendQueue.empty())
		return 0;

	int bitsUsed = 0;
	int numMessages = 0;


	std::vector<Message*> messages;

	while (true)
	{
		if (_sendQueue.empty())
			break;
		if (bitsUsed >= bitsFree)
			break;

		Message* message = _sendQueue.front();
		_sendQueue.pop();

		assert(message);

		messages.push_back(message);
		numMessages++;
	}


	if (numMessages == 0)
		return 0;

	data.numMessages = numMessages;
	data.messages = { messages };

	return 1;
}

void Channel::ProcessPacketData(const ChannelPacket & data)
{

	for (int i = 0; i < data.numMessages; i++)
	{
		Message* message = data.messages[i];

		assert(message);
		
		_recvQueue.push(message);
	}

}

