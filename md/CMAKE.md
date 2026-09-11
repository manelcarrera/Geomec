# Porting to CMake — Feasibility Notes

Reference notes on whether/when to port this repo's two build graphs
(qmake `.pro`/`.pri` and MSBuild `.vcxproj`/`.sln`/`.props`) to CMake, the
de facto standard build-system generator. This is a feasibility opinion,
not a plan — nothing has been ported.

## Table of contents

1. [Is it technically possible?](#1-is-it-technically-possible)
2. [The main risk: porting a build you can't validate](#2-the-main-risk-porting-a-build-you-cant-validate)
3. [Effort compared to the folder restructuring](#3-effort-compared-to-the-folder-restructuring)
4. [Recommendation](#4-recommendation)

## 1. Is it technically possible?

Yes. CMake can express everything both existing build graphs do here:

- MFC (used by `platform/MfcGuiFrame`, `platform/MfcModelFrame`, etc.) via
  `CMAKE_MFC_FLAG`.
- Qt (used throughout) via `find_package(Qt5 ...)` plus `AUTOMOC`/`AUTOUIC`
  — CMake's Qt integration is mature and well-trodden.
- Diana, Open Inventor, and Qwt would each need a hand-written
  `Find<X>.cmake` module, since none of them ship an official CMake config
  — this repo's existing `$(LBROOT)`/`$(OIVHOME)`/`$(QTDIR)`-style
  environment variables map fairly directly onto what such a module would
  look for.

## 2. The main risk: porting a build you can't validate

Per `GETTING_STARTED_MODERN_TOOLCHAIN.md` §8, neither existing build path
currently compiles in the environment this work has been done in — Windows
can't be attempted at all (no Visual Studio here), and the Linux/qmake path
is missing Diana entirely plus carries an unverified TBB/GCC ABI risk.

A build-system port is only known to be *correct* by building the result
and comparing behavior against the original. Without a working baseline to
compare against, porting ~100 project files' worth of include paths,
preprocessor defines, and link order would be done blind, with no way to
catch a mistake until much later (or never, if it silently changes what
gets compiled in).

## 3. Effort compared to the folder restructuring

Meaningfully bigger, and a different kind of work:

- The folder restructuring done earlier in this project was mechanical —
  move a directory, fix a path string, verify by grepping for the old
  path. Low ambiguity, cheap to check.
- A CMake port means re-deriving actual compile/link semantics per module
  — not just where a file lives, but exactly what it's compiled with.
- The two existing build graphs already **disagree with each other** in
  places already flagged in `BUILD_AND_ARCHITECTURE.md`: Qt4 (per
  `geomec/INSTALL.txt`) vs. Qt5 (per `Geomec.vcxproj`), and VS2012 (per
  `INSTALL.txt`) vs. the `v140` toolset actually used. CMake would force a
  decision on each such ambiguity rather than let it stay latent — useful
  in the long run, but real work, not mechanical translation.

## 4. Recommendation

Get the Windows build compiling for real first — it's the more complete
of the two existing paths per `BUILD_AND_ARCHITECTURE.md`. Port to CMake
*after* that, with a working baseline to validate every ported piece
against, rather than attempting the port first and hoping it's equivalent.
