#ifndef _SERVER_H_
#define _SERVER_H_

#include "asio\asio.hpp"

#include "NetMessage.h"

#include <deque>

using asio::ip::udp;

class Server
{
public:
	Server(short port)
		: io_service_(), socket_(io_service_, udp::endpoint(udp::v4(), port))
	{
		do_receive();
	}
	void run()
	{
		io_service_.run();
	}

	void do_receive()
	{
		socket_.async_receive_from(
			asio::buffer(_receiveData, max_length), sender_endpoint_,
			[this](asio::error_code ec, std::size_t bytes_recvd)
		{
			if (!ec && bytes_recvd > 0)
			{
				messages.insert(messages.end(), _receiveData, _receiveData + bytes_recvd);

				message = static_cast<Message>(_receiveData[0]);

				if (message == Message::NET_PING)
				{
					do_send(bytes_recvd);
				}
				received = true;
				address = sender_endpoint_.address().to_string();
			}
			//else
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
			//do_receive();
		});
	}

private:
	asio::io_service io_service_;

	udp::socket socket_;
	udp::endpoint sender_endpoint_;
	enum { max_length = 1024 };
	char _receiveData[max_length];
	char _sendData[max_length];
	std::deque<byte> messages;
public:
	bool received = false;
	Message message = Message::NET_NULL;
	std::string address;
};
#endif