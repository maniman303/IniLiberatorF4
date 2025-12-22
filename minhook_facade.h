#pragma once

#include "pch.h"
#include <MinHook.h>
#include <string>

namespace minhook_facade
{
	bool init()
	{
		if (MH_Initialize() != MH_OK)
		{
			logger::info("Could not initialize MinHook.");
			return false;
		}

		return true;
	}

	void enable()
	{
		if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		{
			logger::info("Could not enable hooks.");
			return;
		}

		logger::info("MinHook initialization finished.");
	}
}