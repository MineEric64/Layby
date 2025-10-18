# Layby
[![demon](https://github.com/MineEric64/Layby/blob/main/demon.jpg?raw=true)](https://www.buymeacoffee.com/mineeric64)


 > [!NOTE]
 > Made by **MineEric64**
>
YouTube Player VST

It works similarly compared to [VidPlayYTSync](https://youtu.be/NEcKXDB0zig) plugin, but **Layby** supports processing audio output to DAW (adding audio input from YouTube video's audio source). So, you can process audio from YouTube's video without any restrictions!

For example on [this demonstration video](https://youtu.be/JsedfYSJNWI), It works with various VSTs with DSP!

Also, not only YouTube, but also supporting Soundcloud, Spotify and etc (any website)

Furthermore, IT'S FREE!!!!!

[!["Buy Me A Coffee"](https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png)](https://www.buymeacoffee.com/mineeric64)

(but hey, thank you so much ⸜(｡˃ ᵕ ˂ )⸝♡)

## Download
- https://github.com/MineEric64/Layby/releases/latest

Including VST3 with installer

## Credits
- [JuneCEF: cef.cpp](https://github.com/abhijitnandy2011/JuceCEF/blob/master/glcef/Source/cef.cpp)
- [vdr-osr-browser: osrhandler.cpp](https://github.com/Zabrimus/vdr-osr-browser/blob/encoding/osrhandler.cpp)
- [CefSharp: AudioHandler.cs](https://github.com/cefsharp/CefSharp/blob/cefsharp/85/CefSharp.Example/Handlers/AudioHandler.cs)
- [processBlock();](https://leestrument.tistory.com/entry/processBlock)

## And...
- Used main framework: JUCE
- Web Browser Library: CEF

I tried to use WebView2 instead of CEF, but it failed. IDK why error happens. Just I have no idea.

Using CEF Static Loading crashes DAW. IDK why too. So I had to change Dynamic Loading (at Runtime) using `LoadLibrary()` function.

If you want to port CEF to WebView2 because it's simple and lightweight (or you want to contribute because you just came with AMAZING idea), you're welcome!! (PR)
