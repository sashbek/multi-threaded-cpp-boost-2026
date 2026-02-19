# Functionality

The project is supposed to be a cmd line multi-threaded realtime game with simple interface (Clash Royale mini edition ;) ).

* There is common field 10x15 dots and power and health level for both players.
* Power level raises every second at one point.
* Every player can choose one of his "cards" and place it in one of ten dots in his first row.
* His placing row is lowest for him and highest for his opponent.
* Players cardstack consists of 4 cards and they all are in access in each time.
* Placed units have health level and cost.
* When card is placed, it's cost is deducted from players power level.
* Every card moves forward with speed one dot per half of a second.
* When two cards are on neighboring dots - they both damage each over 1 health point per second. Some card damages all it's neighbours and some - only first of them. After the death of one cards alife one continues it's movement forward.
* There are also another special effects of cards.
* When unit comes to the over side of the field he damages the opponent health layer.

After game each player gets some amount of points (which are added to his total amount). His opponents are supposed to be found as players with the closest total amount to him (who are also in game search mpde).

## Interface example

```cmd
╔══════════════════════════════════════════════════════════════════════════════╗
║                           CLASH MINI (REALTIME BATTLE)                       ║
╠══════════════════════════════════════════════════════════════════════════════╣
║  Player: ALICE                      Rating: 1240                             ║
║  Opponent: BOB                      Rating: 1190                             ║
╠══════════════════════════════════════════════════════════════════════════════╣
║                                                                              ║
║   [BOB's TOWER]   ╔═══╤═══╤═══╤═══╤═══╤═══╤═══╤═══╤═══╤═══╤═══╤═══╗          ║
║   (Health: 250)   ║   │   │   │   │   │   │   │   │   │   │   │   ║          ║
║                   ╠═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╣          ║
║                   ║   │   │   │   │   │   │   │   │   │   │   │   ║          ║
║                   ╠═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╣          ║
║                   ║   │   │   │   │   │   │   │   │   │   │   │   ║          ║
║                   ╠═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╣          ║
║                   ║   │   │   │   │   │   │   │   │   │   │   │   ║          ║
║                   ╠═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╣          ║
║                   ║   │   │   │   │   │   │   │   │   │   │   │   ║          ║
║                   ╠═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╣          ║
║                   ║   │   │   │   │   │   │   │   │   │   │   │   ║          ║
║                   ╠═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╣          ║
║                   ║   │   │   │   │   │   │   │   │   │   │   │   ║          ║
║                   ╠═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╣          ║
║                   ║   │   │   │   │   │   │   │   │   │   │   │   ║          ║
║                   ╠═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╣          ║
║                   ║   │   │   │   │   │   │   │   │   │   │   │   ║          ║
║                   ╠═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╣          ║
║                   ║   │   │   │   │   │   │   │   │   │   │   │   ║          ║
║                   ╠═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╣          ║
║                   ║   │   │   │   │   │   │   │   │   │   │   │   ║          ║
║                   ╠═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╣          ║
║                   ║   │   │   │   │   │   │   │   │   │   │   │   ║          ║
║                   ╠═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╣          ║
║                   ║   │   │   │   │   │   │   │   │   │   │   │   ║          ║
║                   ╠═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╣          ║
║                   ║   │   │   │   │   │   │   │   │   │   │   │   ║          ║
║                   ╠═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╪═══╣          ║
║                   ║   │   │   │   │   │   │   │   │   │   │   │   ║          ║
║   (Health: 250)   ╚═══╧═══╧═══╧═══╧═══╧═══╧═══╧═══╧═══╧═══╧═══╧═══╝          ║
║   [ALICE's TOWER]                                                            ║
║               Rows: 1   2   3   4   5   6   7   8   9  10  11  12            ║
╠══════════════════════════════════════════════════════════════════════════════╣
║  POWER: [====      ] 4/10 (▲ +1/sec)                                         ║
╠══════════════════════════════════════════════════════════════════════════════╣
║  YOUR CARDS:                                                                 ║
║    [Q] KNIGHT     (HP:15, DMG:2, COST:3) - Damages one enemy                 ║
║    [W] SPEAR GOB. (HP: 8, DMG:1, COST:2) - Damages all neighbours            ║
║    [E] ARCHER     (HP:10, DMG:2, COST:3) - Damages one enemy                 ║
║  > [R] BARBARIANS  (HP:20, DMG:3, COST:5) - Damages one enemy                ║
╠══════════════════════════════════════════════════════════════════════════════╣
║  STATUS BAR: Error - not enougth power to place selected card                ║
╠══════════════════════════════════════════════════════════════════════════════╣
║  [Q-R] Choose card | [1-+] Place card | [Esc] Quit | Ping: 3.1 ms            ║
╚══════════════════════════════════════════════════════════════════════════════╝
```

# Created with: QtBoostCMake template (by bialger)

A C++ project template with Qt6, Boost, Google Tests and CMake that downloads and compiles Qt and Boost.
The resulting app is an executable that can be run on a system without Qt and Boost.

The main Qt6 configuration is found in the [correspondent](lib/qt/CMakeLists.txt) CMakeLists.txt file.
The main Boost configuration is found in the [correspondent](lib/boost/CMakeLists.txt) CMakeLists.txt file.
The main Google Tests configuration is found in the [correspondent](tests/CMakeLists.txt) CMakeLists.txt file.

> Note that statically compiled Qt6 weights more than 5 GB.
> The good thing is that user does not need whole compiled Qt6 with this configuration.

## Prerequisites

* CMake
* Ninja
* Git

On Linux: `libgl1-mesa-dev libglu1-mesa-dev` and all dependencies are required.
> Note that most of the libraries that match `*xcb*` and `*xkb*` may be required on Linux.

## How to build and run

Run the following commands from the project directory.

1. Create CMake cache

This step may take a lot of time because it downloads and compiles Qt.

```shell
cmake -S . -B cmake-build -G "Ninja" -DCMAKE_BUILD_TYPE=Release
```

2. Build executable target

```shell
cmake --build cmake-build --target QtCMake
```

3. Run executable target

* On Windows:

```shell
.\cmake-build\bin\QtCMake.exe
```

* On *nix:

```shell
./cmake-build/bin/QtCMake
```
