# Repository Layout

What each top-level folder is for, and whether it holds code/content we
maintain ("Ours") or vendored material we don't ("Third-party"). See
`BUILD_AND_ARCHITECTURE.md` for build details and `FOLDER_STRUCTURE_PROPOSAL.md`
for how this layout came to be.

## Table of contents

1. [Folder table](#1-folder-table)
2. [Notes](#2-notes)

## 1. Folder table

| Folder | Ownership | What it is |
| --- | --- | --- |
| `geomec/` | Ours | Main Windows Geomec desktop application and GUI code (`Geomec.vcxproj`). |
| `fw51/` | Ours | Framework 51 — shared geometry, mesh, tensor, and model-frame library used across the app. |
| `lib/` | Ours | First-party libraries shared by the main app and other executables, that build on both Windows and Linux (e.g. `GeomecModel`, `DianaCore`, `MaterialLibrary`, `Quantity`). |
| `platform/` | Ours | First-party libraries with no Linux/qmake project today, i.e. Windows-only (MFC-based `MfcGuiFrame`/`MfcModelFrame`, `WindowsRegistry`, `qtwinmigrate`, `GmDatabase`, etc.). |
| `app/` | Ours | Auxiliary applications and legacy application projects (`geomec_launcher`, `diana_app`, `geomec_rgi`, `geomec_cora`, `MPKernelApp`, and others). |
| `geomec_shell/` | Ours | Console/shell executable variant of Geomec. |
| `gm_diana_app/` | Ours | Diana-oriented application executable. |
| `geomec_cora_wrapper/` | Ours | CORA integration wrapper. |
| `geomec_selector/` | Ours | Selector application. |
| `Tests/` | Ours | C++ test source (`TestLib`, `TestRunner`, `Fw51`, `Materials`, `ModelOperations`, `SanityChecks`, `app`). |
| `TestData/` | Ours | Test fixtures — model/input data files the tests load (not source code). |
| `IntegrationTests/` | Ours | Script-driven (shell/Python) integration tests. |
| `Prototypes/` | Ours | Experimental/unfinished work (`ModularGeomec`). Still wired into the qmake build via one `SUBDIRS` target. |
| `archive/` | Ours (dead) | Historical code no longer part of any build target (the legacy `DSealect` application). Kept for reference only. |
| `plugins/` | Ours | Distributable plugin packages/snapshots (e.g. zipped Inversion Tool releases) and related notes. |
| `py/` | Ours | Python tools and integration helpers (`InversionTool`, `DependenciesParser`, `autobuild`). |
| `img/` | Ours | Icons and images used by the application UI. |
| `version/` | Ours | Version header and version metadata. |
| `changelog/` | Ours | Per-release changelog pages (HTML + screenshots) shown to end users. |
| `res/` | Ours | Build, source-control, and utility scripts (`autobuild`, `setenv`, old `svn-tools`). |
| `qmake/` | Ours | Shared qmake configuration and dependency definitions for the Linux build graph. |
| `vsprops/` | Ours | Visual Studio property sheets (shared MSBuild configuration) for the Windows build. |
| `wix/` | Ours | Windows installer projects (WiX). The `.wxs`/`.wixproj` project files are ours; the WiX *toolset* itself is a third-party tool invoked at build time, not vendored here. |
| `md/` | Ours | Repository-level documentation (this file, `BUILD_AND_ARCHITECTURE.md`, `FOLDER_STRUCTURE_PROPOSAL.md`, `ACTIONS_LOG.md`). |
| `third_party/` | **Third-party** | Vendored SDKs and installers we don't maintain: TBB, HDF5, zlib, Blosc (`c-blosc-1.9.0`), Google Test/Mock, plus CORA, SVS, .NET redistributable, and bitmap assets. |

## 2. Notes

- `blosc_filter/` and `qtiocompressor/` live under `lib/` (ours), not
  `third_party/` — they're small first-party wrapper/integration code
  around third-party formats (Blosc, Qt), not vendored library source
  themselves.
- Root-level files (`geomec.pro`, `Geomec.sln`, `app.pro`, `README.linux`,
  `.editorconfig`, `.clang-format`, `Geomec.code-workspace`, etc.) are all
  ours — project/build configuration, not vendored.
