# TK

**T**ransport**K**it

## Contents

- [Bazel](./Bazel) - Bazel 配置依赖
- [CMake](./CMake) - CMake 配置依赖
- [Examples](./Examples) - 使用示例
- [Scripts](./Scripts) - 构建相关的脚本
- [Source](./Source)
  - [Transport](./Source/Transport) - 数据传输模块（提供上传下载优化加速）
  - [TLV](./Source/TLV) - 数据序列化协议
  - [P2P](./Source/P2P) - P2P 通信框架
  - [Tun](./Source/Tun) - 网络层代理模块
  - [Debug](Source/Debug) -  本地调试模块
  - [Telemetry](Source/Telemetry) - 遥测模块
  - [Probe](Source/Probe) - 拨测模块
  - [JSC](Source/JSC) - 轻量 JS 运行时，用于实现插件系统
  - [TX](./Source/TX) - 基础运行时库  (**T**emplate e**X**tended)
  - [ThirdParty](./Source/ThirdParty) - 第三方库
- [Platform](./Source/Platform) - 各个平台对 TransportKit C API 的绑定
- [Tests](./Tests) - 功能测试
