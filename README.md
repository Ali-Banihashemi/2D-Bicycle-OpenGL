# Setup on VSCode (Windows)

This guide will walk you through setting up this OpenGL project in Visual Studio Code (VSCode) on a Windows machine using GLFW3 and Glad libraries. You can use the same approach to set up this project on other operating systems; however, using Visual Studio is probably easier for beginners.

## Prerequisites

Visual Studio Code: Download and install [VSCode](https://code.visualstudio.com/download).

MinGW or Visual Studio: A C++ compiler is required. You can use either [MinGW](https://code.visualstudio.com/docs/cpp/config-mingw) or [Visual Studio](https://visualstudio.microsoft.com/downloads/).

CMake: Download and install [CMake](https://cmake.org/download/).

## Step-by-Step Guide

### 1. Install VSCode Extensions

Open VSCode and install the following extensions:

C/C++ by Microsoft

CMake Tools by Microsoft

CMake by twxs

### 2. Install MinGW (if using MinGW)

If you choose to use MinGW as your compiler:

Download MinGW from [MinGW-w64](https://www.mingw-w64.org/downloads/).

Install MinGW and add C:\MinGW\bin (or the path where MinGW is installed) to your system's PATH environment variable.

### 3. Download and Set Up GLFW

Download the GLFW binaries from [GLFW](https://www.glfw.org/download.html).

Extract the downloaded files to a convenient location, e.g., C:\glfw.

### 4. Download and Set Up Glad

Go to the [Glad website](https://glad.dav1d.de/).

Choose the following settings:

Language: C/C++

Specification: OpenGL

API: gl version 3.3 or later

Profile: Core

Generate a loader: Yes

Click on "Generate" and download the generated files.

Extract the downloaded files to a convenient location, e.g., C:\glad.

### 5. Create Your Project Directory

Create a new directory for your project, e.g., C:\OpenGLProject.

Inside this directory, create the following subdirectories:

src (for your source code)

include (for header files)

lib (for library files)

### 6. Set Up Your CMakeLists.txt

Create a CMakeLists.txt file in your project directory with the following content:

```
cmake_minimum_required(VERSION 3.10)
project(OpenGLProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify the paths to GLFW and Glad
set(GLFW_DIR "C:/glfw")
set(GLAD_DIR "C:/glad")

# Include directories
include_directories(${GLFW_DIR}/include ${GLAD_DIR}/include)

# Link directories
link_directories(${GLFW_DIR}/lib-mingw-w64 ${GLAD_DIR}/src)

# Source files
set(SOURCES src/main.cpp ${GLAD_DIR}/src/glad.c)

# Add executable
add_executable(OpenGLProject ${SOURCES})

# Link libraries
target_link_libraries(OpenGLProject glfw3 opengl32)
```
### 7. Add The Source Code

Add main.cpp from this repository to the src folder in your project directory.

### 8. Build and Run Your Project

Open VSCode in your project directory.

Open the Command Palette (Ctrl+Shift+P) and select CMake: Configure.

Select the appropriate kit (compiler) if prompted.

After configuring, open the Command Palette again and select CMake: Build.

Once the build is complete, you can run your program by opening the Command Palette and selecting CMake: Run Without Debugging.

## Troubleshooting

Ensure that your paths to GLFW and Glad are correct in the CMakeLists.txt.

If using MinGW, make sure MinGW's bin directory is in your system's PATH.

Check that all dependencies are correctly installed and configured.

Additional Resources

GLFW

Glad

