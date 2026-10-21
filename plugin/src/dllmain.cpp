// Entry point. Ultimate ASI Loader calls LoadLibrary on LivingWar.asi inside GhostOfTsushima.exe.
#include <windows.h>

#include <cwchar>

#include "plugin.hpp"

namespace {
bool isGameProcess() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    const wchar_t* name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    return _wcsicmp(name, L"GhostOfTsushima.exe") == 0;
}
} // namespace

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(module);
        if (!isGameProcess()) return TRUE;   // stay inert anywhere else
        HANDLE t = CreateThread(nullptr, 0, lwp::pluginThread, module, 0, nullptr);
        if (t) CloseHandle(t);
    } else if (reason == DLL_PROCESS_DETACH) {
        lwp::requestStop();   // no saving inside the loader lock; every event already autosaves
    }
    return TRUE;
}
