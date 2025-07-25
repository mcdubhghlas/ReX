# Building ReX Engine
## Requirements
### Linux/Unix
The following is required:
- GCC 14+ or Clang 16+.
- Python 3.6+.
- CMake 3.28+.
- pkg-config (used to detect the development libraries listed below).
- Development libraries:
	- X11, Xcursor, Xinerama, Xi and XRandR.
	- Wayland and wayland-scanner.
	- Mesa.
	- ALSA.
	- PulseAudio.

### Windows
The following is required:
- A C++ Compiler
	- Visual Studio Community 2022+.
	- MinGW-w64 with GCC 14+.
	- MinGW-LLVM with Clang 16+.
- Python 3.6+.
- CMake 3.28+.

### MacOS
The following is required:
- Python 3.8+.
- CMake 3.28+.
- Xcode
- Vulkan SDK

## Compiling
Configure CMake by running the following command from the source root directory:
`cmake -B build`

Once CMake is configured, you can start compiling with the following command:
`cmake --build build`

You can speed up compile time by specifying `-j #` to allow for multiple jobs to run simultaniously.
`cmake --build build -j 15`

To compile with mono support add `-DENABLE_MODULES="mono"`.
`cmake -DENABLE_MODULES="mono" -B build`
