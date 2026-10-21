#pragma once
#include <windows.h>

namespace lwp {
// Called once from DllMain (on a new thread) when loaded into GhostOfTsushima.exe.
DWORD WINAPI pluginThread(LPVOID module);
void requestStop();
} // namespace lwp
