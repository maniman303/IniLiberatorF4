## IniLiberatorF4
Liberate your Fallout 4 installation from long startup caused by ini loads!

This `winmm.dll` proxy will skip load of ini files for plugins that do not have ini files (which is most of them). This should massively improve start up times on mod heavy setups.

On my Steam Deck, without it, start took 1 min 46 sec! With the mod I see intro video in 13 sec.

### Build
Requires VCPKG and MinHook installed: `.\vcpkg install minhook:x64-windows-static-md`.