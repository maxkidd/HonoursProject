#ifndef _NETWORK_CONNECTION_H_
#define _NETWORK_CONNECTION_H_

#include "asio\asio.hpp"

class ConnectionPacket;

using namespace asio::ip;

class Connection
{
public:
	Connection(udp::endpoint endpoint);
	~Connection();

	ConnectionPacket* GeneratePacket();

	udp::endpoint Endpoint();
protected:
private:
	//QUEUE(MESSAGES) sentqueue
	//queue(MESSAGES) receivedqueue

	udp::endpoint _endpoint;
};

#endif