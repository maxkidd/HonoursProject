#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "asio\asio.hpp"
#include "NetMessage.h"
#include <chrono>
using asio::ip::udp;

class Client
{
public:
	Client(short port)
		: io_service_(), socket_(io_service_, udp::endpoint(udp::v4(), 0))
	{
		udp::resolver resolver(io_service_);
		sender_endpoint_ = *resolver.resolve({ udp::v4(), "localhost", "1500" });

		do_receive();
	}
	void run()
	{
		io_service_.run();
	}

	void sendMessage(const char* message, size_t length)
	{
		Message mess = Message::NET_TEXT;
		memcpy(&_sendData[_sendDataLength], &mess, sizeof(Message));
		_sendDataLength += sizeof(Message);

		//_sendData[_sendDataLength] = length;
		memcpy(&_sendData[_sendDataLength], &length, sizeof(size_t));
		_sendDataLength += sizeof(size_t);

		strncpy(&_sendData[_sendDataLength], message, length);
		_sendDataLength += length;

		do_send(_sendDataLength);
	}
	void update()
	{
		//_sendData[_sendDataLength] = Message::NET_PING;
		Message mess = Message::NET_PING;
		memcpy(&_sendData[_sendDataLength], &mess, sizeof(Message));
		_sendDataLength += sizeof(Message);
		
		start = std::chrono::high_resolution_clock::now();

		do_send(_sendDataLength);
	}
	void initConnection()
	{
		_sendData[_sendDataLength] = Message::NET_PING;
		_sendDataLength += sizeof(Message);

		start = std::chrono::high_resolution_clock::now();

		do_send(_sendDataLength);
	}

	void do_receive()
	{
		socket_.async_receive_from(
			asio::buffer(_receiveData, max_length), sender_endpoint_,
			[this](asio::error_code ec, std::size_t bytes_recvd)
		{
			if (!ec && bytes_recvd > 0)
			{
				message = static_cast<Message>(_receiveData[0]);

				if (message == Message::NET_PING)
				{
					end = std::chrono::high_resolution_clock::now();
					latency = std::chrono::duration_cast<std::chrono::milliseconds>(end -
						start).count();
				}
				received = true;
				address = sender_endpoint_.address().to_string();
			}
			else
			{
				do_receive();
			}
		});
	}

	void do_send(std::size_t length)
	{
		socket_.async_send_to(
			asio::buffer(_sendData, length), sender_endpoint_,
			[this](asio::error_code /*ec*/, std::size_t /*bytes_sent*/)
		{
			sent = true;
			_sendDataLength = 0;
			do_receive();
		});
	}

private:
	asio::io_service io_service_;

	udp::socket socket_;
	udp::endpoint sender_endpoint_;
	enum { max_length = 1024 };
	char _receiveData[max_length];
	char _sendData[max_length];
	unsigned int _sendDataLength = 0;
	int size = 0;

	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

public:
	bool sent = false;
	bool received = false;
	std::string address;
	Message message = NET_NULL;
	long long latency = 0;

};
#endif