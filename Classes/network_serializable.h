#ifndef _NETWORK_SERIALIZABLE_H_
#define _NETWORK_SERIALIZABLE_H_


#include "network_stream.h"

class Serializable
{
public:
	virtual ~Serializable() {}

	virtual bool Serialize(class IStream& stream) = 0;
	virtual bool Serialize(class OStream& stream) = 0;
};

#endif