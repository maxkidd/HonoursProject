#ifndef _NETWORK_CLIENTSERVER_
#define _NETWORK_CLIENTSERVER_


#include "network_simulation.h"
#include "network_transport.h"
#include "network_connection.h"

#include "network_common.h"

class Client
{
public:
	enum ClientState
	{
		CLIENT_SLEEP, // Not active
		CLIENT_REQUESTING, // Requesting connection with server
		CLIENT_REQUEST_DENIED, // Request denied
		CLIENT_CONNECTED // Connected after request accepted
	} _state;
protected:

	NetworkSimulation* _simulation = nullptr;// Simulation layer
	BaseTransport* _transport = nullptr;// Transport layer
	Connection* _connection = nullptr;// Connection layer

	bool _active = false;// false by default until connected

	// Server info
	const char* _serverIP = nullptr;
	const char* _serverPort = nullptr;
	udp::endpoint _serverEndpoint;

public:
	Client(NetworkSimulation* simulation, BaseTransport* transport);
	virtual ~Client();

	void Connect(const char* ip = "localhost", const char* port = "1500");

	bool IsActive() { return _active; }


	/* Overrides */
	virtual PacketFactory* GetPacketFactory() = 0;
	virtual MessageFactory* GetMessageFactory() = 0;
	virtual void ProcessMessages() = 0;


	// Connection layer
	void SendPackets();
	void ReceivePackets();

	// Transport layer
	void WritePackets();
	void ReadPackets();
	
	std::string GetNetworkState();
protected:

	void ProcessPacket(Packet* packet, const udp::endpoint& endpoint);
	void ProcessAcceptPacket(ConnectionAcceptPacket* packet, const udp::endpoint& endpoint);
	void ProcessDeniedPacket(ConnectionDeniedPacket* packet, const udp::endpoint& endpoint);
	void ProcessConnectionPacket(ConnectionPacket* packet, const udp::endpoint& endpoint);
	void ProcessDisconnectPacket(ConnectionDisconnectPacket* packet, const udp::endpoint& endpoint);

	Packet* CreateRequestPacket();
	Packet* CreateConnectionPacket();

};

class Server
{
protected:
	enum ServerState
	{
		SERVER_SLEEP, // Not active
		SERVER_ALIVE, // Running and can accept clients
		SERVER_CONNECTED // Running with 1 or more clients connected
	} _state;

	NetworkSimulation* _simulation = nullptr;
	BaseTransport* _transport = nullptr;


	static const int MAX_SLOTS = 32;
	static const int NULL_CLIENT_ID = MAX_SLOTS;

	bool _active = false;

	uint16_t _connectedClients = 0;
	
	// Clients
	bool _clientConnected[MAX_SLOTS] = { false };
	//ClientData _clientData[MAX_SLOTS]; // NOT NEEEDED
	Connection* _connections[MAX_SLOTS];
public:
	Server(NetworkSimulation* simulation, BaseTransport* transport);
	virtual ~Server();

	bool IsActive() { return _active; }


	virtual PacketFactory* GetPacketFactory() = 0;
	virtual MessageFactory* GetMessageFactory() = 0;
	virtual void GenerateMessages() = 0;
	virtual void ProcessMessages() = 0;


	// Connection layer
	void SendPackets();
	void ReceivePackets();

	// Transport layer
	void WritePackets();
	void ReadPackets();

	std::string GetNetworkState();

protected:
	void ProcessPacket(Packet* packet, udp::endpoint endpoint);
	/* Process connection request packet from client */
	void ProcessRequestPacket(ConnectionRequestPacket* packet, const udp::endpoint& endpoint);
	void ProcessConnectionPacket(ConnectionPacket* packet, const udp::endpoint& endpoint);
	void ProcessDisconnectPacket(ConnectionDisconnectPacket* packet, const udp::endpoint& endpoint);

	void SendPacketToClient(uint16_t clientID, Packet* packet);
};


#endif