# Getting Up and Running with a Modern Toolchain

A reference for standing up a build using a **current Visual Studio** and
**trial licenses** for the commercial dependencies, aimed specifically at
the goal discussed with the user: get 3D model **visualization** working
(Open Inventor) without needing to actually **run/solve** a model (Diana).
This supplements, and does not replace, `BUILD_AND_ARCHITECTURE.md` (which
documents the original VS2012/OIV 9.3.1 path as the repo currently expects
it).

**Status: planning reference, not yet attempted.** Everything below is
sourced from vendor documentation as of September 2026 (see Sources at the
bottom) plus what this repo's own docs already say — it has not been
verified by an actual build.

## Table of contents

1. [The core question: is Diana actually avoidable?](#1-the-core-question-is-diana-actually-avoidable)
2. [Open Inventor: Visual Studio vs. Qt — a false choice](#2-open-inventor-visual-studio-vs-qt--a-false-choice)
3. [Version dependency matrix](#3-version-dependency-matrix)
4. [Trial licenses](#4-trial-licenses)
5. [Existing license artifacts found in this repo](#5-existing-license-artifacts-found-in-this-repo)
6. [Using a current Visual Studio instead of VS2012](#6-using-a-current-visual-studio-instead-of-vs2012)
7. [Full Visual Studio, or just Build Tools?](#7-full-visual-studio-or-just-build-tools)
8. [Recommended path](#8-recommended-path)
9. [Current environment readiness check](#9-current-environment-readiness-check)
10. [Open risks / unknowns](#10-open-risks--unknowns)
11. [Sources](#11-sources)

## 1. The core question: is Diana actually avoidable?

**Partially, not fully.** "Not going to run a model" reduces how much of a
Diana *license* is needed, but it likely doesn't remove Diana from the
**build**. Per `BUILD_AND_ARCHITECTURE.md`, the app links directly against
Diana SDK libraries at compile/link time — `lib/DianaCore`, `DianaGlue`,
`DianaSignals`, `DianaStartup` all wrap Diana headers/import libraries, and
`DIAROOT` (containing `release`/`debug`/`share`) is a hard build
prerequisite, not just a runtime one. So:

- To **compile** `Geomec.exe` as-is, you almost certainly still need the
  Diana **SDK** (headers + import libs) from `DIAROOT`.
- To **execute an actual analysis**, you'd need a full Diana **runtime**
  license — this is the part "not running a model" genuinely lets you skip.

**Action:** ask Diana FEA sales specifically for a dev/SDK-only evaluation
(build against the API, without a production solve license) — this is a
common distinction vendors make but isn't documented publicly; confirmed
by asking, not assumed.

## 2. Open Inventor: Visual Studio vs. Qt — a false choice

These aren't alternatives — Open Inventor needs **both**, at different
layers:

- **Visual Studio / MSVC toolset** = the compiler Open Inventor's prebuilt
  libraries were themselves built with. Windows C++ vendor SDKs are
  typically ABI-sensitive to the compiler version; you generally must
  build your own code with a compatible toolset to link against them
  without issues.
- **Qt** = the GUI/windowing toolkit Open Inventor renders *into*. Open
  Inventor ships a dedicated component (`SoQt`) specifically for embedding
  its 3D render area inside a Qt application — this repo's
  `geomec/OpenInventor` module is exactly that integration. Without Qt (or
  another supported windowing toolkit), Open Inventor has nothing to draw
  into.

So the real requirement is: **a Visual Studio version compatible with
whichever Open Inventor build you get, *and* a Qt version compatible with
both.**

## 3. Version dependency matrix

| Component | What this repo currently expects | What's confirmed about *current* releases |
| --- | --- | --- |
| Visual Studio / MSVC toolset | `geomec/INSTALL.txt` says VS2012 Premium Update 4; `.vcxproj` files actually use the `v140` toolset (≈ VS2015) — **the repo's own docs already flag this as an inconsistency** | VS2015 through VS2022 share a binary-compatible C++ runtime/STL ABI, so retargeting the toolset in `.vcxproj` (v140 → v143 for VS2022) is normally low-risk for *our own* code |
| Qt | `.vcxproj` links `Qt5Core`/`Qt5Widgets`/`Qt5Xml`; `INSTALL.txt` separately describes building Qt 4.8.5 — **also already flagged as inconsistent** in `BUILD_AND_ARCHITECTURE.md` | Current Open Inventor (2023.2-era) documentation references Qt 5.9 for its own demos; each Qt build is tied to a specific MSVC version, so Qt version and VS version must be chosen together |
| Open Inventor | Docs specify 9.3.1 for VS x64 (circa ~2013) | A current Open Inventor release (2023.2 or later) instead requires a VS2017-compatible C++ runtime — i.e. a *current* trial gives you a much newer SDK than this code was written against |
| Qwt | Version unspecified, "for Windows" plotting | Must be built/downloaded to match whichever Qt build you settle on |
| Diana | Version unspecified; ships its own `release`/`debug`/`share` tree, largely prebuilt | Compiler/toolset compatibility isn't publicly documented — ask Diana support directly |
| TBB, HDF5, zlib, Blosc (c-blosc), Google Test/Mock | Old pinned versions already vendored in `third_party/` (see `THIRD_PARTY_DEPENDENCIES.md`) | All portable, actively-maintained open-source C/C++ — these should rebuild cleanly against a current MSVC toolset with little fuss; the safe part of this whole puzzle |

**The practical consequence:** getting a *current* Open Inventor trial
does not just "plug in" to this ~2013-era codebase. The `geomec/OpenInventor`
integration code was written against the Open Inventor 9.3.1 API, and a
decade-plus of API evolution sits between that and a 2023.2+ release —
expect real porting work, not just a path/env-var change.

## 4. Trial licenses

Both commercial dependencies do offer evaluation access, mediated by a
sales/request form rather than instant self-serve download:

- **Open Inventor** — Thermo Fisher Scientific (current vendor, via the
  VSG → FEI → Thermo Fisher lineage) runs a trial-request form. It doesn't
  publish trial duration/scope publicly; that's disclosed after you submit
  the form and are contacted.
- **Diana** — DIANA FEA BV has a "Try DIANA" trial-request flow and
  separately distinguishes Commercial / Academic / Research license
  categories. Their sales team explicitly offers to tailor a proposal to
  specific needs — worth explicitly asking about a build-only/SDK
  evaluation per §1.

Both are worth pursuing in parallel given the sales-mediated turnaround
time — start the trial-request conversations before investing time in
retargeting the toolchain.

## 5. Existing license artifacts found in this repo

This confirms, from the actual repo contents, how each system authenticates
— and flags a real sensitivity issue.

**Diana — local keyfile, no network needed.** Three license keyfiles are
checked in, all expired, all naming the same real customer:

| File | Expires | Customer | Modules |
| --- | --- | --- | --- |
| `geomec/keyfile/Linux/diafgv.k72` | 31/3/2020 | Shell | Geomec |
| `geomec/keyfile/Linux/diafgv.k72.2019_03_31` | 31/3/2019 | Shell | Geomec |
| `archive/old/DSealect/keyfile/diafgv.k72` | 28/2/2014 | Shell Research bv KSEPL | Linear Static and Eigenvalue Analysis |

Diana's scheme (at least as configured here) is "present a valid local
keyfile" — no server contact required. All three are long expired and
worthless as licenses now.

**Open Inventor — network license server, not an embedded key at all.**
`wix/WixInclude/password.dat` contains:

```text
SERVER muntse-s-09786.europe.shell.com ANY 3015
USE_SERVER
```

A FlexLM-style pointer telling the OIV client to connect (TCP, not
literally HTTP, but the same idea — a network license-server handshake) to
a Shell-internal server. There's no key material here, and it's unreachable
outside Shell's network. **Whatever OIV trial you obtain will need its own
`password.dat`** pointing at wherever your license actually lives — a
vendor-hosted server, your own local server, or a dongle-based license,
depending what Thermo Fisher provisions.

**Sensitivity flag:** these are real (if expired) corporate license
artifacts naming an actual company (Shell) and an internal hostname, sitting
in a repo already pushed to a personal GitHub remote. Deleting the files
from the working tree would not remove them from git history — that needs
a separate, more careful history-rewrite operation if ever wanted. Not
touched as part of this document.

## 6. Using a current Visual Studio instead of VS2012

Two separate concerns:

1. **Retargeting this repo's own `.vcxproj` files** from the `v140`
   toolset to a current one (e.g. `v143` for VS2022) — mechanically
   straightforward (VS's own "Retarget Solution" flow), and low-risk for
   the first-party code given the shared ABI across VS2015–VS2022.
2. **Whatever Open Inventor/Diana SDK you actually obtain must itself
   support that toolset.** This is the real gate: the *old* 9.3.1 Open
   Inventor SDK was almost certainly built for a VS2012/2013-era toolset,
   which may not link cleanly against VS2022-compiled code (mixing STL
   types across a DLL boundary compiled by very different toolset
   generations is the classic failure mode). A *current* Open Inventor
   trial sidesteps this ABI question but reopens the API-porting question
   from §3.

## 7. Full Visual Studio, or just Build Tools?

You don't need the full IDE to compile this repo — **Visual Studio Build
Tools** is Microsoft's separate, lighter installer: MSBuild plus the
compiler/linker toolsets and individual components, with no code editor,
Solution Explorer, or debugger UI. Since compiling this repo is just
running `msbuild Geomec.sln /p:Configuration=Release /p:Platform=x64`
(§4.4 of `BUILD_AND_ARCHITECTURE.md`), Build Tools covers everything
needed to actually build it.

Regardless of which installer is used, the same individual components
must be selected:

- The **MFC/ATL** component
  (`Microsoft.VisualStudio.Component.VC.ATLMFC`) — required, since
  `platform/MfcGuiFrame`, `MfcModelFrame`, `WindowsRegistry` are
  MFC-based.
- Either the **v140 toolset** specifically (to match this repo's
  `.vcxproj` files as-is), or accept retargeting to whatever current
  toolset ships with Build Tools. Verified via search: Microsoft
  maintains binary compatibility across v140 through the current v145
  (Visual Studio 2026), and older toolsets like v140 remain installable
  side-by-side via the installer if matching the repo as-is is
  preferred over retargeting — consistent with §6 above.

Trade-offs of the Build Tools route:

- **Editing/browsing code**: use anything (VS Code, etc.) — the IDE was
  never required for compiling, just convenient.
- **Debugging**: noticeably rougher without the full IDE — no
  integrated debugger UI, so expect to lean on WinDbg or VS Code's C++
  debugger extension instead of a one-click "Start Debugging."

This only lightens the *compiler* piece. Diana, Open Inventor, Qwt, and
WiX are entirely separate installs with their own weight, unaffected by
which VS SKU is chosen.

## 8. Recommended path

Given the goal is visualization only:

1. Request the Open Inventor trial now (longest lead time, sales-mediated).
2. In parallel, ask Diana support whether a build-only/SDK evaluation
   (no solve rights) is possible, since no model will actually be run.
3. Decide Qt version based on whatever Open Inventor version you're
   actually granted (their docs will state a required/tested Qt version).
4. Retarget this repo's `.vcxproj`/`vsprops` toolset to match whatever
   Visual Studio version the granted Open Inventor SDK actually supports
   — which may mean landing on VS2017 or VS2019 rather than VS2022, if
   that's what the trial SDK requires. Confirm before assuming "current
   VS" is compatible.
5. Expect to port `geomec/OpenInventor` from the 9.3.1-era API to whatever
   current API you receive, if you go the "current trial" route rather
   than sourcing an archived 9.3.1-compatible license.

## 9. Current environment readiness check

A direct answer to "can I compile everything right now?" for the actual
machine this repo has been worked on in this session (WSL2 Linux) —
**no, and it's not just licenses.**

**The Windows build path is categorically impossible from here, full
stop, regardless of licenses.** This is a Linux/WSL sandbox — there is no
Visual Studio, no MSBuild, no Windows SDK, not even a Windows kernel.
That's not a missing-package problem, it's the wrong operating system.
**This matters directly for the stated visualization goal: `geomec/OpenInventor`
has no qmake `.pro` project at all** — confirmed by checking `geomec.pro`
(zero OpenInventor references) and the module's own contents (only wired
into `geomec/Geomec.vcxproj`, via `$(OIVHOME)`). Open Inventor
visualization in this codebase is Windows-only; there is no Linux path to
it. **Achieving the visualization goal requires an actual Windows machine
with Visual Studio — not achievable in this environment no matter what
else is installed.**

**The Linux (qmake) build path is also not ready — beyond licensing:**

| Dependency | Status on this machine | Note |
| --- | --- | --- |
| g++ | ✅ present (15.2.0) | Far newer than anything this repo was written against |
| qmake | ✅ present (3.1, Qt 5.15.18) | Matches the Windows side's Qt5 usage — a good sign, but unverified against the actual `.pro` files |
| TBB, HDF5, zlib, Blosc, gtest | ✅ bundled in `third_party/` | See the ABI warning below |
| Python 3 | ✅ present | |
| Diana | ❌ **not installed at all** — not a license problem, the software itself isn't here | Blocks compilation, not just execution, since `DianaCore`/`DianaGlue` link against it |
| Boost | ❌ not installed | `apt install libboost-*-dev` would get this, but hasn't been done |
| GLib dev headers | ❌ not installed | Same — an easy `apt install libglib2.0-dev`, just not done |
| `uuid-dev` | ❌ not installed | Referenced via `$GM_SW_TOOL_PATH/extra/include/uuid` in the docs |
| Open Inventor | ❌ not applicable here | Not part of the Linux graph at all (see above) |

**A real, verified ABI risk, independent of anything above:** the bundled
`third_party/tbb/linux` binaries are prebuilt for `gcc4.1`/`gcc4.4`/`gcc4.7`
(visible directly in their path names) — compilers from roughly 2011–2013.
This machine's g++ is version 15 (2025). That's a huge generation gap
across a known C++ ABI break (libstdc++'s dual ABI, introduced at GCC 5).
Even after installing Boost/GLib/uuid-dev and somehow obtaining Diana,
linking fresh GCC-15-compiled objects against these ancient prebuilt TBB
binaries is a real risk of failure — not guaranteed to fail, but not a
safe assumption either. This would need to be tested directly, or the
prebuilt TBB replaced with a version built against the current GCC.

`qmake/basepath.pri` also hardcodes a site-specific path
(`/glb/data/...`) for GLib that would need replacing with whatever path is
valid on this machine, as already noted in `BUILD_AND_ARCHITECTURE.md`
§5.3.

**Bottom line:** licensing is real, but it's not the only gap. Right now,
neither build path compiles in this environment — Windows can't be
attempted at all here, and Linux is missing Diana entirely (not just its
license), missing Boost/GLib/uuid-dev, and carries an unverified TBB/GCC
ABI risk on top of that.

## 10. Open risks / unknowns

- Whether Open Inventor's trial can be scoped to an *older* SDK version
  (matching this repo's existing 9.3.1 integration code) instead of the
  current release — unconfirmed, ask sales directly.
- Exact Diana compiler/toolset compatibility — not publicly documented.
- Whether Diana truly can't be excluded from the link step at all (a
  from-scratch audit of `DianaCore`/`DianaGlue`/`DianaSignals` call sites
  would be needed to know if a stub/mock replacement is feasible instead
  of the real SDK — not attempted here).

## 11. Sources

- [Open Inventor Software trial request — Thermo Fisher Scientific](https://www.thermofisher.com/us/en/reference-components/MSD-reference-components/instruments-reference-components/open-inventor-software-trial.html)
- [Licensing – Activate Open Inventor Licenses – Open Inventor 9 Developer Zone](https://developer9.openinventor.com/index.php/general-documentation/licensing-activate-open-inventor-licenses/)
- [Open Inventor: Platforms of Open Inventor 2023.2](https://developer.openinventor.com/refmans/latest/RefManCpp/_inventor_platforms_2023_2.html)
- [System Requirements – Open Inventor 9.9 – Open Inventor 9 Developer Zone](https://developer9.openinventor.com/index.php/system-requirements-open-inventor-9-9/)
- [SoQt Class Reference [Qt]](https://www.openinventor.com/reference-manuals/NewRefMan1030/RefManCpp/class_so_qt.html)
- [Licenses & Modules - DIANA FEA](https://dianafea.com/licenses-modules/)
- [DIANA FEA](https://en.wikipedia.org/wiki/DIANA_FEA)
- [What's new for MSVC Build Tools — Microsoft Learn](https://learn.microsoft.com/en-us/cpp/overview/what-s-new-for-msvc?view=msvc-170)
- [Install the Microsoft C++ (MSVC) Build Tools — Microsoft Learn](https://learn.microsoft.com/en-us/cpp/overview/acquire-msvc?view=msvc-170)
- [C++ Tools and Features in Visual Studio Editions — Microsoft Learn](https://learn.microsoft.com/en-us/cpp/overview/visual-cpp-tools-and-features-in-visual-studio-editions?view=msvc-170)
