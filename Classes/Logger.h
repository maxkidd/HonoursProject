#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "ImGUI\imgui.h"

#include <chrono>

enum _logType
{
	LOG_PACKET_RECEIVED,
	LOG_PACKET_SENT,
	LOG_PACKET_ACK,
	LOG_ERROR
};
#define COLOUR_RED		ImVec4(1.0f, 0.3f, 0.3f, 1.0f)
#define COLOUR_GREEN	ImVec4(0.3f, 1.0f, 0.3f, 1.0f)
#define COLOUR_PURPLE	ImVec4(0.5f, 0.3f, 0.5f, 1.0f)
#define COLOUR_BLUE		ImVec4(0.3f, 0.3f, 1.0f, 1.0f)
#define COLOUR_WHITE	ImVec4(0.3f, 0.3f, 0.3f, 1.0f)
#define COLOUR_BLACK	ImVec4(1.0f, 1.0f, 1.0f, 1.0f)

static ImVec4 LOG_COLOR(_logType type)
{
	switch (type)
	{
	case LOG_PACKET_RECEIVED:
		return COLOUR_GREEN;
	case LOG_PACKET_SENT:
		return COLOUR_BLUE;
	case LOG_PACKET_ACK:
		return COLOUR_PURPLE;
	case LOG_ERROR:
		return COLOUR_RED;
	default:
		return COLOUR_WHITE;
	}
}

struct NetworkLog
{
	struct BandwidthData
	{
		bool _isSending;
		int _bytes;

		BandwidthData(bool sending, int bytes) :_isSending(sending), _bytes(bytes) {}
	};

	static NetworkLog* getInstance();
	static void destroyInstance();

	ImGuiTextBuffer     Buf;

	//ImVector<std::pair<_logType, ImGuiTextBuffer>>	LogType;
	ImGuiTextFilter _filter;
	ImVector<ImVec4> _typeOffsets;        // Index to colour offset
	ImVector<int>       _lineOffsets;        // Index to lines offset
	ImVector<BandwidthData>       o_bandwidthData;
	ImVector<BandwidthData>       i_bandwidthData;

	std::chrono::high_resolution_clock::time_point _lastSecond;
	ImVector<BandwidthData>       o_bandwidthTimeData;
	ImVector<BandwidthData>       i_bandwidthTimeData;

	bool _scrollToBottom;
	int display_count = 70;
	int func_type = 0;
	bool colour = false;

	struct LogView
	{
		static float Sin(void*, int i) { return sinf(i * 0.1f); }
		static float Saw(void*, int i) { return (i & 1) ? 1.0f : 0.0f; }

		static bool paused;// Toggle for histogram view

		static float bandwidth(void* data, int i) {
			if (i == 0)
				return 0.0f;

			auto bandwidthData = (ImVector<BandwidthData>*)data;

			static int currentStart;
			if (!paused)
				currentStart = bandwidthData->size();

			return (*bandwidthData)[currentStart - i]._bytes;
		}
		static float bandwidthKbps(void* data, int i) {
			if (i == 0)
				return 0.0f;

			auto bandwidthData = (ImVector<BandwidthData>*)data;

			static int currentStart;
			if (!paused)
				currentStart = bandwidthData->size();

			return (float)(*bandwidthData)[currentStart - i]._bytes / 1024.0f;
		}
	};


	NetworkLog() 
	{
		o_bandwidthTimeData.push_back(BandwidthData(true, 0));
		i_bandwidthTimeData.push_back(BandwidthData(false, 0));

		i_bandwidthData.reserve(100);
		o_bandwidthData.reserve(100);
	}
	void    Clear() {
		Buf.clear(); _lineOffsets.clear(); _typeOffsets.clear();
	}
	void    TogglePause() {
		LogView::paused = !LogView::paused;
	}

	void    AddLog(_logType type, const char* fmt, ...) IM_PRINTFARGS(2)
	{


		int old_size = Buf.size();
		va_list args;
		va_start(args, fmt);
		Buf.appendv(fmt, args);
		va_end(args);
		for (int new_size = Buf.size(); old_size < new_size; old_size++)
			if (Buf[old_size] == '\n')
			{
				_lineOffsets.push_back(old_size);
				_typeOffsets.push_back(LOG_COLOR(type));
			}
		_scrollToBottom = true;
	}
	void    AddPacketLog(_logType type, bool isSending, int bytes)
	{
		auto time = std::chrono::high_resolution_clock::now();
		if (((std::chrono::duration<float>)(time - _lastSecond)).count() > 1.0f)
		{
			//o_bandwidthTimeData.back()._bytes /= 1024;
			//i_bandwidthTimeData.back()._bytes /= 1024;

			_lastSecond = time;
			o_bandwidthTimeData.push_back(BandwidthData(true, 0));
			i_bandwidthTimeData.push_back(BandwidthData(false, 0));
		}

		if (isSending)
		{
			o_bandwidthData.push_back(BandwidthData(isSending, bytes));
			o_bandwidthTimeData.back()._bytes += bytes;
		}
		else
		{
			i_bandwidthData.push_back(BandwidthData(isSending, bytes));
			i_bandwidthTimeData.back()._bytes += bytes;
		}
		
	}

	void    Draw(const char* title, bool* p_opened = NULL)
	{
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiSetCond_FirstUseEver);
		ImGui::Begin(title, p_opened);
		if (ImGui::Button("Clear")) Clear();
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		if(ImGui::Button("Colour"))
			colour = !colour;
		ImGui::SameLine();
		_filter.Draw("Filter", -100.0f);
		ImGui::SameLine();
		if (ImGui::Button(LogView::paused ? "Unpause" : "Pause")) TogglePause();


		ImGui::Separator();

		ImGui::PushItemWidth(100); ImGui::Combo("Packet", &func_type,
			"Incoming Packets\0Outgoing Packets\0Incoming kbps\0Outgoing kbps\0"); ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::SliderInt("Sample count", &display_count, 1, 500);

		if(func_type == 0)
			ImGui::PlotHistogram("Incoming packets", LogView::bandwidth, (void*)&i_bandwidthData, 
				i_bandwidthData.size() > display_count ? display_count : i_bandwidthData.size() + 1, 0, NULL, -1.0f, 1500.0f, ImVec2(0, 80.0f));
		else if (func_type == 1)
			ImGui::PlotHistogram("Outgoing packets", LogView::bandwidth, (void*)&o_bandwidthData,
				o_bandwidthData.size() > display_count ? display_count : o_bandwidthData.size() + 1, 0, NULL, -1.0f, 1500.0f, ImVec2(0, 80.0f));
		else if (func_type == 2)
			ImGui::PlotLines("Incoming kbps", LogView::bandwidthKbps, (void*)&i_bandwidthTimeData,
				i_bandwidthTimeData.size() > display_count ? display_count : i_bandwidthTimeData.size(), 0.0f, "kbps", 1.0f, 100.0f, ImVec2(0, 80));
		else
			ImGui::PlotLines("Outgoing kbps", LogView::bandwidthKbps, (void*)&o_bandwidthTimeData,
				o_bandwidthTimeData.size() > display_count ? display_count : o_bandwidthTimeData.size(), 0.0f, "kbps", 1.0f, 100.0f, ImVec2(0, 80));

		ImGui::Separator();

		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
		if (copy) ImGui::LogToClipboard();

		if (_filter.IsActive())
		{
			const char* buf_begin = Buf.begin();
			const char* line = buf_begin;
			for (int line_no = 0; line != NULL; line_no++)
			{

				const char* line_end = (line_no < _lineOffsets.Size) ? buf_begin + _lineOffsets[line_no] : NULL;
				if (_filter.PassFilter(line, line_end))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, _typeOffsets[line_no]);
					ImGui::TextUnformatted(line, line_end);
					ImGui::PopStyleColor();
				}
				line = line_end && line_end[1] ? line_end + 1 : NULL;
			}
		}
		else if(colour)
		{
			const char* buf_begin = Buf.begin();
			const char* line = buf_begin;
			for (int line_no = 0; line != NULL; line_no++)
			{

				const char* line_end = (line_no < _lineOffsets.Size) ? buf_begin + _lineOffsets[line_no] : NULL;
				if (_typeOffsets.size() > line_no)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, _typeOffsets[line_no]);
					ImGui::TextUnformatted(line, line_end);
					ImGui::PopStyleColor();
				}
				line = line_end && line_end[1] ? line_end + 1 : NULL;
			}
			//ImGui::TextUnformatted(Buf.begin());
		}
		else
		{
			ImGui::TextUnformatted(Buf.begin());
		}

		if (_scrollToBottom)
			ImGui::SetScrollHere(1.0f);
		_scrollToBottom = false;

		ImGui::EndChild();

		ImGui::End();
	}
};

#endif