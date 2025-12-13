#include "pch.h"
#include <MinHook.h>
#include <string>
#include <format>
#include <iostream>
#include <filesystem>
#include <unordered_set>

namespace fs = std::filesystem;

namespace iniworkaround
{
	std::unordered_set<std::string> iniFiles;

	// GetPrivateProfileStringA
	typedef DWORD(WINAPI* GetPrivateProfileStringA)(LPCSTR, LPCSTR, LPCSTR, LPSTR, DWORD, LPCSTR);

	GetPrivateProfileStringA fpGetPrivateProfileStringA = NULL;

	DWORD DetourGetPrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName)
	{
		std::string fileName = to_lower(lpFileName);
		if (!fileName.starts_with("data\\") || fileName.find("f4se") != std::string::npos)
		{
			return fpGetPrivateProfileStringA(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
		}

		fs::path filePath{ fileName };

		if (!iniFiles.contains(filePath.filename().string()))
		{
			SetLastError(ERROR_FILE_NOT_FOUND);

			return copy_string(lpDefault, lpReturnedString, nSize);
		}

		// logger::info(std::format("GetPrivateProfileStringA file [{0}] key [{1}].", lpFileName, lpKeyName));

		return fpGetPrivateProfileStringA(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
	}

	// GetPrivateProfileIntA
	typedef UINT(WINAPI* GetPrivateProfileIntA)(LPCSTR, LPCSTR, INT, LPCSTR);

	GetPrivateProfileIntA fpGetPrivateProfileIntA = NULL;

	UINT DetourGetPrivateProfileIntA(LPCSTR lpAppName, LPCSTR lpKeyName, INT nDefault, LPCSTR lpFileName)
	{
		std::string fileName = to_lower(lpFileName);
		if (!fileName.starts_with("data\\") || fileName.find("f4se") != std::string::npos)
		{
			return fpGetPrivateProfileIntA(lpAppName, lpKeyName, nDefault, lpFileName);
		}

		fs::path filePath{ fileName };

		if (!iniFiles.contains(filePath.filename().string()))
		{
			SetLastError(ERROR_FILE_NOT_FOUND);

			return static_cast<UINT>(nDefault);
		}

		// logger::info(std::format("GetPrivateProfileIntA file [{0}] key [{1}].", lpFileName, lpKeyName));

		return fpGetPrivateProfileIntA(lpAppName, lpKeyName, nDefault, lpFileName);
	}

	static void loadIniFiles()
	{
		fs::path dataDir = fs::current_path() / "data";

		// logger::info(std::format("Current path: {0},", fs::current_path().string()));

		if (!fs::exists(dataDir)) {
			logger::info("Data folder does not exists!");
			return;
		}

		if (!fs::is_directory(dataDir)) {
			logger::info("Data is not a folder!");
			return;
		}

		for (const auto& entry : fs::directory_iterator(dataDir)) {
			if (entry.is_regular_file() && to_lower(entry.path().extension().string()) == ".ini") {
				iniFiles.insert(to_lower(entry.path().filename().string()));
			}
		}

		logger::info(std::format("Loaded {0} ini files.", iniFiles.size()));
	}

	static void init()
	{
		logger::info("Start INI workaround.");

		if (MH_Initialize() != MH_OK)
		{
			logger::info("Could not initialize MinHook.");
			return;
		}

		HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
		FARPROC procGetPrivateProfileStringA = GetProcAddress(kernel32, "GetPrivateProfileStringA");

		if (MH_CreateHookEx(procGetPrivateProfileStringA, &DetourGetPrivateProfileStringA, &fpGetPrivateProfileStringA) != MH_OK)
		{
			logger::info("Could not hook GetPrivateProfileStringA.");
			return;
		}

		logger::info("Hooked GetPrivateProfileStringA.");

		FARPROC procGetPrivateProfileIntA = GetProcAddress(kernel32, "GetPrivateProfileIntA");

		if (MH_CreateHookEx(procGetPrivateProfileIntA, &DetourGetPrivateProfileIntA, &fpGetPrivateProfileIntA) != MH_OK)
		{
			logger::info("Could not hook GetPrivateProfileIntA.");
			return;
		}

		logger::info("Hooked GetPrivateProfileIntA.");

		logger::info("Hooked GetPrivateProfileSectionNamesA.");

		if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		{
			logger::info("Could not enable hooks.");
			return;
		}

		logger::info("MinHook initialization finished.");

		loadIniFiles();
	}
}