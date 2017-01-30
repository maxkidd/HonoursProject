#ifndef _NETWORK_STREAM_H_
#define _NETWORK_STREAM_H_

#include "network_bitpack.h"

#include <stdint.h>
using namespace std;

class OStream
{
public:
	enum { IsWriting = 1 };
	enum { IsReading = 0 };

	OStream(uint32_t* buffer, int bytes) : _bitWriter(buffer, bytes) {}

	bool SerializeInteger(uint32_t value)
	{
		_bitWriter.WriteBits(value, 32);
		return true;
	}

	void Flush() { _bitWriter.FlushBits(); }

	int BytesUsed() { return _bitWriter.GetBytesWritten(); }
private:

	BitWrite _bitWriter;
};

class InStream
{
public:
	enum { IsWriting = 0 };
	enum { IsReading = 1 };

	InStream(uint32_t* buffer, int bytes) : _bitReader(buffer, bytes) {}

	bool SerializeInteger(uint32_t& value)
	{
		value = _bitReader.ReadBits(32);
		return true;
	}

private:
	BitRead _bitReader;
};
#endif