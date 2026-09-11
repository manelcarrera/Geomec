# Actions Log

A running log of actions taken on this repository and this machine's dev
environment during work with Claude. Kept up to date after every action —
newest entries at the bottom of each session's section.

## Table of contents

1. [Session 1](#1-session-1)
   - 1.1 [Repository review](#11-repository-review)
   - 1.2 [Repository restructuring (all 4 phases implemented)](#12-repository-restructuring-all-4-phases-implemented)
   - 1.3 [Indentation normalization](#13-indentation-normalization)
   - 1.4 [Commit and push](#14-commit-and-push)
   - 1.5 [VS Code editor settings (this machine, not the repo)](#15-vs-code-editor-settings-this-machine-not-the-repo)
   - 1.6 [C/C++ IntelliSense (#include errors)](#16-cc-intellisense-include-errors)
   - 1.7 [Qt5 installed for IntelliSense](#17-qt5-installed-for-intellisense)
   - 1.8 [This log](#18-this-log)
   - 1.9 [Repository layout reference](#19-repository-layout-reference)
   - 1.10 [Consolidated documentation into `md/`](#110-consolidated-documentation-into-md)
   - 1.11 [Third-party dependencies reference](#111-third-party-dependencies-reference)
   - 1.12 [Documentation formatting standard](#112-documentation-formatting-standard)
   - 1.13 [Modern toolchain / trial-license research](#113-modern-toolchain--trial-license-research)
   - 1.14 [Found real license artifacts checked into the repo](#114-found-real-license-artifacts-checked-into-the-repo)
   - 1.15 [Compile-readiness check on this machine](#115-compile-readiness-check-on-this-machine)
   - 1.16 [CMake migration feasibility notes](#116-cmake-migration-feasibility-notes)
   - 1.17 [Which dependencies could modern C++ replace](#117-which-dependencies-could-modern-c-replace)
   - 1.18 [MFC modernization feasibility notes](#118-mfc-modernization-feasibility-notes)
   - 1.19 [Why each dependency vs. the standard library, with numbers](#119-why-each-dependency-vs-the-standard-library-with-numbers)
   - 1.20 [Added "better option available today" column](#120-added-better-option-available-today-column)
   - 1.21 [Clarified TBB's replacement is C++17, not C++20](#121-clarified-tbbs-replacement-is-c17-not-c20)
   - 1.22 [Created Spanish (ES) translations of all md/ files](#122-created-spanish-es-translations-of-all-md-files)
   - 1.23 [Full Visual Studio vs. Build Tools](#123-full-visual-studio-vs-build-tools)

## 1. Session 1

### 1.1 Repository review

- Read `BUILD_AND_ARCHITECTURE.md` and surveyed the existing folder layout,
  file sizes, and how the Windows (`vsprops`/MSBuild) and Linux (`qmake`)
  build graphs resolve paths.
- Wrote `FOLDER_STRUCTURE_PROPOSAL.md`: assessment of the original layout's
  problems (vendored binaries mixed into `lib/`, test data mixed into
  `Tests/`, a case-only `Tests/` vs `test/` collision, orphaned legacy code)
  and a proposed restructuring, offered as 4 phases.

### 1.2 Repository restructuring (all 4 phases implemented)

- **Phase 1** — Split `lib/` into first-party `lib/` and new `third_party/`
  (vendored SDKs: `tbb`, `hdf5`, `gmock-1.7.0`, `zlib-1.2.6`,
  `c-blosc-1.9.0`, `pstl`); merged the old root `3rdParty/` into
  `third_party/` (`CORA1`, `SVS`, `dotNet`, `point_elementsets_bitmaps`).
- **Phase 2** — Split `Tests/` into source-only `Tests/` and new
  `TestData/` (was `Tests/Models`, ~420MB of binary model/input data);
  renamed the lowercase `test/` to `IntegrationTests/` to remove a
  directory pair differing only by case.
- **Phase 3** — Moved the inert, build-unreferenced legacy `old/` tree
  (historical `DSealect` app) into `archive/old/`. `Prototypes/` was
  deliberately left in place (still an active qmake `SUBDIRS` target with
  depth-sensitive relative paths — see `FOLDER_STRUCTURE_PROPOSAL.md`).
- **Phase 4** — Carved new `platform/` out of `lib/` for the 15 modules
  with no qmake `.pro` project (Windows-only): `CutSurfaceUtil`,
  `DoubleEditLib`, `FemAppGUI`, `GlobalMessage`, `GmDatabase`, `IT`,
  `MPKernel`, `MfcGuiFrame`, `MfcModelFrame`, `OpenInventorDataInterface`,
  `Plugins`, `PyLib`, `WellSchemeUtils`, `WindowsRegistry`, `XMLLib`,
  `qtwinmigrate`.
- Updated every `.pro`/`.pri`, `.vcxproj`/`.props`, `.sln`, WiX
  (`.wixproj`/`.wxs`/`.wxi`), and shell/Python script reference found by
  static search to point at the new locations. Introduced
  `THIRDPARTYROOT`, `TESTDATAROOT`, `PLATFORMROOT` root variables
  alongside the existing `LBROOT`/`TESTSROOT` pattern, defined once each
  in `qmake/globals.pri` and `vsprops/Environment.props`.
- Updated `BUILD_AND_ARCHITECTURE.md` and `FOLDER_STRUCTURE_PROPOSAL.md`
  to document the structure as implemented, including the deviations from
  the original diagram (no `src/` wrapper, `TestData`/`IntegrationTests`
  as top-level dirs, `Prototypes/` left in place).
- **Not verified with an actual build** — this environment has neither
  the Windows toolchain (Visual Studio, Diana, Qt, Open Inventor, WiX)
  nor a fully configured Linux/Diana build environment.

### 1.3 Indentation normalization

- Reindented 3,431 of 4,190 first-party `.c`/`.cpp`/`.h`/`.hpp`/`.cc`
  files (`lib/`, `platform/`, `geomec/`, `fw51/`, `app/`, `geomec_shell/`,
  `gm_diana_app/`, `geomec_cora_wrapper/`, `geomec_selector/`, `Tests/`,
  `IntegrationTests/`, `Prototypes/`) from a mix of tabs and 4-space
  indentation to 2-space indentation. Only leading whitespace was
  touched; `third_party/` and `archive/` were left untouched. Line
  endings (LF/CRLF/mixed) were preserved exactly.
- Added `.editorconfig` (`indent_style = space`, `indent_size = 2`,
  `tab_width = 2`) at the repo root.
- Added `editor.tabSize: 2`, `editor.insertSpaces: true`,
  `editor.detectIndentation: false` to `Geomec.code-workspace`.

### 1.4 Commit and push

- Committed everything above as a single commit and pushed to
  `origin/main` (`git@github.com:manelcarrera/Geomec.git`):
  `36f1df8` "Restructure repository layout and normalize indentation to
  2 spaces" (10,400 files changed).

### 1.5 VS Code editor settings (this machine, not the repo)

- Checked for an existing `editor.fontSize` setting — none found
  anywhere (global, workspace, or `.vscode/settings.json`); VS Code was
  using its 14px default.
- Created `~/.vscode-server/data/User/settings.json` with
  `editor.fontSize: 12`, later changed to `11`, then `10` (current value)
  per follow-up requests.
- User reported the font size change wasn't visibly applying. Confirmed
  the setting file itself was correct and the running VS Code Server
  process was reading from that exact `--user-data-dir`. Suggested likely
  causes outside this filesystem's visibility: a per-window zoom level
  (`Ctrl+Scroll`/`Ctrl+=`) masking the change, or needing "Developer:
  Reload Window" — recommended trying "View: Reset Zoom" first.

### 1.6 C/C++ IntelliSense (`#include` errors)

- Investigated a reported cpptools error ("#include errors detected...")
  on `fw51/FieldValueFile/Base/FvGocadFile.cpp`. Found no
  `c_cpp_properties.json` existed, so cpptools had no explicit
  `compilerPath`/standard configured.
- Along the way, discovered `.vscode/settings.json` and `.clang-format`
  now exist in the repo (not created by this session) with
  `editor.formatOnSave: true` using `clang-format` (LLVM style,
  `IndentWidth: 2`) for C/C++/Objective-C(++) files. Flagged to the user
  that this has already reformatted some files well beyond the
  whitespace-only reindentation done above (e.g. a 503-line diff on
  `Tests/TestLib/src/TestLib.cpp` against the pushed commit) — left as-is
  pending the user's decision on whether to keep it.
- Created `.vscode/c_cpp_properties.json` with `compilerPath:
  /usr/bin/g++`, `cStandard: c17`, `cppStandard: c++14`,
  `intelliSenseMode: linux-gcc-x64`, keeping the existing
  `${workspaceFolder}/**` includePath. Noted this fixes IntelliSense for
  cross-platform files but not ones needing Qt/Diana/MFC/Open
  Inventor/Windows headers, which aren't installed in this sandbox.

### 1.7 Qt5 installed for IntelliSense

- User asked whether installing Qt was necessary to clear Qt-related
  `#include` warnings (`fw51/FieldValueFile/Base/FvTextFile.cpp` and
  others: `<QTextStream>`, `<qfile.h>`, `<qfileinfo.h>`). Explained it's
  optional (cosmetic, doesn't affect correctness) and gave three options:
  install Qt, leave the warnings, or suppress error squiggles without
  installing anything.
- Recommended installing Qt (`qtbase5-dev`) since Qt is used pervasively
  across the GUI code, versus a blanket suppression that would also hide
  real problems in Qt-independent files.
- Attempted `sudo apt install -y qtbase5-dev` — failed (interactive sudo
  authentication not available in this tool environment). Asked the user
  to run it themselves.
- User confirmed done. Verified `qtbase5-dev`/`qtbase5-dev-tools`
  installed (Qt 5.15.18) with headers under
  `/usr/include/x86_64-linux-gnu/qt5/`.
- Updated `.vscode/c_cpp_properties.json` to add
  `/usr/include/x86_64-linux-gnu/qt5` and
  `/usr/include/x86_64-linux-gnu/qt5/**` to `includePath`.

### 1.8 This log

- Created this file (`ACTIONS_LOG.md`) at the user's request, to be kept
  updated after every action taken going forward.

### 1.9 Repository layout reference

- Created `REPO_LAYOUT.md`: a table of every top-level folder, what it's
  for, and whether it's first-party ("Ours") or vendored ("Third-party").
  Only `third_party/` is marked third-party; `wix/` is noted as ours
  (project files) even though the WiX toolset itself is a third-party
  tool invoked at build time.

### 1.10 Consolidated documentation into `md/`

- Moved all root-level documentation into a new `md/` folder:
  `BUILD_AND_ARCHITECTURE.md`, `FOLDER_STRUCTURE_PROPOSAL.md`,
  `ACTIONS_LOG.md` (this file), `REPO_LAYOUT.md`. Third-party README/docs
  vendored inside `third_party/` were left where they are — this only
  covers our own root-level docs. Updated `REPO_LAYOUT.md`'s folder table
  to include `md/`.

### 1.11 Third-party dependencies reference

- Created `THIRD_PARTY_DEPENDENCIES.md`: every bundled (`third_party/`)
  and non-bundled third-party dependency, plus Microsoft-specific
  libraries/tools (MFC, Windows SDK, .NET Framework, WiX, MSVC toolset),
  each with what it's for, whether it needs a commercial license,
  how to get/build it, and where the build expects it to live. Flagged
  `CORA1` and `SVS` as needing manual license verification since no
  license file or clear vendor string could be found for either.

### 1.12 Documentation formatting standard

- Added a numbered table of contents to every file in `md/`
  (`BUILD_AND_ARCHITECTURE.md`, `FOLDER_STRUCTURE_PROPOSAL.md`,
  `ACTIONS_LOG.md`, `REPO_LAYOUT.md`, `THIRD_PARTY_DEPENDENCIES.md`) and
  numbered all their sections/subsections, per the user's standing
  request that all `md/` files carry a TOC and numbered sections —
  apply this same formatting to any new `md/` file going forward.

### 1.13 Modern toolchain / trial-license research

- Researched (via web search) whether OpenGL/Open Inventor/Diana need
  commercial licenses, current Open Inventor vendor (Thermo Fisher
  Scientific, via the VSG → FEI lineage) and its Qt integration (`SoQt`),
  current Open Inventor VS-compatibility data point (2023.2 needs a
  VS2017-compatible runtime), and trial-request processes for both Open
  Inventor and Diana FEA (both sales-mediated, not instant self-serve).
- Created `GETTING_STARTED_MODERN_TOOLCHAIN.md`: addresses whether Diana
  is avoidable if no model will be run (SDK/link-time dependency likely
  still required even without a runtime solve license), clarifies Open
  Inventor needs both a compatible VS toolset *and* Qt (not a choice
  between them), a version-dependency matrix across VS/Qt/Open
  Inventor/Diana/the already-bundled `third_party/` libs, and a
  recommended path given the user's goal (visualization only, no solving).
  Flagged as unverified planning, not an attempted build.

### 1.14 Found real license artifacts checked into the repo

- Searched the repo for existing license files and found three real (all
  expired) Diana keyfiles naming "Shell" / "Shell Research bv KSEPL" as
  customer (`geomec/keyfile/Linux/diafgv.k72` and its dated sibling,
  `archive/old/DSealect/keyfile/diafgv.k72`), plus
  `wix/WixInclude/password.dat` — not a key but a FlexLM-style network
  license-server pointer (`SERVER muntse-s-09786.europe.shell.com ANY
  3015`) for Open Inventor.
- Flagged to the user that these are real corporate artifacts (customer
  name + internal hostname) already pushed to their personal GitHub, and
  that deleting the working-tree files wouldn't remove them from git
  history — a history rewrite would be needed for that, not attempted.
- Documented both findings in `GETTING_STARTED_MODERN_TOOLCHAIN.md`
  §5 ("Existing license artifacts found in this repo"), renumbering the
  file's later sections (6–8) to make room.

### 1.15 Compile-readiness check on this machine

- Checked what's actually installed on this WSL2 Linux machine against
  what each build path needs: `g++` 15.2.0, `qmake` 3.1 (Qt 5.15.18), and
  Python 3 are present; Boost, GLib dev headers, and `uuid-dev` are not;
  Diana itself (not just a license) is entirely absent.
- Confirmed `geomec/OpenInventor` has no qmake `.pro` project at all
  (zero references in `geomec.pro`) — Open Inventor visualization is
  Windows-only in this codebase, so the stated visualization goal can't
  be pursued on Linux regardless of any other fix.
- Found a concrete, verified ABI risk independent of licensing: the
  bundled `third_party/tbb/linux` binaries are prebuilt for
  `gcc4.1`/`gcc4.4`/`gcc4.7` (visible in their own path names), a huge
  generation gap from this machine's GCC 15, spanning the libstdc++ dual-ABI
  break at GCC 5.
- Documented all of this in `GETTING_STARTED_MODERN_TOOLCHAIN.md` as new
  §8 ("Current environment readiness check"), renumbering "Open risks" to
  §9 and "Sources" to §10 (also newly numbered, for consistency with the
  rest of the file).

### 1.16 CMake migration feasibility notes

- User asked whether qmake/MSBuild could be ported to CMake. Answered:
  technically yes (MFC via `CMAKE_MFC_FLAG`, Qt via `find_package`,
  custom `Find<X>.cmake` modules needed for Diana/Open Inventor/Qwt since
  none ship CMake configs), but recommended against doing it now —
  neither existing build path currently compiles in this environment
  (per §1.15), so a port would have no working baseline to validate
  against. Recommended getting the Windows build compiling for real
  first, then porting with something to check the result against.
- Created `CMAKE.md` with these feasibility notes for future reference.

### 1.17 Which dependencies could modern C++ replace

- User asked whether any third-party dependencies (TBB as the example)
  could be replaced by C++20 features. Researched current compiler
  support before answering (GCC's C++17 parallel algorithms are
  implemented using TBB internally — merely including `<execution>`
  creates a link-time TBB dependency on GCC/libstdc++, unlike MSVC's STL
  which has its own backend; C++20 `<chrono>` timezone/calendar support
  reached full support in GCC 14.2 and MSVC VS2019 16.10+).
- Added new §4 ("Which of these could modern C++ replace?") to
  `THIRD_PARTY_DEPENDENCIES.md`: `pstl` is largely obsolete (superseded by
  native C++17 `<execution>` support), `boost_date_time` is replaceable by
  C++20 `<chrono>` on a current-enough compiler, TBB is only partially
  replaceable (and not at all on GCC/Linux, where the standard parallel
  algorithms themselves depend on TBB) with its concurrent
  containers/flow graph/allocator having no standard equivalent at any
  C++ version, and everything else (HDF5, zlib/Blosc, gtest, Qt/Qwt/MFC,
  Open Inventor, Diana, FlexLM/Hasp, CORA1/SVS/dotNet/WiX) solves problems
  outside the C++ standard's scope entirely. Renumbered "Notes" to §5 and
  added a new §6 ("Sources") citing the compiler-support research.

### 1.18 MFC modernization feasibility notes

- User asked how complex moving off MFC would be and what the target
  should be. Quantified actual MFC depth by grepping the codebase rather
  than guessing: the app shell itself is MFC (`CWinApp` in
  `geomec/src/Geomec.cpp`, `CMainFrame` in `MainFrm.cpp`), 186
  `BEGIN_MESSAGE_MAP` blocks, 98 `CDialog`/`CWnd`-derived classes, and 51
  files inside `geomec/OpenInventor` itself use MFC (the 3D viewport is
  MFC-hosted, no `SoQt`/`SoWin` usage found).
- Found `platform/qtwinmigrate` is a known MFC↔Qt migration-bridge
  library already in use, plus 7 files already using `QSettings` next to
  8 still using `platform/WindowsRegistry` — evidence a partial
  MFC→Qt migration was already started once. Concluded Qt (not a new
  framework) is the natural target, since it's already used everywhere
  else in the app.
- Created `MFC_MODERNIZATION.md` with the depth findings, the
  dependency-ordered migration steps (dialogs → OIV viewport via `SoQt` →
  `WindowsRegistry`→`QSettings` → app shell → retire
  `qtwinmigrate`/`MfcGuiFrame`/`MfcModelFrame`), and a scale/effort note
  (not scriptable, multi-month-to-multi-year, dialog-by-dialog work).

### 1.19 Why each dependency vs. the standard library, with numbers

- User asked for the reasoning behind each dependency choice vs. the C++
  standard library, with quantitative/speed data where possible (HDF5
  named as the example). Researched published benchmark figures rather
  than estimating: HDF5 reaches ~70–95% of raw filesystem throughput on
  large tuned writes (5–30% overhead) but can be 2–3× slower than a raw
  array format when poorly tuned; Blosc compresses at up to ~8 GB/s
  (faster than memcpy's ~7.5 GB/s) and decompresses up to ~35–50 GB/s
  with PGO, its Zstd codec beating zlib's own ~4 GB/s peak by over 2×;
  TBB's scalable allocator typically gives 20–30% faster parallel
  sections (up to ~4× in extreme cases, one Xeon Phi case up to 56×).
- Added new §5 ("Why each dependency was used instead of the standard
  library") to `THIRD_PARTY_DEPENDENCIES.md` as a table covering every
  dependency — quantitative for HDF5/TBB/Blosc-zlib, "does not exist as a
  standard alternative" for Qt/Qwt/Open Inventor/MFC/Diana/licensing
  tools, and "not applicable" for dev-time-only tools like Google
  Test/Mock. Renumbered Notes to §6 and Sources to §7, and added the new
  benchmark sources to §7.

### 1.20 Added "better option available today" column

- User asked to extend §5's table with a column calling out better/free
  options currently available, especially standard-library or
  open-source ones. Split the combined "WiX, CORA1, SVS, dotNet" row into
  separate rows (each has a different answer) and added the column for
  every dependency: e.g. oneTBB/oneDPL (TBB/pstl's current free
  successors), zlib-ng/Zstd/Blosc2 (beat the old bundled zlib
  1.2.6/c-blosc 1.9.0), C++20 `<chrono>` (free, replaces
  `boost_date_time`), Qt 6 (Qt5.15 standard support ended 26 May 2025,
  verified via search), VTK (free alternative to Open Inventor, not a
  drop-in), current WiX v7 (repo docs reference v3.7), and modern .NET
  replacing the bundled, long-retired .NET Framework 4.0. `SVS` and
  `CORA1` correctly got "no informed/no drop-in alternative" given the
  earlier unresolved-vendor and domain-specificity findings. Added the
  new sources (Qt EOL, .NET Framework lifecycle, WiX release notes) to §7.

### 1.21 Clarified TBB's replacement is C++17, not C++20

- User flagged that TBB's standard-library overlap had been described as
  C++17 in conversation but wanted it explicit in the doc too, since the
  original question was framed around C++20. The table in §4 already
  said C++17 correctly, so added a prominent clarifying paragraph above
  the table spelling out the distinction directly: TBB's overlap
  (`parallel_for`/`parallel_reduce`) is a **C++17** `<execution>` feature;
  C++20 added no new parallel algorithms at all, only lower-level
  concurrency primitives (`std::jthread`, `std::latch`, `std::barrier`,
  `std::counting_semaphore`) and the `<chrono>` timezone support relevant
  to replacing `boost_date_time` — none of which change the TBB picture.

### 1.22 Created Spanish (ES) translations of all md/ files

- Created `md/es/` and translated all 8 files (`ACTIONS_LOG.md`,
  `BUILD_AND_ARCHITECTURE.md`, `CMAKE.md`, `FOLDER_STRUCTURE_PROPOSAL.md`,
  `GETTING_STARTED_MODERN_TOOLCHAIN.md`, `MFC_MODERNIZATION.md`,
  `REPO_LAYOUT.md`, `THIRD_PARTY_DEPENDENCIES.md`) to Spanish, keeping
  code identifiers, file paths, and variable names in English, and
  regenerating each TOC's anchor links to match the translated headers.
  This is a one-time snapshot translation, not an ongoing dual-language
  sync — the `es/` copies will drift out of date as the English files
  keep changing (including this very log) unless re-translated on
  request.

### 1.23 Full Visual Studio vs. Build Tools

- User asked whether compiling for Windows requires installing the full
  Visual Studio IDE or something lighter. Verified via search that
  Visual Studio Build Tools (Microsoft's separate, IDE-less installer —
  MSBuild plus compiler/linker toolsets and individual components) is
  sufficient, since building this repo is just an `msbuild` invocation.
  Confirmed binary compatibility is maintained from v140 through the
  current v145 (Visual Studio 2026), and that MFC/ATL
  (`Microsoft.VisualStudio.Component.VC.ATLMFC`) and the v140 toolset are
  both selectable as individual components in Build Tools, not exclusive
  to the full IDE.
- Added new §7 ("Full Visual Studio, or just Build Tools?") to
  `GETTING_STARTED_MODERN_TOOLCHAIN.md`, covering the required
  components, the editing/debugging trade-offs of skipping the IDE, and
  that this only lightens the compiler piece (Diana/Open
  Inventor/Qwt/WiX are unaffected). Renumbered sections 7–10 to 8–11 and
  added the new Microsoft Learn sources to §11.
