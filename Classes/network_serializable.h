#ifndef _NETWORK_SERIALIZABLE_H_
#define _NETWORK_SERIALIZABLE_H_


#include "network_stream.h"

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