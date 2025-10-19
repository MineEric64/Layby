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

CefLoader::CefLoader() {

}

CefLoader::~CefLoader() {
    
}

bool CefLoader::init(const wchar_t* dirPath, const wchar_t* dllPath) {
    if (handle != NULL) return false;

    BOOL success = SetDllDirectoryW(dirPath);

    //DEBUG(juce::String(dirPath), "OKAY");
    if (!success) return false;

    handle = LoadLibraryW(dllPath);
    //DEBUG(juce::String(dllPath), handle != NULL ? "1" : "0");
    
    if (handle == NULL) return false;

    initializeCEF = (INITIALIZE_CEF)GetProcAddress(handle, "initializeCEF");
    shutdownCEF = (SHUTDOWN_CEF)GetProcAddress(handle, "shutdownCEF");
    isInitialized = (IS_INITIALIZED)GetProcAddress(handle, "isInitialized");
    resized = (RESIZED)GetProcAddress(handle, "resized");
    loadURL = (LOAD_URL)GetProcAddress(handle, "loadURL");
    getURL = (GET_URL)GetProcAddress(handle, "getURL");
    timerCallback = (TIMER_CALLBACK)GetProcAddress(handle, "timerCallback");
    setLocalBounds = (SET_LOCAL_BOUNDS)GetProcAddress(handle, "setLocalBounds");
    setAudioParam = (SET_AUDIO_PARAM)GetProcAddress(handle, "setAudioParam");
    mouseMove = (MOUSE_MOVE)GetProcAddress(handle, "mouseMove");
    mouseDown = (MOUSE_DOWN)GetProcAddress(handle, "mouseDown");
    mouseUp = (MOUSE_UP)GetProcAddress(handle, "mouseUp");
    keyPressed = (KEY_PRESSED)GetProcAddress(handle, "keyPressed");
    executeJS = (EXECUTE_JS)GetProcAddress(handle, "executeJS");
    showDevTools = (SHOW_DEV_TOOLS)GetProcAddress(handle, "showDevTools");
    getImageSize = (GET_IMAGE_SIZE)GetProcAddress(handle, "getImageSize");
    getImage = (GET_IMAGE)GetProcAddress(handle, "getImage");
    getAudioBuffer = (GET_AUDIO_BUFFER)GetProcAddress(handle, "getAudioBuffer");

    return true;
}

void CefLoader::free() {
    //HMODULE handleToFree = handle;

    handle = NULL;
    initializeCEF = NULL;
    shutdownCEF = NULL;
    isInitialized = NULL;
    resized = NULL;
    loadURL = NULL;
    getURL = NULL;
    timerCallback = NULL;
    setLocalBounds = NULL;
    setAudioParam = NULL;
    mouseMove = NULL;
    mouseDown = NULL;
    mouseUp = NULL;
    keyPressed = NULL;
    executeJS = NULL;
    showDevTools = NULL;
    getImageSize = NULL;
    getImage = NULL;
    getAudioBuffer = NULL;

    //WARNING: If FreeLibrary function is not called, memory will be leaked.
    //but, in some DAWs There's access violation exception when call FreeLibrary function.
    //So I have no idea what can I do.
    /*if (handleToFree != NULL) {
        FreeLibrary(handleToFree);
    }*/

    //If you want to delay to run some methods, try this (called thread)
    /*if (handleToFree != NULL) {
        CefCleanerThread* cleaner = new CefCleanerThread(handleToFree);
        cleaner->startThread();
    }*/

    //kill();
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

//void CefLoader::CefCleanerThread::run()
//{
//    if (handleToFree != NULL) {
//        FreeLibrary(handleToFree);
//    }
//}