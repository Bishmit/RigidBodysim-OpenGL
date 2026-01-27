#!/bin/bash

# ================= CONFIG =================
BUILD_DIR="build"
BUILD_TYPE="Release"
PARALLEL_JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
# =========================================

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${CYAN}==== RigidBodySimu Build Script (Linux/macOS) ====${NC}"

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="Linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macOS"
else
    OS="Unknown"
fi

echo -e "Detected OS: ${GREEN}$OS${NC}"

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}ERROR: CMake is not installed!${NC}"
    echo -e "${YELLOW}Install it using:${NC}"
    if [[ "$OS" == "Linux" ]]; then
        echo "  sudo apt-get install cmake    # Debian/Ubuntu"
        echo "  sudo dnf install cmake        # Fedora"
        echo "  sudo pacman -S cmake          # Arch"
    elif [[ "$OS" == "macOS" ]]; then
        echo "  brew install cmake"
    fi
    exit 1
fi

# Check for required libraries
echo -e "\n${CYAN}Checking for required libraries...${NC}"

check_library() {
    local lib=$1
    if pkg-config --exists "$lib" 2>/dev/null; then
        echo -e "  ✓ $lib found"
        return 0
    else
        echo -e "  ${YELLOW}✗ $lib not found${NC}"
        return 1
    fi
}

missing_libs=()

check_library "glfw3" || missing_libs+=("glfw3")
check_library "gl" || check_library "opengl" || missing_libs+=("OpenGL")

if [ ${#missing_libs[@]} -gt 0 ]; then
    echo -e "\n${YELLOW}WARNING: Some libraries are missing: ${missing_libs[*]}${NC}"
    echo -e "${YELLOW}To install them:${NC}"
    
    if [[ "$OS" == "Linux" ]]; then
        echo -e "  ${CYAN}Debian/Ubuntu:${NC}"
        echo "    sudo apt-get install libglfw3-dev libgl1-mesa-dev libx11-dev"
        echo -e "  ${CYAN}Fedora:${NC}"
        echo "    sudo dnf install glfw-devel mesa-libGL-devel libX11-devel"
        echo -e "  ${CYAN}Arch:${NC}"
        echo "    sudo pacman -S glfw-x11 mesa libx11"
    elif [[ "$OS" == "macOS" ]]; then
        echo "    brew install glfw"
    fi
    
    read -p "Continue anyway? (y/n) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Create build directory
if [ ! -d "$BUILD_DIR" ]; then
    echo -e "\n${CYAN}Creating build directory...${NC}"
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR" || exit 1

# Configure with CMake
echo -e "\n${CYAN}Configuring project with CMake...${NC}"
echo -e "Build Type: ${GREEN}$BUILD_TYPE${NC}"
echo -e "Parallel Jobs: ${GREEN}$PARALLEL_JOBS${NC}"

cmake .. -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

if [ $? -ne 0 ]; then
    echo -e "\n${RED}CMake configuration failed!${NC}"
    cd ..
    exit 1
fi

# Build the project
echo -e "\n${CYAN}Building project...${NC}"
cmake --build . --config "$BUILD_TYPE" --parallel "$PARALLEL_JOBS"

if [ $? -ne 0 ]; then
    echo -e "\n${RED}Build failed!${NC}"
    cd ..
    exit 1
fi

# Success!
echo -e "\n========================================"
echo -e "${GREEN}Build successful!${NC}"
echo -e "========================================\n"
echo -e "${CYAN}Executable location:${NC}"
echo -e "  $BUILD_DIR/main\n"

cd ..

# Offer to run the executable
read -p "Do you want to run the executable now? (y/n) " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo -e "\n${CYAN}Starting application...${NC}"
    ./"$BUILD_DIR"/main
fi