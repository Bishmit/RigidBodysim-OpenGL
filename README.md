# 🧱 Rigid Body Simulation

![Demo](Picture_SS/prototype2.png)

A 2D physics engine built using Modern OpenGL and Dear ImGui, supporting real-time rendering and interactive GUI controls.

## 🚀 Setup and Run Instructions

## 🧩 Clone the Repository

```
git clone https://github.com/Bishmit/RigidBodySim-OpenGL.git
cd RigidBodySim-OpenGL
```

## 💻 Platform-Specific Build Instructions

### 🐧 Linux (Debian/Ubuntu)

```
sudo apt update
sudo apt install libglfw3-dev libgl1-mesa-dev libx11-dev libxi-dev libxrandr-dev \
                 libxinerama-dev libxcursor-dev libgl-dev ninja-build cmake g++

mkdir build && cd build
cmake .. -G Ninja
ninja
./main
```

### 🐧 Linux (Arch)

```
sudo pacman -Syu glfw-x11 mesa ninja cmake gcc

mkdir build && cd build
cmake .. -G Ninja
ninja
./main
```

### 🐧 Linux (Fedora)

```
sudo dnf install glfw-devel mesa-libGL-devel ninja-build cmake g++

mkdir build && cd build
cmake .. -G Ninja
ninja
./main
```

### 🍎 macOS

```
brew install glfw glad cmake ninja

mkdir build && cd build
cmake .. -G Ninja
ninja
./main
```

**If CMake fails to detect OpenGL/GLFW on macOS**

```
export GLFW_DIR=$(brew --prefix glfw)
```

---

### 🪟 Windows

**Option 1: Using vcpkg (Recommended)**

1. Install vcpkg:

```powershell
git clone https://github.com/Microsoft/vcpkg.git C:\Tools\vcpkg
cd C:\Tools\vcpkg
.\bootstrap-vcpkg.bat
```

2. Install required packages:

```powershell
.\vcpkg.exe install glfw3 glad glm --triplet x64-windows
```

3. Install Visual Studio 2022 (Community Edition is free)

* Include "Desktop development with C++"

```
.\build.ps1
```

---

Run the executable:

```
.\build\Release\main.exe
```

**Open the generated solution in Visual Studio and build the project**

---

## 🛠️ Build Script (Linux / macOS)

For Linux and macOS users, a helper build script is provided:

```
./build.sh
```

The script:

* Detects the operating system
* Checks for CMake and required libraries
* Configures and builds the project automatically
* Optionally runs the executable after a successful build
