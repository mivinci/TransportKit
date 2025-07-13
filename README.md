# TransportKit

**T**ransport**K**it provides data transfer acceleration (both download and upload), Peer-to-peer data transfers, and
local proxy services.

> The project is still being early developed, do not use it for any purpose whatsoever.
>

What's TransportKit used for?

Data transmission serves as the foundation for nearly all pieces of software – whether downloading games from platforms
like Steam, streaming video content, or requesting resources via HTTP protocols on the web. TransportKit is trying to be
such the one your software can be built upon to deliver data end to end as fast as your bandwidth allows.

## Architecture

![TransportKit](Documents/TransportKit.png)

## Contents

- [Bazel](Bazel) — Bazel configurations
- [CMake](CMake) — CMake configurations
- [Examples](Examples) — Usage examples
- [Scripts](Scripts) — Scripts to build TransportKit
- [Platform](Platform) — Bindings to the TransportKit C API, Java for Android, for example
- [Tests](Tests) — Feature tests
- [Source](Source)
    - [API](Source/API) — TransportKit C API
    - [TransportCore](Source/TransportCore) — Data transfer core
    - [LocalServer](Source/LocalServer) — Local proxy server
    - [Debugger](Source/Debugger) — Debug server
    - [Telemetry](Source/Telemetry) — Telemetry client
    - [NetProber](Source/NetProber) — P2P NetProber
    - [ThirdParty](Source/ThirdParty) — Third-party libraries
    - [KFC](Source/KFC) — **K**it **F**or **C**oncurrency

## Develop

TransportKit uses [CMake](https://cmake.org) to generate building configurations on different platforms.

### CLion (Preferred)

Open the project directory and that's it.

### VSCode

Open the project directory with VSCode, and you'll need at least these plugins:

- [Clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd)
- [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)

### Xcode

Run the command below to generate project configurations for Xcode.

```bash
cmake -G Xcode -B build-xcode .
```

And then use Xcode to open the generated file named `build-xcode/TransportKit.xcodeproj`. You have to re-run the command
instead of editing Xcode's config file manually every time you make file changes to the project.

## Code Style

The project basically follows [LLVM C++ Style Guide](https://llvm.org/docs/CodingStandards.html), along with some of my
personal coding preferences.

## Credits

Projects and tools that help write TransportKit:

- https://github.com/webkit/webkit
- https://github.com/capnproto/capnproto
- https://github.com/curl/curl
- https://webrtc.googlesource.com
- https://copilot.tencent.com

## License

TransportKit is MIT Licensed.
