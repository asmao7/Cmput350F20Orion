# OrionBot

![](https://github.com/asmao7/Cmput350F20Orion/blob/master/docs/OrionBot.gif)

OrionBot is a SC2 bot for CMPUT 350 at UAlberta.

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
.\OrionBot.exe -c -a zerg -d Hard -m CactusValleyLE.SC2Map
.\OrionBot.exe -c -a zerg -d Hard -m BelShirVestigeLE.SC2Map
.\OrionBot.exe -c -a zerg -d Hard -m ProximaStationLE.SC2Map
```

## License
```MIT License

Copyright (c) 2021 Anastasia Borissova, Joe Ha, Asma Omar

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
