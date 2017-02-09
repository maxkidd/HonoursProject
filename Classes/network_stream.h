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
	bool SerializeInteger(uint32_t value, uint32_t min_value, uint32_t max_value)
	{
		assert(min_value < max_value);
		assert(value >= min_value);
		assert(value <= max_value);

		uint32_t numDiff = (max_value - min_value);
		int bits = 0;
		while (numDiff >>= 1)
		{
			bits++;
		}
		
		value -= min_value;

		_bitWriter.WriteBits(value, bits);
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
	bool SerializeInteger(uint32_t& value, uint32_t min_value, uint32_t max_value)
	{
		assert(min_value < max_value);

		uint32_t numDiff = (max_value - min_value);
		int bits = 0;
		while (numDiff >>= 1)
		{
			bits++;
		}
		assert(bits <= 32);
		uint32_t readValue = _bitReader.ReadBits(bits);

		value = readValue + min_value;

		return true;
	}

private:
	BitRead _bitReader;
};
#endif