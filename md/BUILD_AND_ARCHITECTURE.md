# Geomec Architecture, Build, and Deployment Guide

This document describes the repository as it exists in this source tree. It covers the main application, shared libraries, bundled third-party components, tests, and the separate Windows and Linux build paths.

## Table of contents

1. [Repository Overview](#1-repository-overview)
2. [Main Application and Modules](#2-main-application-and-modules)
   - 2.1 [Main Windows application: `geomec/`](#21-main-windows-application-geomec)
   - 2.2 [Framework and domain libraries](#22-framework-and-domain-libraries)
     - 2.2.1 [`fw51/`](#221-fw51)
     - 2.2.2 [`lib/`](#222-lib)
   - 2.3 [Executables and integrations](#23-executables-and-integrations)
3. [External Dependencies](#3-external-dependencies)
   - 3.1 [Required platform/runtime dependencies](#31-required-platformruntime-dependencies)
   - 3.2 [Bundled dependencies under `third_party/`](#32-bundled-dependencies-under-third_party)
4. [Windows Build](#4-windows-build)
   - 4.1 [Toolchain and SDK prerequisites](#41-toolchain-and-sdk-prerequisites)
   - 4.2 [Required environment variables](#42-required-environment-variables)
   - 4.3 [Configure licensing and Diana](#43-configure-licensing-and-diana)
   - 4.4 [Compile with Visual Studio](#44-compile-with-visual-studio)
   - 4.5 [Windows deployment](#45-windows-deployment)
5. [Linux Build](#5-linux-build)
   - 5.1 [Linux prerequisites](#51-linux-prerequisites)
   - 5.2 [Configure and compile the qmake graph](#52-configure-and-compile-the-qmake-graph)
   - 5.3 [Linux build caveats](#53-linux-build-caveats)
   - 5.4 [Linux deployment](#54-linux-deployment)
6. [Testing and Smoke Checks](#6-testing-and-smoke-checks)
7. [Recommended Build Order](#7-recommended-build-order)
8. [Files to Read First](#8-files-to-read-first)
9. [Current Repository Limitations](#9-current-repository-limitations)

## 1. Repository Overview

Geomec is a large C++ application made of several layers:

```text
Geomec/
|-- geomec/                 Main Windows Geomec application and GUI code
|-- geomec.pro              Main qmake super-project
|-- Geomec.sln              Visual Studio solution
|-- lib/                    First-party Geomec/Diana/FW51 libraries (cross-platform)
|-- platform/               First-party libraries that currently build on Windows only
|-- third_party/            Vendored SDKs and installers (HDF5, TBB, zlib, Blosc,
|                            gmock/gtest, CORA, SVS, .NET, bitmap data)
|-- fw51/                   Framework 51 geometry, mesh, tensor, and model code
|-- app/                    Auxiliary applications and legacy application projects
|-- geomec_shell/           Console/shell executable
|-- gm_diana_app/           Diana-oriented application
|-- geomec_cora_wrapper/    CORA integration wrapper
|-- geomec_selector/        Selector application
|-- Tests/                  C++ test source (TestLib, TestRunner, Fw51, Materials, ...)
|-- TestData/               Large model/input datasets used by the tests
|-- IntegrationTests/       Script-driven integration-test support (formerly `test/`)
|-- archive/                Historical/orphaned code no longer part of any build target
|-- wix/                    Windows installer projects
|-- qmake/                  Shared qmake configuration and dependency definitions
|-- vsprops/                Visual Studio property sheets
|-- py/                     Python tools and integration helpers
|-- res/                    Build, source-control, and utility scripts
|-- version/                Version headers and version metadata
```

`lib/`, `platform/`, and `third_party/` used to be one directory (`lib/`) that mixed
first-party source with vendored binaries of wildly different sizes. They were split
so that first-party code (`lib/`, `platform/`) is clearly separate from vendored
material (`third_party/`) that is not maintained here. `platform/` holds the subset
of first-party libraries that currently have no qmake (`.pro`) project and therefore
only build on Windows (e.g. the MFC-based `MfcGuiFrame`/`MfcModelFrame`); everything
still in `lib/` builds on both platforms from the same source tree. Similarly,
`Tests/` used to hold both the (tiny) C++ test source and a 400+ MB `Models`
subdirectory of binary test data; the data now lives in the top-level `TestData/`
directory. The old, lowercase `test/` directory was renamed to `IntegrationTests/`
to remove a directory pair that differed only by case (`Tests/` vs `test/`), which is
a hazard on case-insensitive Windows/macOS checkouts. See `FOLDER_STRUCTURE_PROPOSAL.md`
for the full rationale. `Prototypes/` was intentionally left in place even though it
is legacy/experimental, because it is still an active qmake `SUBDIRS` target
(`ModGMBus`) with depth-sensitive relative include paths; moving it was judged
higher-risk than its labeling benefit.

The repository contains both source and prebuilt/runtime material. The `third_party/`
directory is not source Geomec maintains; it contains platform-specific binaries, SDK
files, installers, and examples vendored from upstream projects.

## 2. Main Application and Modules

### 2.1 Main Windows application: `geomec/`

The `geomec/` directory is the main Geomec desktop application. Its modules are organized by domain:

| Module | Responsibility |
| --- | --- |
| `BaseModel` | Base model and common model infrastructure |
| `Geology` | Geological objects and operations |
| `HexaModel` | Hexahedral model support |
| `TetraModel` | Tetrahedral model support and import/export helpers |
| `Material` | Materials and material editing |
| `Value` | Value objects and value-related UI |
| `View` | View, visualization, and tensor visualization |
| `OpenInventor` | Open Inventor integration |
| `GUI`, `dlg`, `delegate`, `observer` | Qt/MFC GUI components and event handling |
| `FlexLM` | FlexLM licensing integration |
| `DStaborLink` | External/analysis integration |
| `IT` | Integration and plug-in support |
| `Test`, `doc`, `Help` | Tests and user/developer documentation |

The corresponding Visual Studio project is `geomec/Geomec.vcxproj`. The root solution is `Geomec.sln`.

### 2.2 Framework and domain libraries

#### 2.2.1 `fw51/`

FW51 contains reusable geometry and numerical data structures:

- `FieldValueFile`: field/value file handling.
- `Geometry`: geometric entities and operations.
- `ModelFrame`: model-frame abstractions.
- `OpenGLFrame`: OpenGL-facing framework code.
- `Tensor`: tensor mathematics and visualization data.
- `TetMesh`: tetrahedral mesh support.
- `Triangle`: triangle and surface mesh support.
- `GuiFrame`: GUI framework code; the qmake project marks this as not normally used.

#### 2.2.2 `lib/`

The main library groups listed by `geomec.pro` are:

- **Core/domain:** `GeomecModel`, `FemAppModel`, `MaterialLibrary`, `MaterialLibraryXML`, `Quantity`, `NewWellScheme`, `WellScheme`, `CommonWellScheme`, `DCasing`, `CutSurfaceUtilModel`, `MeshDataCacher`.
- **Diana integration:** `DianaCore`, `DianaSignals`, `DianaStartup`, `DianaGlue`, `ConsoleLibrary`.
- **Geomec integrations:** `geomec_rgi_library`, `geomec_cora_library`, `RGInterface`, `GMSkuaConnector`.
- **Utilities:** `Util`, `QUtil`, `RPN`, `GeomecStringTable`, `qtiocompressor`.
- **Mesh/compression/storage:** `TSMesh`, `blosc`, `blosc_filter`, HDF5, zlib, and related support.
- **Testing:** Google Mock/gtest under `third_party/gmock-1.7.0` and test libraries under `Tests/`.

The qmake dependency lists are centralized in `qmake/libs.pri`, while include paths and platform paths are assembled by `qmake/all.pri`, `qmake/globals.pri`, and `qmake/basepath.pri`.

### 2.3 Executables and integrations

The qmake super-project defines or references these executable-level projects:

- `geomec/Geomec.vcxproj`: main Windows desktop application.
- `geomec_shell/geomec_shell.pro`: console/shell application using the Geomec, FW51, Diana, and third-party libraries.
- `gm_diana_app/gm_diana_app.pro`: Diana application using `DianaStartup`, `DianaCore`, `QUtil`, and `Util`.
- `geomec_cora_wrapper/geomec_cora_wrapper.pro`: CORA wrapper executable/library boundary.
- `geomec_selector/geomec_selector.pro`: selector application.
- `app/`: auxiliary applications such as `geomec_launcher`, `diana_app`, `createfilos_app`, `geomec_rgi`, `geomec_cora`, and `MPKernelApp`.

There is also a smaller legacy qmake graph in `app.pro` that builds `lib/lib01` before `app/app01`. It is separate from the main `geomec.pro` graph.

## 3. External Dependencies

### 3.1 Required platform/runtime dependencies

The application is tightly coupled to the Diana distribution and its runtime libraries. It also uses:

- Qt for the GUI and XML/OpenGL support.
- Qwt for plotting/widgets on Windows.
- Open Inventor for 3D visualization.
- Intel runtime libraries.
- FlexLM and/or Hasp licensing components.
- HDF5 for data storage.
- TBB for parallel/runtime support in the qmake Linux configuration.
- Boost, GLib, zlib, Blosc, and other support libraries.
- Python integration components for selected tools/plugins.

Many of these dependencies are referenced through environment variables rather than downloaded by the build. A successful compile therefore requires matching SDK headers, import/static libraries, runtime DLLs/shared objects, and license files.

### 3.2 Bundled dependencies under `third_party/`

The repository contains platform-specific copies or build inputs for HDF5, TBB, zlib, Blosc, and Google Test/Mock under `third_party/`. Qt I/O compression (`qtiocompressor`) is a small first-party wrapper and stays under `lib/`. `third_party/` also contains CORA, SVS, .NET, point-element bitmap data, and installer/runtime material (formerly the separate `3rdParty/` directory).

Do not assume that every bundled binary is used by every target. The active dependency set depends on the selected platform, configuration, and executable. The Visual Studio property sheets are the authoritative source for Windows library paths; the qmake `.pri` files are the authoritative source for the qmake/Linux graph.

## 4. Windows Build

The Windows path is the most complete and explicit build path in this repository.

### 4.1 Toolchain and SDK prerequisites

The historical installation instructions in `geomec/INSTALL.txt` specify:

- Visual Studio 2012 Premium with Update 4.
- x64 native build tools and the Visual Studio v140-compatible toolset.
- Intel C++/Fortran runtime libraries.
- WiX Toolset 3.7 for installers.
- Qt and Qwt.
- Open Inventor SDK 9.3.1 for Visual Studio x64.
- Diana distribution with `release`, `debug`, and `share` directories.
- Hasp driver and/or the configured licensing system.

The checked-in Visual Studio files contain an important version mismatch: `Geomec.vcxproj` uses Visual Studio v140 and links Qt5 library names such as `Qt5Core`, `Qt5Widgets`, and `Qt5Xml`, while the older text in `geomec/INSTALL.txt` describes building Qt 4.8.5. Confirm the Qt version expected by the selected project/property sheets before installing or rebuilding Qt.

### 4.2 Required environment variables

The Windows property sheets expect variables including:

```text
INTEL              Intel runtime directory
QTDIR              Qt installation directory
QWTDIR             Qwt installation directory
WixToolPath        WiX bin directory
DIAROOT            Diana root containing release/debug/share
OIVHOME            Open Inventor SDK directory
VSG_LICENSE_FILE   Open Inventor password.dat path
VSG_LICENSE_DEBUG  Optional Open Inventor license log path
```

Add at least these directories to `PATH`:

```text
%QTDIR%\bin
%QWTDIR%\lib
%INTEL%
```

The project derives values such as `DIAPATH`, `DIASHARE`, `DIAOBJ`, `FWDIR`, `FLEXLM`, and `USERDLLS` from `DIAROOT` and the repository layout. See `geomec/Environment.props` and `vsprops/BuildCommon.props`.

Open Inventor may put its Qt DLL directories at the beginning of `PATH`. Move those directories after `%QTDIR%\bin` so that the intended Qt DLLs are found first.

### 4.3 Configure licensing and Diana

1. Install/configure the Hasp driver if the selected Diana release requires it.
2. Configure the license server or `hasplm.ini` as required by the local installation.
3. Unpack the Diana distribution under the directory referenced by `DIAROOT`.
4. Verify the required Hasp DLL is present in Diana `release\bin` and `debug\bin`.
5. Provide the Open Inventor `password.dat` path through `VSG_LICENSE_FILE`.
6. Check the registry entries described in `geomec/INSTALL.txt`, especially `DIAPATH`, `DIASHARE`, `DianaExeName`, `FgvExeName`, and `InputExeName`.

### 4.4 Compile with Visual Studio

Use the root solution:

```text
Geomec.sln
```

In Visual Studio:

1. Open `Geomec.sln`.
2. Select platform `x64`.
3. Select `Debug`, `ReleaseWithDebug`, or `Release`.
4. Build the solution.
5. Set `Geomec` as the startup project when debugging the desktop application.
6. For Debug and Release debugging, set the executable environment to include the paths represented by `$(PATHPREFIX);$(PATH);$(OIVPATH)`.

The solution includes the main application, FW51 projects, library projects, auxiliary applications, and tests. Individual `.vcxproj` files can be built with MSBuild when a solution-wide build is unnecessary. A representative command from a Visual Studio developer command prompt is:

```bat
msbuild Geomec.sln /m /p:Configuration=Release /p:Platform=x64
```

Use the Visual Studio version installed for the solution. Older projects may require project conversion or the original v140 toolset.

### 4.5 Windows deployment

The installer projects are under `wix/`, including:

- `WixInstall32` and `WixInstall64` for 32-bit/64-bit installation projects.
- `WixBundledInstall` for bundled installation.
- `WixPostInstall` and `WixHaspPostInstall` for post-install actions.
- `WixInclude` for shared installer fragments and helper scripts.

A deployment must contain more than `Geomec.exe`:

- The matching Geomec executable and its dependent DLLs.
- The correct Qt and Qwt runtime DLLs.
- Open Inventor runtime DLLs and license configuration.
- Diana runtime binaries and the Diana `share` data directory.
- FW51 and Geomec libraries required by the executable.
- HDF5/TBB/Blosc/zlib or other runtime libraries used by the build.
- FlexLM/Hasp licensing components where applicable.
- Configuration, registry entries, resources, help, and version files.

Use the WiX project for a repeatable installer rather than manually copying DLLs. Test the installed application on a machine without the developer environment, because a successful developer-machine run can accidentally rely on `PATH` entries that are not part of the deployment.

## 5. Linux Build

The Linux path is based on qmake and Makefiles. It is less self-contained than the Windows solution and assumes an internal Diana/toolchain layout.

### 5.1 Linux prerequisites

`README.linux` requires an environment variable named `GM_SW_TOOL_PATH`. The directory is expected to contain these subdirectories or links:

```text
$GM_SW_TOOL_PATH/diana       Diana distribution: release, debug, share
$GM_SW_TOOL_PATH/GCC         GCC 4.7.2 or newer, including include and lib64
$GM_SW_TOOL_PATH/extra       UUID library, including include/uuid and lib
$GM_SW_TOOL_PATH/qt-4.8.5   Qt installation
```

The qmake files also reference or expect values such as:

```text
DIAPATH       Diana build/runtime path
DIASRC        Diana source/share source path
QTDIR         Qt path
INTEL         Intel runtime path, if used by the local Diana build
BOOST         Boost installation
LD_LIBRARY_PATH  Must include Diana and required compiler/runtime paths
```

Run the repository check script before configuring:

```sh
cd /path/to/Geomec
. ./qmake/check.environment.sh
```

The script currently reports missing variables but does not fail the shell because its `exit 1` lines are commented out. Treat any report as a real configuration error.

### 5.2 Configure and compile the qmake graph

The intended entry point is `geomec.pro`, not the smaller legacy `app.pro`:

```sh
cd /path/to/Geomec
qmake -r geomec.pro CONFIG+=release BUILD_TYPE=release
make -j"$(nproc)"
```

For a debug build:

```sh
qmake -r geomec.pro CONFIG+=debug BUILD_TYPE=debug
make -j"$(nproc)"
```

If the qmake version does not understand the project syntax or the expected Qt modules, use the Qt/qmake version associated with the target configuration. The qmake files use `CONFIG += debug_and_release` and generate output under a build-specific `bin/<configuration>` tree. `qmake/destination.pri` places intermediate objects under `.obj/<configuration>` and libraries under the corresponding output tree.

The qmake graph builds libraries before applications. The principal shell target links the Geomec libraries, FW51 libraries, Diana libraries, HDF5, Blosc, TBB, GLib, zlib, Boost, `dl`, and `rt` on Unix. The exact link set is defined in `geomec_shell/geomec_shell.pro`, `gm_diana_app/gm_diana_app.pro`, `qmake/libs.pri`, and the included `.pri` files.

### 5.3 Linux build caveats

The Linux configuration is not a portable modern distribution build:

- `qmake/basepath.pri` contains site-specific paths such as `/glb/data/...` and a RHEL/EasyBuild GLib path.
- Several variables must be supplied by the local Diana/toolchain installation.
- The checked-in Linux documentation describes Qt 4.8.5, while parts of the source use Qt modules and project settings associated with newer Qt versions.
- `IntegrationTests/Tests/make.sh` contains old hard-coded target names and paths and should be treated as a historical helper, not as the primary build entry point.

Before attempting a full build, update `qmake/basepath.pri` or provide compatible paths through a local configuration, then verify that the selected Qt, compiler ABI, Diana libraries, HDF5, TBB, Boost, GLib, and Python runtime all match.

### 5.4 Linux deployment

There is no complete, self-contained Linux installer definition comparable to the WiX projects. Deployment should be assembled as a directory containing:

```text
bin/                  Built executable(s)
lib/                  Built Geomec/FW51 shared libraries, if built shared
diana/                Matching Diana release/debug runtime and share data
Qt runtime            Qt libraries/plugins for the selected Qt build
HDF5/TBB/Blosc/etc.   Runtime shared libraries required by ldd
resources             Geomec resources, scripts, and configuration
license files         Diana/Open Inventor/licensing files as applicable
```

After building, inspect dependencies with:

```sh
ldd path/to/geomec_shell
ldd path/to/gm_diana_app
```

Set `LD_LIBRARY_PATH` for the deployed layout, or install the shared libraries in a standard loader location and add an appropriate rpath. A deployment should not depend on the developer's `/glb/data` paths.

## 6. Testing and Smoke Checks

The repository includes unit and integration projects under `Tests/`, plus large model files under `TestData/`.

For a Windows smoke test, the historical instructions recommend building x64 and running the application with `TestData\\TestLoadAndCalculate.gm4`, then trying a small nonlinear analysis.

For Linux, build the relevant test targets from the qmake graph after the libraries are available. Do not assume every test is portable: some tests depend on Diana, licensing, external model files, Python, or platform-specific runtime libraries.

## 7. Recommended Build Order

For either platform, use this order when diagnosing failures:

1. Verify compiler, Qt, Diana, Open Inventor, licensing, and runtime environment.
2. Build low-level third-party and utility libraries.
3. Build FW51 libraries.
4. Build Geomec domain and Diana integration libraries.
5. Build the selected executable (`Geomec`, `geomec_shell`, or a specialized app).
6. Build/run the smallest relevant test target.
7. Assemble deployment files and test on a clean machine or container.

## 8. Files to Read First

- Main qmake graph: `geomec.pro`
- Linux prerequisites: `README.linux`
- Windows prerequisites and runtime setup: `geomec/INSTALL.txt`
- Windows solution: `Geomec.sln`
- Main Windows project: `geomec/Geomec.vcxproj`
- Windows environment macros: `geomec/Environment.props` and `vsprops/BuildCommon.props`
- qmake dependency list: `qmake/libs.pri`
- qmake paths and output directories: `qmake/globals.pri`, `qmake/basepath.pri`, and `qmake/destination.pri`
- Installer projects: `wix/`

## 9. Current Repository Limitations

This source tree is a historical multi-platform codebase rather than a clean reproducible build environment. The Windows solution and WiX projects provide the clearest production path. The Linux qmake path is useful as a build reference, but it requires adaptation of site-specific paths and careful matching of old dependency versions. A future maintenance improvement would be to add explicit per-platform dependency manifests, remove hard-coded infrastructure paths, and provide a reproducible Linux packaging script.