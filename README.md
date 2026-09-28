# Pikku-ukot mesoo

**Pikku-ukot mesoo** is a shooter game, inspired by classics like Liero and Molez, developed between 1999 and 2001.

## Building

The game was written against the proprietary DieselEngine and FMOD 3, neither of which is available anymore. The `port/` directory reimplements the parts of both that the game uses on top of [SDL3](https://libsdl.org), so the original sources in `src/` build on Windows, macOS and Linux with only small portability fixes.

You need a C++17 compiler, CMake 3.26 or newer, and Git. The game uses SDL3 and SDL3_mixer, which you can either install yourself or have CMake download and build with `-DPUM_VENDORED_SDL=ON`. The vendored build links both statically, so the result runs without any SDL libraries installed.

Prebuilt folders for all three platforms are attached to each [CI run](../../actions/workflows/build.yml).

### Windows

Install [Visual Studio 2022](https://visualstudio.microsoft.com/) with the *Desktop development with C++* workload (it includes CMake) and [Git](https://git-scm.com/). Then, in a *Developer PowerShell for VS 2022*:

```powershell
git clone https://github.com/milzer/pikku-ukot-mesoo
cd pikku-ukot-mesoo
cmake -S . -B build -DPUM_VENDORED_SDL=ON
cmake --build build --config Release
.\build\Release\pum.exe
```

### macOS

With [Homebrew](https://brew.sh/):

```sh
brew install cmake sdl3 sdl3_mixer
git clone https://github.com/milzer/pikku-ukot-mesoo
cd pikku-ukot-mesoo
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/pum
```

To build a self-contained executable instead, skip `sdl3 sdl3_mixer` and add `-DPUM_VENDORED_SDL=ON` to the first `cmake` command.

### Linux

On Debian or Ubuntu, install the compiler and the development headers SDL needs for graphics, sound and input:

```sh
sudo apt install build-essential cmake git \
  libasound2-dev libpulse-dev libpipewire-0.3-dev \
  libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
  libwayland-dev libxkbcommon-dev libdecor-0-dev \
  libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev \
  libdbus-1-dev libibus-1.0-dev libudev-dev
```

Other distributions need the equivalent packages. Then build with the vendored SDL, since few distributions package SDL3_mixer 3.x yet:

```sh
git clone https://github.com/milzer/pikku-ukot-mesoo
cd pikku-ukot-mesoo
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DPUM_VENDORED_SDL=ON
cmake --build build
./build/pum
```

If your distribution does have SDL3 and SDL3_mixer 3.x with module (libxmp) support, you can install those and leave out `-DPUM_VENDORED_SDL=ON`.

### Game files

The build copies the game data next to the executable, and the game reads and writes its files there: levels are recompiled from `uudet/` on every start, and `options.kei` and `keys.1`–`keys.3` hold settings and key bindings. To put the game and its data in a folder of their own, run:

```sh
cmake --install build --config Release --prefix dist
```

## Playing

- Main menu: `F1` start, `F2` options, `F3` change level, `Esc` quit
- In game: `Esc` opens the exit prompt (`F9` continue, `F10` quit to menu)
- `Alt+Enter` toggles fullscreen

Default controls (left, right, aim up, aim down, change weapon, jump, shoot):

| Player | Keys |
| --- | --- |
| 1 | arrows, right shift, enter, right ctrl |
| 2 | `S` `F` `E` `D`, left shift, tab, `Q` |
| 3 | `J` `L` `I` `K`, `O`, `Y`, `H` |

Keys can be changed in the options menu under *muuta nappulat*.

## Contributing

Your interest and contributions to the game are warmly welcomed. Whether it's through forking the project to explore new possibilities, or just sharing ideas for its future, every bit of support counts.
