#ifndef _NETWORK_SERIALIZABLE_H_
#define _NETWORK_SERIALIZABLE_H_


#include "network_stream.h"

template <typename Stream> bool SerializeFloat(Stream& stream, float& value)
{
	uint32_t int_value;

	if (Stream::IsWriting)
		memcpy(&int_value, &value, 4);

	bool result = stream.SerializeBits(int_value, 32);

	if (Stream::IsReading)
		memcpy(&value, &int_value, 4);

	return result;
}
template <typename Stream> bool SerializeBool(Stream& stream, bool& value)
{
	uint32_t int_value;

	if (Stream::IsWriting && value)
		int_value = 1;

	bool result = stream.SerializeBits(int_value, 1);

	if (Stream::IsReading)
		value = (int_value != 0);

	return result;
}

//class IStream;
//class OStream;
class Serializable
{
public:
	Serializable() {}
	virtual ~Serializable() {}

	virtual bool SerializeInternal(class InStream& stream) = 0;
	virtual bool SerializeInternal(class OStream& stream) = 0;
};

#define VIRTUAL_SERIALIZE_FUNCTIONS()											\
	bool SerializeInternal(class InStream& stream){return Serialize(stream);};	\
	bool SerializeInternal(class OStream& stream){return Serialize(stream);};

#endif