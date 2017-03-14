#ifndef _NETWORK_STREAM_H_
#define _NETWORK_STREAM_H_

#include "network_bitpack.h"

#include <stdint.h>
using namespace std;

class MessageFactory;
class PacketFactory;

struct StreamContext
{
	MessageFactory* _messageFactory = nullptr;
	PacketFactory* _packetFactory = nullptr;
};

class BaseStream
{
public:
	void SetContext(StreamContext* context) { _context = context; };
	StreamContext* GetContext() { return _context; };

private:
	StreamContext* _context;
};

class OStream : public BaseStream
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

		int bits = (min_value == max_value) ? 0 : log2(max_value - min_value) + 1;
		
		value -= min_value;

		_bitWriter.WriteBits(value, bits);
		return true;
	}

	bool SerializeBits(uint32_t value, int bits)
	{
		assert(bits > 0);
		assert(bits <= 32);

		_bitWriter.WriteBits(value, bits);

		return true;
	}


	void Flush() { _bitWriter.FlushBits(); }

	int BytesUsed() { return _bitWriter.GetBytesWritten(); }
private:

	BitWrite _bitWriter;
};

class InStream : public BaseStream
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


		int bits = (min_value == max_value) ? 0 : log2(max_value - min_value) + 1;

		assert(bits <= 32);
		uint32_t readValue = _bitReader.ReadBits(bits);

		value = (uint32_t) readValue + min_value;

		return true;
	}
	bool SerializeBits(uint32_t& value, int bits)
	{
		assert(bits > 0);
		assert(bits <= 32);

		value = _bitReader.ReadBits(bits);

		return true;
	}

private:
	BitRead _bitReader;
};
#endif