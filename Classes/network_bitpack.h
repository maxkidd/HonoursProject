#ifndef _NETWORK_BITPACK_H_
#define _NETWORK_BITPACK_H_

#include <stdint.h>
#include <assert.h>
#include <asio\ip\detail\endpoint.hpp>

using namespace std;


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
		assert(_bitsWritten + bits <= _numBits);
		//_bitsRead += bits;

		data &= (uint64_t(1) << bits) - 1;

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
			_buffer[_wordIndex] = asio::detail::socket_ops::host_to_network_long(uint32_t(_scratch & 0xFFFFFFFF));
			_scratch >>= 32;
			_scratchBits -= 32;
			_wordIndex++;
		}
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
		assert(_bitsRead + bits <= _numBits);

		_bitsRead += bits;

		if (_scratchBits < bits)
		{
			_scratch |= asio::detail::socket_ops::network_to_host_long(_buffer[_wordIndex]) << _scratchBits;
			_scratchBits += 32;
			_wordIndex++;
		}

		const uint32_t returnVal = _scratch & ((uint64_t(1) << bits) - 1);

		_scratch >>= bits;
		_scratchBits -= bits;

		return returnVal;
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