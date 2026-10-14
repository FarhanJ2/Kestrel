# Kestrel

Phoenix 6 robot program for a Raspberry Pi (64-bit Raspberry Pi OS). Phoenix 6 for non-FRC targets is Linux-only, so on macOS everything (build, IntelliSense, test runs) happens inside Docker.

# Developing (macOS, Zed)

1. Start Docker Desktop.
2. Open this folder in Zed and choose **Open in Dev Container** (or run `project: open remote` → Dev Container).

The dev container is built from the `dev` stage of the `Dockerfile`. On creation it runs CMake, which writes `build/compile_commands.json`; clangd runs inside the container and picks it up automatically, so Phoenix 6, SDL2 and the Linux system headers all resolve.

Inside the container:

```sh
cmake --build build -j   # build
./build/Kestrel          # run
```

After changing `CMakeLists.txt` or adding source files, re-run `cmake -S . -B build`.

To build without opening the editor, run `./build.sh [Debug|Release]` on the host. It uses the same image and the same `build/` directory as the dev container.

# Deploying to the Raspberry Pi

The `runtime` stage of the `Dockerfile` is a minimal image containing only the Release binary and its libraries. With Docker installed on the Pi:

```sh
git clone <this repo> && cd Kestrel
docker compose up -d --build   # build and start; restarts on boot and on crash
docker compose logs -f         # watch output
```

`compose.yaml` runs the container on the host network (SocketCAN interfaces are only visible there) and passes through `/dev/input` for the game controller.

- If using a CANivore, [install canivore-usb](https://v6.docs.ctr-electronics.com/en/stable/docs/installation/installation-nonfrc.html#canivore-installation) on the Pi itself; it is a kernel module and cannot live in the container.
- Alternatively, build the image on an arm64 Mac and copy it over: `docker build --target runtime -t kestrel . && docker save kestrel | ssh <pi> docker load`, then `docker compose up -d` on the Pi.

## Running without Docker

On any supported Debian/Ubuntu system: [install Phoenix 6](https://v6.docs.ctr-electronics.com/en/stable/docs/installation/installation-nonfrc.html) using the C++ (Linux) instructions, `sudo apt install cmake build-essential libsdl2-dev`, then:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/Kestrel
```

# Modifying the Robot Program

The main robot program is located inside main.cpp.

By default, the GameController class is used for controller input. Users may choose to use the Joystick class instead.

## Setting up Generic SocketCAN Adapters

When the canivore-usb kernel module is installed (required when using CANivore), **all** SocketCAN adapters will be automatically configured and started by the robot program.

However, if it is not installed, then SocketCAN adapters must be manually brought up before running the robot program using `./generic_socketcan_start.sh [CAN interface (default: can0)]`.
