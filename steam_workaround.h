#pragma once

#include "pch.h"
#include <MinHook.h>
#include <string>

namespace steam_workaround
{
	bool isGetISteamUtilsHooked = false;

	using GetISteamUtils_t = void* (__fastcall*)(void* _this, int32_t hPipe, const char* version);

	GetISteamUtils_t fpGetISteamUtils;

	void* __fastcall DetourGetISteamUtils(void* _this, int32_t hPipe, const char* version)
	{
		auto utils = fpGetISteamUtils(_this, hPipe, version);

		logger::info("Creating ISteamUtils.");

		return utils;
	}

	using CreateInterface_t = void* (__fastcall*)(const char* version);

	CreateInterface_t fpCreateInterface;

	void* __fastcall DetourCreateInterface(const char* version)
	{
		void* iface = fpCreateInterface(version);

		if (!isGetISteamUtilsHooked && version && strstr(version, "SteamClient020"))
		{
			isGetISteamUtilsHooked = true;

			void** vtable = *(void***)iface;
			int index = 14;

			DWORD old;
			VirtualProtect(&vtable[index], sizeof(void*),
				PAGE_EXECUTE_READWRITE, &old);

			fpGetISteamUtils =
				(GetISteamUtils_t)vtable[index];
			vtable[index] = DetourGetISteamUtils;

			VirtualProtect(&vtable[index], sizeof(void*), old, &old);
		}

		return iface;
	}

	void hook(HMODULE steam)
	{
		auto addr = GetProcAddress(steam, "SteamInternal_CreateInterface");

		if (MH_CreateHookEx(addr, &DetourCreateInterface, &fpCreateInterface) != MH_OK)
		{
			logger::info("Could not hook Steam CreateInterface.");
			return;
		}

		logger::info("Hooked Steam CreateInterface.");
	}

	void init()
	{
		HMODULE steam = LoadLibraryA("steam_api64.dll");

		if (steam)
		{
			hook(steam);
		}
	}
}