#pragma once
#include <unordered_map>

class WindowsMessageMap
{
public:
	WindowsMessageMap();
	std::string Get(DWORD msg) const;

private:
	std::unordered_map<DWORD, std::string> map;
};

