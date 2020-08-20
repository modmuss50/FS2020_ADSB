# FS2020_ADSB

This is a small program that uses the Microsoft Flight Simulator 2020 SDK to output current in-game location of your plane
to a commonly used data stream. This allows you to use existing flight tracking software to track yourself in game, even on another device such as an iPad.

(Currently [SBS Base station format](http://woodair.net/sbs/Article/Barebones42_Socket_Data.htm) is used as a proof of concept)

### Building (Windows x64 only)

Visual studio 2019 and MSFS 2020 SDK are required, I used CLion as my IDE.

Copy simconnect.h from `C:\MSFS SDK\SimConnect SDK` into this dir.

Run cmake with the following args: `-G "Visual Studio 16 2019" -A x64`

Make sure to have SimConnect.dll next to the exe file


## TODO 

If you want to help out with this project feel free to pick a todo from here or contribute your own ideas!

- Output the raw ADSB format for wider compatibility with flight tracking software.
- Output nearby aircraft
- Map aircraft models to an existing IACO hex code so the aircraft model matches. (Currenly shows as an F35)
- CI using github actions, upload to github releases
- Better docs on how to use it
- Rewrite in another language that isnt c++? C? / Rust? / D? / Go?
- Make the socket client better, its not very thread safe and can leak memory.
- Allow the default port (30003) to be set via a run arg or something