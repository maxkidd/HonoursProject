#ifndef _NETWORK_STREAM_H_
#define _NETWORK_STREAM_H_

#include "network_bitpack.h"

#include <stdint.h>
using namespace std;

class OStream
{
	enum { IsWriting = 1 };
	enum { IsReading = 0 };

	OStream(uint32_t* buffer, int bytes) : _bitWriter(buffer, bytes) {}

	bool SerializeInteger(uint32_t value)
	{
		_bitWriter.WriteBits(value, 32);
	}
	BitWrite _bitWriter;
};

class IStream
{
public:
	enum { IsWriting = 0 };
	enum { IsReading = 1 };

	bool SerializeInteger(uint32_t value)
	{
		_bitReader.ReadBits(32);
	}
	BitRead _bitReader;
};
#endif