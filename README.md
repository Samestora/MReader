```
                                    __  _______                 __         
                                   /  |/  / __ \___  ____ _____/ /__  _____
                                  / /|_/ / /_/ / _ \/ __ `/ __  / _ \/ ___/
                                 / /  / / _, _/  __/ /_/ / /_/ /  __/ /    
                                /_/  /_/_/ |_|\___/\__,_/\__,_/\___/_/     

                                       Windows x86_64 - DirectX 11
                                         Minimal memory footprint
                                          No Installation Needed
```

<p align="center">
    <img src="https://img.shields.io/badge/language-c%2B%2B-%23f34b7d.svg?style=for-the-badge&logo=cplusplus" alt="C++">
    <img src="https://img.shields.io/badge/platform-Windows-0078d7.svg?style=for-the-badge&logo=wine" alt="Windows">
    <img src="https://img.shields.io/badge/arch-x86_64_-green.svg?style=for-the-badge" alt="x86_64_">
</p>

## :open_book: MReader
**True native desktop application** for Windows compatible from Windows 7 to 11  
Portable (no installation needed) with low memory footprint all in DirectX11

#### Images that are currently supported by MReader :
- JPG / JPEG 
- PNG
- WebP
- HDR

> Will support more as the application is more developed

#### Libraries that are used :

- [DirectX SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812)  
- [JSON for Modern C++ v3.11.3](https://github.com/nlohmann/json.git)  
- [STB](https://github.com/nothings/stb)  
- [libwebp v1.5.0](https://developers.google.com/speed/webp/download)  
- [ImGui v1.91.6](https://github.com/ocornut/imgui)

## :rocket: Usage
1. Basic  
1.1 Add your manga or comic inside the ``Manga/`` directory  
1.2 Add your custom font inside the ``Font/`` directory  
1.3 Run the executable

2. Advanced  
2.1  Change the ``config.json`` and change it to suit your needs but it must end with trailing ``\\`` at the end of the path!  
2.2  How it would probably look for you
```json
{
  "mangaPath": "D:\\Manga\\2023\\Winter\\",
  "fontPath": "C:\\Fonts\\OpenDyslexic\\"
}
```
## 🧪 Development

[!IMPORTANT]  
It's still under pre release, so expect bugs.

> **Note** <br>
> Make sure that **DXSDK_DIR** is declared in your environment variables.

To see your environment variables :

> **Settings --> System --> About --> System Advanced Settings --> Environment Variables**

### Visual Studio 2022 :

1. Make sure to have all of the necessary vendor file this is possible by pulling all of the submodules. 
2. Open the solution file (.sln).
3. Build the project in Release (x86 or x64).    
3.1 if you want to develop on x86 you have to manually compile libwebp to 32bit library and store it. (I've included it inside the vendor, but in case of update you have to do it yourself)

Every configuration in x86 / x64 (Debug and Realese) are already configured with librairies and includes.

Everything is setup, you just need to choose between DLL or EXE.

> **Warning** <br>
> If you have any linking error when compiling make sure that you have correctly install DirectX SDK.

### LICENSE
[Licensed under GPL-2.0 license]('./LICENSE.txt')