#include "network_channel.h"

#include "network_serializable.h"

template<typename Stream> bool ChannelPacket::Serialize(Stream& stream, MessageFactory* mf, int channels)
{
	// Serialize bool if has messages
	// Serialize number of messages
	// For each


	uint32_t hasMessages = Stream::IsWriting && numMessages > 0;

	stream.SerializeInteger(hasMessages, 0, 1);
	stream.SerializeInteger(numMessages);

	if (hasMessages)
	{
		//uint32_t numOfMessages = numMessages;

		//stream.SerializeInteger(type, 0, mf->_messageTypes);
		//messages[i]->SerializeInternal(stream);

		//int initialMessageSize = messages.size();

		if (Stream::IsReading)
			messages.clear();

		for (uint32_t i = 0; i < numMessages; i++)
		{
			uint32_t type = 0;

			if (Stream::IsWriting)
				type = messages[i]->GetType();

			//stream.SerializeInteger(type, 0, mf->_messageTypes);

			if (Stream::IsReading) 
			{
				NMessage* message = mf->Create(type);
				messages.push_back(message);
			}
			
			assert(messages[i]);
			//messages[i]->SerializeInternal(stream);
		}
	}
	
	return true;
}

Channel::Channel(MessageFactory * mf) : _mf(mf)
{
}

void Channel::SendMsg(NMessage * message)
{
	_sendQueue.push(message);
}

NMessage * Channel::ReceiveMsg()
{
	if (_recvQueue.empty())
		return nullptr;

	NMessage* returnMsg = _recvQueue.front();
	_recvQueue.pop();
	return returnMsg;
}

/*ChannelPacket * Channel::GeneratePacketData(int freeBits)
{
	return nullptr;
}*/

int Channel::GetPacketData(ChannelPacket & data, int bitsFree)
{
	if (_sendQueue.empty())
		return 0;

	int bitsUsed = 0;
	uint32_t numMessages = 0;


	std::vector<NMessage*> messages;

	while (true)
	{
		if (_sendQueue.empty())
			break;
		/*if (bitsUsed >= bitsFree)
			break;*/

		NMessage* message = _sendQueue.front();
		_sendQueue.pop();

		assert(message);

		messages.push_back(message);
		numMessages++;
	}



	data.numMessages = numMessages;
	data.messages = messages;

	if (numMessages == 0)
		return 0;

	return 1;
}

void Channel::ProcessPacketData(const ChannelPacket & data)
{
	for (uint32_t i = 0; i < data.numMessages; i++)
	{
		NMessage* message = data.messages[i];

		assert(message);
		
		_recvQueue.push(message);
	}
}

bool ChannelPacket::SerializeInternal(InStream & stream, MessageFactory * mf, int channels)
{
	return Serialize(stream, mf, channels);
}

bool ChannelPacket::SerializeInternal(OStream & stream, MessageFactory * mf, int channels)
{
	return Serialize(stream, mf, channels);
}
