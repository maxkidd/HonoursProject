#ifndef _NETWORK_BITPACK_H_
#define _NETWORK_BITPACK_H_

#include <stdint.h>
#include <assert.h>
#include <asio\ip\detail\endpoint.hpp>

using namespace std;

/**
	Bit write class
	Writes at the bit-level to a buffer converting endianness for network streams
*/
class BitWrite
{
public:
	BitWrite(uint32_t* buffer, int bytes) : _buffer(buffer),  _numWords(bytes/4)
	{
		_numBits = _numWords * 32;
		_bitsWritten = _scratchBits = _wordIndex = _scratch = 0;
	}

	void WriteBits(uint32_t data, int bits)
	{
		assert(bits > 0);
		assert(bits <= 32);
		assert(_bitsWritten + bits <= _numBits);
		assert(uint64_t(data) <= ((1ULL << bits) - 1));

		_scratch |= uint64_t(data) << _scratchBits;
		_scratchBits += bits;

		if (_scratchBits >= 32)
		{
			_buffer[_wordIndex] = asio::detail::socket_ops::host_to_network_long(uint32_t(_scratch & 0xFFFFFFFF));
			_scratch >>= 32;
			_scratchBits -= 32;
			_wordIndex++;
		}
		_bitsWritten += bits;
	}

	void FlushBits()
	{
		if (_scratchBits != 0)
		{
			assert(_scratchBits <= 32);

			uint32_t bits = GetAlignBits();

			_scratch |= uint64_t(0) << _scratchBits;
			_scratchBits += bits;

			assert(_scratchBits == 32);

			_buffer[_wordIndex] = asio::detail::socket_ops::host_to_network_long(uint32_t(_scratch & 0xFFFFFFFF));
			_scratch >>= 32;
			_scratchBits -= 32;
			_wordIndex++;

			_bitsWritten += bits;
		}
	}

	int GetAlignBits()
	{
		return (32 - (_bitsWritten % 32)) % 32;
	}

	int GetBytesWritten()
	{
		return (_bitsWritten + 7) / 8;
	}
private:

	uint32_t* _buffer;
	uint64_t _scratch;
	int _wordIndex;
	int _scratchBits;

	int _numBits;
	int _numWords;
	int _bitsWritten;
};

/**
	Bit read class
	Reades at the bit-level from a buffer converting endianness back from network streams
*/
class BitRead
{
public:
	BitRead(const uint32_t* buffer, int bytes) : _buffer(buffer), _numBytes(bytes) 
	{
		_numBits = _numBytes * 8;
		_bitsRead = _scratch = _scratchBits = _wordIndex = 0;
	}

	uint32_t ReadBits(int bits)
	{
		assert(bits > 0);
		assert(bits <= 32);
		assert(_bitsRead + bits <= _numBits);

		_bitsRead += bits;

		assert(_scratchBits >= 0 && _scratchBits <= 64);

		if (_scratchBits < bits)
		{
			_scratch |= uint64_t(asio::detail::socket_ops::network_to_host_long(_buffer[_wordIndex])) << _scratchBits;
			_scratchBits += 32;
			_wordIndex++;
		}

		assert(_scratchBits >= bits);
		assert(_scratchBits >= 0 && _scratchBits <= 64);

		const uint32_t returnVal = _scratch & ((uint64_t(1) << bits) - 1);

		_scratch >>= bits;
		_scratchBits -= bits;

		return returnVal;
	}


	int GetAlignBits()
	{
		return (8 - (_bitsRead % 8)) % 8;
	}

private:

	const uint32_t* _buffer;
	uint64_t _scratch;
	int _numBits;
	int _numBytes;
	int _bitsRead;
	int _scratchBits;
	int _wordIndex;
};


#endif