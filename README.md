# solnce
A (prospective) 2D space strategy game

## Building
Currently the project can only be built on Linux. Windows build is coming soon™.
1. Ensure that you have all dependencies installed (this is for Ubuntu, but it will compile on any distro as long as the equivalent packages are installed)
```
apt install build-essential clang cmake libsdl2-dev libc6 libc-bin
```
2. Run the build script
```
sh ./build.sh
```
An executable named `solnce` will be generated. Beware when packaging that the executable expects `assets/` and `src/shaders/` to be present in the same directory.

Alternatively, if you have Docker you can just run `docker compose up`
