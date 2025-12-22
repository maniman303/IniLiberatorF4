#include "pch.h"
#include "steam_workaround.h"
#include <windows.h>

void steam_workaround::init()
{
	logger::info("Started looking for steam module.");

	HMODULE steam = NULL;

	steam = GetModuleHandleA("steam_api64.dll");

	/*while (!steam)
	{
		Sleep(100);

		steam = GetModuleHandleA("steam_api64.dll");
	}

	logger::info("Steam module found!");*/
}