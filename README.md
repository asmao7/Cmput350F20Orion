# OrionBot
Orion SC2 Bot for CMPUT 350 at UAlberta.

This bot works with our fork of [Sc2LadderServer](https://github.com/solinas/Sc2LadderServer) which will be used to run the tournament at the end of the term. It should help you
set up the build process with the correct version of SC2 API so you can focus on creating your bot.

# Developer Install / Compile Instructions
## Requirements
* [CMake](https://cmake.org/download/)
* Starcraft 2 ([Windows](https://starcraft2.com/en-us/)) ([Linux](https://github.com/Blizzard/s2client-proto#linux-packages)) 
* [Starcraft 2 Map Packs](https://github.com/Blizzard/s2client-proto#map-packs)

## Windows

Download and install [Visual Studio 2019](https://www.visualstudio.com/downloads/) if you need it. Building with Visual Studio 2019 not yet supported.

```bat
:: Clone the project
$ git clone --recursive https://github.com/asmao7/Cmput350F20Orion.git
$ cd Cmput350F20Orion

:: Create build directory.
$ mkdir build
$ cd build

:: Generate VS solution.
$ cmake ../ -G "Visual Studio 16 2019"

:: Build the project using Visual Studio.
$ start OrionBot.sln

:: Play against built-in AI by specifying command line arguments
.\OrionBot.exe -c -a zerg -d hard -m CactusValleyLE.SC2Map
```

