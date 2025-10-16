/*
  ==============================================================================

    CefLoader.cpp
    Created: 16 Oct 2025 8:06:02am
    Author:  MineEric64

  ==============================================================================
*/

#include "CefLoader.h"
#include <JuceHeader.h>
#define DEBUG(message, title) juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::InfoIcon, title, message)

class CefUnloadDelay : public juce::Timer
{
public:
    CefUnloadDelay(HMODULE h) : handleToFree(h) {
        startTimer(1000);
    }

private:
    HMODULE handleToFree;

    void timerCallback() override
    {
        stopTimer();

        if (handleToFree)
        {
            FreeLibrary(handleToFree);
        }
        delete this;
    }
};

CefLoader::CefLoader() {

}

CefLoader::~CefLoader() {
    if (shutdownCEF != NULL) shutdownCEF();
    free();
}

bool CefLoader::init(const wchar_t* dirPath, const wchar_t* dllPath) {
    if (handle != NULL) return false;

    BOOL success = SetDllDirectoryW(dirPath);

    if (success) {
        DEBUG(juce::String(dirPath), "OKAY");
    }
    else {
        DEBUG(juce::String(dirPath), "HMM");
        return false;
    }

    handle = LoadLibraryW(dllPath);
    DEBUG(juce::String(dllPath), handle != NULL ? "1" : "0");
    
    if (handle == NULL) return false;

    initializeCEF = (INITIALIZE_CEF)GetProcAddress(handle, "initializeCEF");
    shutdownCEF = (SHUTDOWN_CEF)GetProcAddress(handle, "shutdownCEF");
    isInitialized = (IS_INITIALIZED)GetProcAddress(handle, "isInitialized");
    resized = (RESIZED)GetProcAddress(handle, "resized");
    loadURL = (LOAD_URL)GetProcAddress(handle, "loadURL");
    timerCallback = (TIMER_CALLBACK)GetProcAddress(handle, "timerCallback");
    setLocalBounds = (SET_LOCAL_BOUNDS)GetProcAddress(handle, "setLocalBounds");
    setAudioParam = (SET_AUDIO_PARAM)GetProcAddress(handle, "setAudioParam");
    mouseMove = (MOUSE_MOVE)GetProcAddress(handle, "mouseMove");
    mouseDown = (MOUSE_DOWN)GetProcAddress(handle, "mouseDown");
    mouseUp = (MOUSE_UP)GetProcAddress(handle, "mouseUp");
    getImage = (GET_IMAGE)GetProcAddress(handle, "getImage");
    getAudioBuffer = (GET_AUDIO_BUFFER)GetProcAddress(handle, "getAudioBuffer");

    return true;
}

void CefLoader::free() {
    HMODULE handleToFree = handle;
    handle = NULL;
    resized = NULL;
    shutdownCEF = NULL;
    getImage = NULL;
    getAudioBuffer = NULL;
    mouseMove = NULL;
    timerCallback = NULL;

    new CefUnloadDelay(handleToFree);
}

void CefLoader::kill() {
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (strcmp(pEntry.szExeFile, "layby_subwoofer.exe") == 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                (DWORD)pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
}