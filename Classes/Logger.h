#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "ImGUI\imgui.h"

enum _logType
{
	LOG_PACKET_RECEIVED,
	LOG_PACKET_SENT,
	LOG_PACKET_ACK,
	LOG_ERROR
};
#define COLOUR_RED		ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define COLOUR_GREEN	ImVec4(0.0f, 1.0f, 0.0f, 1.0f)
#define COLOUR_PURPLE	ImVec4(0.5f, 0.0f, 0.5f, 1.0f)
#define COLOUR_BLUE		ImVec4(0.0f, 0.0f, 1.0f, 1.0f)
#define COLOUR_WHITE	ImVec4(0.0f, 0.0f, 0.0f, 1.0f)
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
	ImGuiTextFilter Filter;
	ImVector<ImVec4> typeOffsets;        // Index to colour offset
	ImVector<int>       LineOffsets;        // Index to lines offset
	ImVector<BandwidthData>       o_bandwidthData;
	ImVector<BandwidthData>       i_bandwidthData;
	bool                ScrollToBottom;
	int display_count = 70;
	int func_type = 0;

	struct LogView
	{
		static float Sin(void*, int i) { return sinf(i * 0.1f); }
		static float Saw(void*, int i) { return (i & 1) ? 1.0f : 0.0f; }

		static bool paused;// Toggle for histogram view

		static float bandwidth(void* data, int i) { 
			static int currentStart;
			ImVector<BandwidthData>* vec = (ImVector<BandwidthData>*)data;
			if (!paused)
				currentStart = vec->size();

			return (*(ImVector<BandwidthData>*)data)[currentStart - i]._bytes;
		}
	};


	NetworkLog() 
	{
		i_bandwidthData.reserve(100);
		o_bandwidthData.reserve(100);
	}
	void    Clear() {
		Buf.clear(); LineOffsets.clear(); typeOffsets.clear();
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
				LineOffsets.push_back(old_size);
				typeOffsets.push_back(LOG_COLOR(type));
			}
		ScrollToBottom = true;
	}
	void    AddPacketLog(_logType type, bool isSending, int bytes)
	{
		if (isSending)
			o_bandwidthData.push_back(BandwidthData(isSending, bytes));
		else
			i_bandwidthData.push_back(BandwidthData(isSending, bytes));
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
		ImGui::SameLine();
		if (ImGui::Button(LogView::paused ? "Unpause" : "Pause")) TogglePause();


		ImGui::Separator();

		ImGui::PushItemWidth(100); ImGui::Combo("Packet", &func_type, "Incoming\0Outgoing\0"); ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::SliderInt("Sample count", &display_count, 1, 500);

		if(func_type == 0)
			ImGui::PlotHistogram("Incoming packets", LogView::bandwidth, (void*)&i_bandwidthData, 
				i_bandwidthData.size() > display_count ? display_count : i_bandwidthData.size() + 1, 0, NULL, -1.0f, 1000.0f, ImVec2(0, 80.0f));
		else
			ImGui::PlotHistogram("Outgoing packets", LogView::bandwidth, (void*)&o_bandwidthData, 
				o_bandwidthData.size() > display_count ? display_count : o_bandwidthData.size() + 1, 0, NULL, -1.0f, 1000.0f, ImVec2(0, 80.0f));

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
				{
					ImGui::PushStyleColor(ImGuiCol_Text, typeOffsets[line_no]);
					ImGui::TextUnformatted(line, line_end);
					ImGui::PopStyleColor();
				}
				line = line_end && line_end[1] ? line_end + 1 : NULL;
			}
		}
		else
		{
			ImGui::TextUnformatted(Buf.begin());
		}

		if (ScrollToBottom)
			ImGui::SetScrollHere(1.0f);
		ScrollToBottom = false;

		ImGui::EndChild();

		ImGui::End();
	}
};

#endif