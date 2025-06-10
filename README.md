# TransportKit

**T**ransport**K**it - 提供多协议数据传输加速、P2P 传输、本地代理等功能。

## 架构

![TransportKit](./Documents/TransportKit.png)

## 目录

- [Bazel](./Bazel) - Bazel 配置
- [CMake](./CMake) - CMake 配置
- [Examples](./Examples) - 使用示例
- [Scripts](./Scripts) - 构建相关的脚本
- [Source](./Source)
  - [API](./Source/API) - TransportKit C API
  - [TransportCore](./Source/TransportCore) - 数据传输内核
  - [LocalServer](./Source/LocalServer) - 本地代理服务
  - [Debugger](Source/Debugger) -  本地调试器
  - [Telemetry](Source/Telemetry) - 遥测模块
  - [NetProber](Source/NetProber) - 网络拨测器
  - [ThirdParty](./Source/ThirdParty) - 第三方库
  - [TX](./Source/TX) - 基础运行时库  (**T**emplate e**X**tended)
- [Platform](./Source/Platform) - 各个平台对 TransportKit C API 的绑定
- [Tests](./Tests) - 功能测试
