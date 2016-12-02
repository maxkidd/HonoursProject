#ifndef _NETMESSAGE_H_
#define _NETMESSAGE_H_

enum Message
{
	NET_NULL,
	NET_INIT_BEGIN,
	NET_INIT_RESPONSE,
	NET_INIT_FINALISE,
	NET_PING,
	NET_TEXT
};

struct M_InitBegin
{

};

struct M_InitResponse
{

};
struct M_InitFinalise
{

};
struct M_Ping
{

};

#endif