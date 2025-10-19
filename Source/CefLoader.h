/*
  ==============================================================================

    CefLoader.h
    Created: 16 Oct 2025 8:06:02am
    Author:  MineEric64

  ==============================================================================
*/

#pragma once
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <string.h>

#include <JuceHeader.h>

typedef int (__cdecl* INITIALIZE_CEF)(const char* path, const wchar_t* cache);
typedef void (__cdecl* SHUTDOWN_CEF)();
typedef int (__cdecl* IS_INITIALIZED)();
typedef void (__cdecl* RESIZED)();
typedef void (__cdecl* LOAD_URL)(const char* url);
typedef void (__cdecl* GET_URL)(char* url);
typedef void (__cdecl* TIMER_CALLBACK)();
typedef void (__cdecl* SET_LOCAL_BOUNDS)(int width_, int height_);
typedef void (__cdecl* SET_AUDIO_PARAM)(int sampleRate_, int samplesPerBlock);
typedef void (__cdecl* MOUSE_MOVE)(int x, int y);
typedef void (__cdecl* MOUSE_DOWN)(int x, int y, int isLeft, int isMiddle, int isRight, int clickCount);
typedef void (__cdecl* MOUSE_UP)(int x, int y, int isLeft, int isMiddle, int isRight, int clickCount);
typedef int (__cdecl* KEY_PRESSED)(const char16_t key, int vk);
typedef void (__cdecl* EXECUTE_JS)(const char* script);
typedef void (__cdecl* SHOW_DEV_TOOLS)();
typedef void (__cdecl* GET_IMAGE_SIZE)(int* width, int* height);
typedef int (__cdecl* GET_IMAGE)(void* p, int length);
typedef int (__cdecl* GET_AUDIO_BUFFER)(float** data, int length, int destChannels);

class CefLoader {
public:
    CefLoader();
    ~CefLoader();
    bool init(const wchar_t* dirPath, const wchar_t* dllPath);
    void free();
    void kill();

    HMODULE handle = NULL;

    //Functions
    INITIALIZE_CEF initializeCEF = NULL;
    SHUTDOWN_CEF shutdownCEF = NULL;
    IS_INITIALIZED isInitialized = NULL;
    RESIZED resized = NULL;
    LOAD_URL loadURL = NULL;
    GET_URL getURL = NULL;
    TIMER_CALLBACK timerCallback = NULL;
    SET_LOCAL_BOUNDS setLocalBounds = NULL;
    SET_AUDIO_PARAM setAudioParam = NULL;
    MOUSE_MOVE mouseMove = NULL;
    MOUSE_DOWN mouseDown = NULL;
    MOUSE_UP mouseUp = NULL;
    KEY_PRESSED keyPressed = NULL;
    EXECUTE_JS executeJS = NULL;
    SHOW_DEV_TOOLS showDevTools = NULL;
    GET_IMAGE_SIZE getImageSize = NULL;
    GET_IMAGE getImage = NULL;
    GET_AUDIO_BUFFER getAudioBuffer = NULL;

//private:
//    class CefCleanerThread : public juce::Thread
//    {
//    public:
//        CefCleanerThread(HMODULE h) : juce::Thread("CEFCleaner"), handleToFree(h) {}
//        void run() override;
//
//    private:
//        HMODULE handleToFree;
//    };
};