#pragma once

#ifdef CEFWRAPPER_EXPORTS
#define CEFWRAPPER_API __declspec(dllexport)
#else
#define CEFWRAPPER_API __declspec(dllimport)
#endif

extern "C" {
    CEFWRAPPER_API int initializeCEF(const char* path, wchar_t* cache);
    CEFWRAPPER_API void shutdownCEF();

    CEFWRAPPER_API void resized();
    CEFWRAPPER_API void loadURL(const char* url);
    CEFWRAPPER_API void timerCallback();
    CEFWRAPPER_API void setLocalBounds(int width_, int height_);
    CEFWRAPPER_API void setAudioParam(int sampleRate_, int samplesPerBlock_);

    CEFWRAPPER_API void mouseMove(int x, int y);
    CEFWRAPPER_API void mouseDown(int x, int y, int isLeft, int isMiddle, int isRight, int clickCount);
    CEFWRAPPER_API void mouseUp(int x, int y, int isLeft, int isMiddle, int isRight, int clickCount);

    CEFWRAPPER_API void getImage(void* p, int* width, int* height);
    CEFWRAPPER_API void getAudioBuffer(float** data, int length);
}