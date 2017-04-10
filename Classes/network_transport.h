#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

#include <queue>
#include "asio\asio.hpp"

#include "network_packet.h"
#include "network_message.h"

#include "NetworkDebugDataSource.h"

#include "ImGUI\CCIMGUI.h"

using asio::ip::udp;

#define MAX_SEND_QUEUE (100)
#define MAX_RECEIVE_QUEUE (100)

struct PacketInfo
{
	PacketInfo()
	{
		packet = nullptr;
	}
	udp::endpoint endpoint;
	Packet* packet;
};
enum _logType
{
	LOG
};
struct NetworkLog
{

	ImGuiTextBuffer     Buf;
	ImVector<_logType>	LogType;
	ImGuiTextFilter     Filter;
	ImVector<int>       LineOffsets;        // Index to lines offset
	bool                ScrollToBottom;

	NetworkLog() {}
	void    Clear() { Buf.clear(); LineOffsets.clear(); }

	void    AddLog(const char* fmt, ...) IM_PRINTFARGS(2)
	{
		int old_size = Buf.size();
		va_list args;
		va_start(args, fmt);
		Buf.appendv(fmt, args);
		va_end(args);
		for (int new_size = Buf.size(); old_size < new_size; old_size++)
			if (Buf[old_size] == '\n')
				LineOffsets.push_back(old_size);
		ScrollToBottom = true;
	}

	void    Draw(const char* title, bool* p_opened = NULL)
	{
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiSetCond_FirstUseEver);
		ImGui::Begin(title, p_opened);
		if (ImGui::Button("Clear")) Clear();
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		Filter.Draw("Filter", -100.0f);
		ImGui::Separator();
		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
		if (copy) ImGui::LogToClipboard();

		if (Filter.IsActive())
		{
			const char* buf_begin = Buf.begin();
			const char* line = buf_begin;
			for (int line_no = 0; line != NULL; line_no++)
			{

				const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
				if (Filter.PassFilter(line, line_end))
					ImGui::TextUnformatted(line, line_end);
				line = line_end && line_end[1] ? line_end + 1 : NULL;
			}
		}
		else
		{
			const char* buf_begin = Buf.begin();
			const char* line = buf_begin;
			for (int line_no = 0; line != NULL; line_no++)
			{
				const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
				if (Filter.PassFilter(line, line_end))
					ImGui::TextUnformatted(line, line_end);
				line = line_end && line_end[1] ? line_end + 1 : NULL;
			}

			//ImColor col = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);

			//LogType[2];

			//ImGui::PushStyleColor(ImGuiCol_Text, col);
			ImGui::TextUnformatted(Buf.begin());
			//ImGui::PopStyleColor();
		}

		if (ScrollToBottom)
			ImGui::SetScrollHere(1.0f);
		ScrollToBottom = false;
		ImGui::EndChild();
		ImGui::End();
	}
};
class BaseTransport
{
public:
	BaseTransport(PacketFactory* packetFactory, MessageFactory* messageFactory)
		: _packetFactory(packetFactory), _messageFactory(messageFactory), max_packet_size_(1900)
	{
		_context._packetFactory = packetFactory;
		_context._messageFactory = messageFactory;
	}
	virtual ~BaseTransport() {}

	Packet* CreatePacket();
	Packet* ReceivePacket(udp::endpoint &endpoint);

	void SendPacket(const udp::endpoint & endpoint, Packet * data);

	void WritePackets();
	void ReadPackets();

	//void SetDebugService(NetworkDebugDataSource* dataSource) { _debugData = dataSource; }
	NetworkDebugDataSource* GetDebugService() { return &_debugData; }
	asio::io_service* GetIOService() { return &_IOService; }
protected:
	// Internal receive functions to recieve packets from destination
	virtual int InternalReceivePacket(udp::endpoint & endpoint, void * data, int bytes) = 0;
	// Internal send function to transport packets to a destination
	virtual bool InternalSendPacket(const udp::endpoint & endpoint, const void * data, int size) = 0;

private:
	std::queue<PacketInfo> _sendQueue;
	std::queue<PacketInfo> _receiveQueue;

	PacketFactory* _packetFactory;
	MessageFactory* _messageFactory;

	NetworkDebugDataSource _debugData;
	StreamContext _context;
	int max_packet_size_;
protected:
	asio::io_service _IOService;

	NetworkLog log;

};

class SocketTransport : public BaseTransport
{
public:
	//SocketTransport(PacketFactory* packetFactory);
	SocketTransport(PacketFactory * packetFactory, MessageFactory* messageFactory, unsigned short port = 0);
	virtual ~SocketTransport();

	unsigned short GetPort();
protected:
	virtual int InternalReceivePacket(udp::endpoint & endpoint, void * data, int bytes);
	virtual bool InternalSendPacket(const udp::endpoint & endpoint, const void * data, int size);

private:
	udp::socket _socket;

	// ImGUI
	bool opened = true;
};

#endif
