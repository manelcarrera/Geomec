# Folder Structure Proposal

This document assesses the repository layout that existed before this
restructuring (as described in the original `BUILD_AND_ARCHITECTURE.md`) and
records what was actually changed. All four phases described below have now
been applied to the working tree — see "Status" at the end for what that
means and does not mean.

## 1. Was the original structure OK?

Not wrong, but it had real, measurable issues:

- **`lib/` mixes first-party code with vendored binaries at wildly different
  scales.** It holds ~50 first-party modules (mostly tens–hundreds of KB
  each, ~30MB total) alongside vendored third-party SDKs: `lib/tbb`
  (**330MB**), `lib/hdf5` (**130MB**), `lib/gmock-1.7.0` (10MB),
  `lib/zlib-1.2.6`, `lib/c-blosc-1.9.0`, `lib/pstl`. Nothing in the
  directory name distinguishes which is which.
- **`Tests/` is 423MB, and 421MB of that is `Tests/Models`** (binary
  model/input data). The actual test source (`TestLib`, `Tests`,
  `TestRunner`, `Fw51`, `Materials`, `ModelOperations`, `SanityChecks`)
  totals well under 1MB. There is also a *second*, differently-named tree,
  `test/` (lowercase), for script-driven integration tests — `Tests/` vs
  `test/` differing only by case is a real hazard on case-insensitive
  Windows/macOS checkouts.
- **Legacy/dead code lives inside the active tree**: ~20 `old/`
  subfolders nested inside live modules (`geomec/*/old`, `lib/*/old`), plus
  a root `old/` containing an entire historical sibling app (`DSealect`),
  plus `Prototypes/` for an abandoned effort. None of this is separated
  from what is actually built today.
- Net effect: of the repo's 1.7GB, **~475MB is vendored SDKs and ~421MB is
  test data** — over half the tree isn't source code at all, but sits at
  the same level as source.

**Good news that lowers the risk of fixing this:** the Windows
(`vsprops/BuildCommon.props`, `$(LBROOT)`) and Linux (`qmake/*.pri`,
`$$LBROOT`/`$$DLLROOT`) build graphs both resolve paths through a small
number of centrally-defined root variables, not hardcoded relative paths
scattered across every project file. A top-level reorg mainly touches a
handful of central files, not all ~100+ `.vcxproj`/`.pro` files.

**On Windows/Linux separation specifically**: most `lib/*` modules that
build on both platforms share one `h/`+`src/` tree, referenced by both a
`.pro` and a `.vcxproj` — genuinely shared cross-platform source. Splitting
it by OS would create needless duplication. Only a real subset is
Windows-only already (MFC-based: `MfcGuiFrame`, `MfcModelFrame`,
`WindowsRegistry`, `qtwinmigrate`, `GmDatabase`, etc.) — those just aren't
labeled as such today.

## 2. Structure as implemented

```text
Geomec/
├── lib/                     First-party Geomec/Diana/FW51 libraries that build
│                            on both platforms (unchanged location — the vendored
│                            modules were moved OUT rather than the first-party
│                            ones moved into a new src/ wrapper; see "Deviations")
├── platform/                First-party libraries with no qmake (.pro) project
│                            today, i.e. Windows-only: CutSurfaceUtil,
│                            DoubleEditLib, FemAppGUI, GlobalMessage, GmDatabase,
│                            IT, MPKernel, MfcGuiFrame, MfcModelFrame,
│                            OpenInventorDataInterface, Plugins, PyLib,
│                            WellSchemeUtils, WindowsRegistry, XMLLib, qtwinmigrate
├── third_party/             Vendored SDKs and installers, formerly split across
│                            lib/ and the root 3rdParty/: tbb, hdf5, gmock-1.7.0,
│                            zlib-1.2.6, c-blosc-1.9.0, pstl, CORA1, SVS, dotNet,
│                            point_elementsets_bitmaps
├── Tests/                   C++ test source only (TestLib, Tests, TestRunner,
│                            Fw51, Materials, ModelOperations, SanityChecks, app)
├── TestData/                The 421MB of binary model/input data, formerly
│                            Tests/Models
├── IntegrationTests/        Script-driven integration tests, formerly the
│                            lowercase test/ (renamed to remove the case-only
│                            collision with Tests/)
├── archive/
│   └── old/                 The root old/ directory (historical DSealect app),
│                            which had zero references from the live build
├── geomec/, fw51/, app/, geomec_shell/, gm_diana_app/,
│   geomec_cora_wrapper/, geomec_selector/, Prototypes/, wix/, qmake/,
│   vsprops/, py/, res/, version/, changelog/, img/, plugins/    unchanged
```

**Rationale:**
- `lib/` vs `third_party/` is the highest-value, lowest-ambiguity split —
  it separates ~25MB of code that is actually maintained here from ~570MB
  of SDKs that are not.
- `Tests/` vs `TestData/` does the same for the 423MB `Tests/` tree, and
  renaming `test/` to `IntegrationTests/` removes the risky case-only
  naming collision with `Tests/`.
- No Windows/Linux source duplication — only the genuinely Windows-only
  modules moved, into one clearly-labeled `platform/` folder; everything
  cross-platform stays as one tree.
- `archive/` gets clearly-dead legacy material out of the tree.

### Deviations from the original diagram

The structure actually implemented differs from the first draft in this
document in a few deliberate ways, discovered while doing the work:

- **No `src/` wrapper.** The original diagram nested `lib/`, `fw51/`,
  `geomec/`, etc. under a new top-level `src/`. That was dropped: both the
  qmake graph (`qmake/globals.pri`) and the Windows props
  (`vsprops/Environment.props`) compute paths to `lib/` via root variables
  (`LBROOT`, `$(LBROOT)`) with depth-relative shortcuts for some build
  contexts (e.g. `equals(TYPE,'lib'): LBROOT=..`). Adding one more level of
  nesting would have changed the hop-count those shortcuts assume, for a
  purely cosmetic gain. Vendored and Windows-only material was moved *out*
  of `lib/` into new top-level siblings (`third_party/`, `platform/`)
  instead — same depth, no relative-path arithmetic to redo.
- **`TestData/` and `IntegrationTests/` instead of `tests/data/` and
  `tests/integration/`.** Nesting test data and integration tests under a
  new lowercase `tests/` would have put `Tests/` and `tests/` side by side —
  reintroducing the exact case-only collision this change set out to
  remove. Both landed as top-level siblings instead.
- **`Prototypes/` was left in place**, not swept into `archive/`. Unlike
  root `old/` (zero references from the live build), `Prototypes/` is an
  active qmake `SUBDIRS` target (`ModGMBus`, wired from `geomec.pro`) with
  a depth-relative `ROOT` override (`contains(SPLIT,'Prototypes'): ROOT =
  ../../..`) tied to its current nesting depth. Moving it under `archive/`
  would have added a directory level, invalidating that override.
  `Prototypes/` is also already self-descriptive as experimental, so the
  labeling gain didn't justify the risk.
- **`build/windows/`, `build/linux/`, `tools/`, `docs/` were not created.**
  These were part of the original aspirational diagram but were never
  offered as one of the four implementation phases and were not applied.

## 3. What was done

All four phases were implemented in one pass:

1. Split `lib/` into `lib/` (first-party) and `third_party/` (vendored
   SDKs), including merging the old root `3rdParty/` into `third_party/`.
2. Split `Tests/` into `Tests/` (source) and `TestData/` (data), and
   renamed `test/` to `IntegrationTests/`.
3. Moved root `old/` into `archive/old/`.
4. Carved `platform/` out of `lib/` for the 15 modules that have no qmake
   project today.

For each, every `$(LBROOT)`/`$$LBROOT`-style root variable, hardcoded
relative path, `.sln` project entry, WiX installer reference, and shell/
Python script path that could be found by static search (qmake `.pro`/
`.pri`, MSBuild `.vcxproj`/`.props`, `.sln`, `.wixproj`/`.wxs`/`.wxi`, and
build/deploy scripts) was updated to point at the new locations. New root
variables were introduced alongside the existing `LBROOT`/`TESTSROOT`
pattern: `THIRDPARTYROOT`, `TESTDATAROOT`, and `PLATFORMROOT`, each defined
once in `qmake/globals.pri` and `vsprops/Environment.props`.

Deliberately left untouched: references embedded in already-dead project
formats superseded by `.vcxproj` (`.vcproj`, `.dsp`, `.dsw` — none of which
`Geomec.sln` references), the historical `old/DSealect` tree (whose
internal relative paths were already inconsistent with the current layout
before this change), a remote SVN checkout URL in
`geomec/script/get_shell_code.bat` (points at an external server's own
directory layout, unrelated to this local rename), and a couple of
comments referencing other people's historical absolute local paths
(`plugins/TEST`, a comment in `IntegrationTests/geomec_rgi/create_installation.py`).

## 4. Status

The moves and reference updates described above have been applied to the
working tree (not yet committed). **This has not been verified with an
actual build on either platform** — the environment this was done in has
neither the Windows toolchain (Visual Studio, Diana, Open Inventor, Qt,
WiX) nor a fully configured Linux qmake/Diana environment available, and
the existing setup already requires extensive site-specific environment
configuration to build at all (see the main `BUILD_AND_ARCHITECTURE.md`).
Every change was checked by static search — repeatedly grepping the whole
tree for the old paths after each edit — but that cannot substitute for an
actual compile on both platforms. **Before relying on this, build both the
Windows solution and the Linux qmake graph and fix anything a real build
surfaces that static search missed.**
