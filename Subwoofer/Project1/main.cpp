//Sub-process for Layby
//Actually, I want to buy a subwoofer because I love hearing the music with speakers!!!

#include <windows.h>
#pragma comment(lib, "libcef.lib")
#pragma comment(lib, "libcef_dll_wrapper.lib")
#include "include/cef_app.h"
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#pragma comment(linker, "/ENTRY:wWinMainCRTStartup")

#ifdef _WIN32
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPWSTR lpCmdLine, int nCmdShow) {
    CefMainArgs args(hInstance);
#else
int main(int argc, char* argv[]) {
    CefMainArgs args(argc, argv);
#endif
    return CefExecuteProcess(args, nullptr, nullptr);
}