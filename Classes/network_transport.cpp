#include "network_transport.h"

bool SocketTransport::InternalReceivePacket(udp::endpoint & endpoint, void * data, int bytes)
{
	/*socket_.async_receive_from(
		asio::buffer(_receiveData, max_length), endpoint,
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
	});*/
	int result = socket_.receive_from(asio::buffer(data, bytes), endpoint);

	if(result <= 0)
	return false;
}

bool SocketTransport::InternalSendPacket(const udp::endpoint & endpoint, const void * data, int size)
{
	//socket_.async_send_to(
	//	asio::buffer(data, size), endpoint,
	//	[this](asio::error_code /*ec*/, std::size_t /*bytes_sent*/)
	//{
	//	sent = true;
	//	_sendDataLength = 0;
	//	do_receive();
	//});
	socket_.send_to(asio::buffer(data, size), endpoint);
	return true;
}
