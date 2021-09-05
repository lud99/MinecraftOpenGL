#pragma once

#include <string>

#include <iostream>
#include <map>
#include <mutex>
#include <Common/json.hpp>

class ConsoleEntry
{
public:
	ConsoleEntry() {};

	std::string stringData;

	std::map<std::string, std::vector<ConsoleEntry>> children;
};

ConsoleEntry& operator<< (ConsoleEntry& left, const char* right);
ConsoleEntry& operator<< (ConsoleEntry& left, const std::string& right);
ConsoleEntry& operator<< (ConsoleEntry& left, const json& right);
ConsoleEntry& operator<< (ConsoleEntry& left, const int right);
ConsoleEntry& operator<< (ConsoleEntry& left, const float right);

class Console
{
public:
	static ConsoleEntry& Log(std::string category, std::string title = "");

	static void Render();

	static const int MaxLength = 200;

	static std::mutex m_Mutex;

private:

	// The key is the logging category 
	static std::map<std::string, ConsoleEntry> m_LogEntries; //std::map<std::string, std::map<std::string, std::vector<ConsoleEntry>>> m_LogEntries;
};