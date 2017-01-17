#ifndef _NETWORK_CONNECTION_H_
#define _NETWORK_CONNECTION_H_


class ConnectionPacket;

class Connection
{
public:
	Connection();
	~Connection();

	ConnectionPacket* GeneratePacket();
protected:
private:
	//QUEUE(MESSAGES) sentqueue
	//queue(MESSAGES) receivedqueue

};

#endif