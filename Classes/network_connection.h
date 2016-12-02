#ifndef _NETWORKCONNECTION_H_
#define _NETWORKCONNECTION_H_

#include "network_packet.h"


class ConnectionPacket : public Packet
{
	
};

class Connection
{
public:
	Connection();
	~Connection();

	ConnectionPacket* GeneratePacket();
protected:
private:


};

#endif