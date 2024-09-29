#pragma once 

#include <wtypes.h>
#include <stdint.h>
#include <sddl.h>
#include <iostream>
#include <TlHelp32.h>
#define Alpine_OverwritePtr RegSetValueExA 

enum requests
{
	REQUEST_SETTINGS
};

struct Minimalist_CMD
{
	int magic_code;
	uintptr_t request;
	bool AudioChange = false;
	bool DesktopFile = false;
	bool Geolocation = false;
	bool TimeZone = false;
};

class Minimalist
{
private:

	HKEY targetKey;
	bool anti_duplicate;
	std::atomic<bool> anti_overflow = false;
	Minimalist_CMD instructions;

	void make_request()
	{
		if (!anti_overflow)
		{
			anti_overflow = true;
			void* val = &instructions;
			RegSetValueExA(targetKey,
				("MuiInit"), 0, REG_QWORD,
				reinterpret_cast<BYTE*>(&val), sizeof(uint64_t));
			anti_overflow = false;
		}

	}

public:

	Minimalist() {};

	bool InitComm()
	{
		RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\CMF", 0, KEY_ALL_ACCESS, &targetKey); // opens a registry key
		anti_duplicate = true;
		return true;
	}

	void CloseComm()
	{
		RegCloseKey(targetKey);
	}

	void ApplyChanges(bool one, bool two, bool three, bool four)
	{
		instructions.magic_code = 1337;
		instructions.request = REQUEST_SETTINGS;
		instructions.AudioChange = one;
		instructions.DesktopFile = two;
		instructions.Geolocation = three;
		instructions.TimeZone = four;
		//instructions.pBuffer = &buffer;

		make_request();
	}
};
inline Minimalist* Minimalist_Driver = new Minimalist;
