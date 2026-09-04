# WebAssembly feasibility prototype

This target proves that the existing C++/Qt application shell and ManiVault
plugin interfaces can run in a browser. It intentionally builds only the
existing core-owned `DataHierarchyPlugin` as a static view plugin. Desktop
builds continue to discover shared-library plugins in the `Plugins` directory.

## Prerequisites

- Qt 6.8 or newer for WebAssembly, including `Core`, `Gui`, `Widgets`,
  `OpenGL`, `OpenGLWidgets`, and `Concurrent`
- The Emscripten SDK version required by that Qt package
- Ninja

Use a multithreaded Qt for WebAssembly package while `Qt6::Concurrent` and
Taskflow remain enabled. Serving a multithreaded build requires COOP/COEP HTTP
headers; Qt's `qtwasmserver` development server configures these headers.

## Configure and build

Activate the Emscripten environment, then use the `qt-cmake` belonging to the
Qt for WebAssembly installation:

```powershell
<qt-wasm>\bin\qt-cmake.bat -S . -B build-wasm `
  -G Ninja `
  -DMV_INSTALL_DIR="$PWD/install-wasm" `
  -DCMAKE_BUILD_TYPE=Release

cmake --build build-wasm --parallel
<qt-wasm>\bin\qtwasmserver.py --port 8000 build-wasm\ManiVault
```

Open the URL printed by `qtwasmserver`. The application creates a blank
project, discovers `DataHierarchyPlugin` through Qt's static plugin registry,
and docks its QWidget in the ManiVault workspace.

## Deliberate prototype limits

- No dynamic WebAssembly plugin loading
- No external plugin repositories
- No Python integration
- No Qt WebEngine-based splash, Markdown/about, or embedded-video widgets
- No Sentry/Crashpad, cpptrace, AVX, install, or native deployment steps

The static loader resolves the same `dependencies` metadata used by dynamic
plugins. A subsequent milestone can add PointData and Scatterplot as static
targets without replacing the loader or the plugin interfaces.
