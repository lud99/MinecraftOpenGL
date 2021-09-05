#include "DebugConsole.h"

#ifndef SERVER_BUILD
#include <imgui.h>
#endif

#include <thread>
#include <mutex>

#include <iostream>

ConsoleEntry& operator<<(ConsoleEntry& left, const char* right)
{
	//std::lock_guard<std::mutex> log(m_Mutex);

	left.stringData += std::string(right);

	return left;
}

ConsoleEntry& operator<< (ConsoleEntry& left, const std::string& right)
{
	//std::lock_guard<std::mutex> log(m_Mutex);

	if (right.length() > Console::MaxLength)
		left.stringData += right.substr(0, Console::MaxLength) + "...";
	else
		left.stringData += right;

	return left;
}

ConsoleEntry& operator<<(ConsoleEntry& left, const json& right)
{
	//std::lock_guard<std::mutex> log(m_Mutex);

	std::string str = right.dump();
	if (str.length() > Console::MaxLength)
		left.stringData += str.substr(0, Console::MaxLength) + "...";
	else
		left.stringData += str;

	return left;
}

ConsoleEntry& operator<<(ConsoleEntry& left, const int right)
{
	//std::lock_guard<std::mutex> log(m_Mutex);

	left.stringData += std::to_string(right);

	return left;
}

ConsoleEntry& operator<<(ConsoleEntry& left, const float right)
{
	//std::lock_guard<std::mutex> log(m_Mutex);

	left.stringData += std::to_string(right);

	return left;
}

ConsoleEntry& Console::Log(std::string category, std::string title)
{
	std::lock_guard<std::mutex> log(Console::m_Mutex);

	ConsoleEntry e;
	std::vector<ConsoleEntry>& entries = m_LogEntries[category].children[title];
	entries.push_back(e);

	return entries.back();
}

void Console::Render()
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	// Display the logs with ImGui
#ifndef SERVER_BUILD
	ImGui::Begin("Console");

	int tabPadding = 20;
	int depth = 0;

	for (auto& e1 : m_LogEntries)
	{
		std::string category = e1.first;
		auto& entriesForCategory = e1.second.children;

		if (category == "" || ImGui::CollapsingHeader(category.c_str()))
		{
			if (category != "") depth++;

			for (auto& e2 : entriesForCategory)
			{
				std::string title = e2.first;
				std::vector<ConsoleEntry>& logs = e2.second;

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + tabPadding * depth);

				if (title == "" || ImGui::CollapsingHeader(title.c_str()))
				{
					if (category != "") depth++;

					for (int i = 0; i < logs.size(); i++)
					{
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + tabPadding * depth);

						ImGui::Text(logs[i].stringData.c_str());
					}
				}
			}

			depth--;
			if (depth < 0) depth = 0;
		}

		depth = 0;
	}

	ImGui::End();
#else
	// Print with std::cout
	
	int tabPadding = 20;
	int depth = 0;

	for (auto& e1 : m_LogEntries)
	{
		std::string category = e1.first;
		auto& entriesForCategory = e1.second.children;

		if (category != "")
			std::cout << category << " -> ";

		for (auto& e2 : entriesForCategory)
		{
			std::string title = e2.first;
			std::vector<ConsoleEntry>& logs = e2.second;

			if (title != "")
				std::cout << title << " -> ";

			for (int i = 0; i < logs.size(); i++)
			{
				std::cout << logs[i].stringData << "\n";
			}
		}
	}

	// Clear the logs 
	m_LogEntries.clear();

#endif
}

std::mutex Console::m_Mutex;
std::map<std::string, ConsoleEntry> Console::m_LogEntries;