# Apex External Base

<div align="center">

![C++](https://img.shields.io/badge/Language-C%2B%2B20-00599C.svg?style=for-the-badge&logo=c%2B%2B)
![Platform](https://img.shields.io/badge/Platform-Windows%20x64-0078D6.svg?style=for-the-badge&logo=windows)
![Graphics](https://img.shields.io/badge/Renderer-DirectX%2011-43B02A.svg?style=for-the-badge&logo=directx)
![UI](https://img.shields.io/badge/UI-ImGui-F1502F.svg?style=for-the-badge)
![Target](https://img.shields.io/badge/Target-Apex%20Legends%20DX12-E03C31.svg?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-green.svg?style=for-the-badge)

A modern, high-performance, multithreaded external base and framework for **Apex Legends (DirectX 12)** written in C++20. Featuring custom ImGui rendering, window hijacking, memory caching, and modular architecture.

</div>

---

## ✨ Features

- **🚀 Multithreaded Caching System**: Dedicated `std::jthread` loops for process monitoring, game state caching (`GameCache`), and player/entity caching (`PlayerCache`) to ensure zero rendering stutter and optimal FPS.
- **🎨 DirectX 11 & ImGui Overlay**: High-performance external overlay via transparent window hijacking with custom drawing primitives and a sleek ImGui-powered menu.
- **⚡ Driver / CR3 Memory Interface**: Clean external driver communication abstraction supporting CR3 translation, image base resolution, and secure memory reads/writes.
- **🔒 Compile-Time String Encryption**: Integrated `skCrypt` for compile-time string protection.
- **📦 Clean & Modular Structure**: Decoupled systems (`core/`, `engine/`, `deps/`, `include/`) for seamless feature expansion.

---

## 📁 Project Structure

```text
apex-external-base/
├── core/
│   ├── menu/                 # ImGui UI menu system
│   ├── rendering/            # DirectX 11 render utils & custom drawing primitives
│   ├── systems/              # Player loop & visual rendering logic
│   └── render_loop.cpp       # Main overlay rendering loop
├── deps/
│   ├── crypter/              # skCrypt compile-time string encryption
│   ├── imgui/                # Dear ImGui library & Win32/DX11 backends
│   ├── math/                 # Vector and matrix math structures
│   ├── memory/               # Memory communication & driver interface
│   └── overlay/              # DirectX 11 overlay & window hijacking
├── engine/
│   ├── camera/               # World-to-screen & camera matrix logic
│   ├── game/                 # Game state caching & management thread
│   ├── impl/                 # Engine offsets & definitions
│   ├── player/               # Entity & player structures
│   └── threading/            # Multithreaded game & player loops
├── include/
│   └── includes.h            # Primary precompiled header & dependencies
└── main.cpp                  # Application entry point & orchestration
```

---

## 🛠️ Prerequisites

- **Visual Studio 2022** (with C++ Desktop Development workload)
- **Windows SDK** (Windows 10 / Windows 11)
- **DirectX SDK / DirectX 11 compatible graphics driver**
- **C++20** standard or newer

---

## 🚀 Getting Started

### 1. Build the Project
1. Open [`apex-external-base.sln`](apex-external-base.sln) in Visual Studio 2022.
2. Set the build configuration to **`Release | x64`**.
3. Build the solution (`Ctrl + Shift + B`).

### 2. Usage
1. Launch **Apex Legends** (`r5apex_dx12.exe`).
2. Run the compiled `apex-external-base.exe` executable as Administrator.
3. Once the console displays `> F1 IN GAME`, press <kbd>F1</kbd> while inside the game window.
4. The overlay will attach and render visual systems.

---

## 🏷️ Tags & Topics

```text
[ c++20 ]  [ apex-legends ]  [ external-base ]  [ imgui ]  [ directx11 ]  [ dx12 ]  [ game-hacking ]  [ memory-reading ]  [ window-hijacking ]  [ multithreading ]  [ skcrypt ]
```

> **Keywords:** `#cpp` `#cpp20` `#apex-legends` `#apex-external` `#imgui` `#directx11` `#dx12` `#memory-reading` `#game-hacking` `#window-hijacking` `#external-overlay` `#r5apex` `#skcrypt`

---

## ⚠️ Disclaimer & License

This project is intended for **educational and proof-of-concept purposes only**. The authors are not responsible for any misuse of this codebase or any consequences arising from its use.

Distributed under the **MIT License**.
