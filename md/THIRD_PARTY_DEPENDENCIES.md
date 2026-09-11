# Third-Party and Microsoft Dependencies

Every external library/SDK this project uses: what it's for, whether it
requires a license, how to get or build it, and where the build expects it
to live. See `BUILD_AND_ARCHITECTURE.md` for the full environment-variable
list and `REPO_LAYOUT.md` for the folder-ownership overview.

Two categories:
- **Bundled** — already checked into `third_party/` in this repo.
- **Not bundled** — the build expects these to exist on the machine
  (via SDK install + environment variable), not checked in.

## Table of contents

1. [Bundled in `third_party/`](#1-bundled-in-third_party)
2. [Required but NOT bundled](#2-required-but-not-bundled)
3. [Microsoft libraries and tools](#3-microsoft-libraries-and-tools)
4. [Which of these could modern C++ replace?](#4-which-of-these-could-modern-c-replace)
5. [Why each dependency was used instead of the standard library](#5-why-each-dependency-was-used-instead-of-the-standard-library)
6. [Notes](#6-notes)
7. [Sources](#7-sources)

## 1. Bundled in `third_party/`

| Library | Used for | License | Download / build | Where it lives |
| --- | --- | --- | --- | --- |
| TBB (Threading Building Blocks) | Parallel/runtime support for the Linux (qmake) build | Free, open source (Apache 2.0) | Prebuilt binaries + headers already included; upstream at [github.com/oneapi-src/oneTBB](https://github.com/oneapi-src/oneTBB) if a rebuild is ever needed | `third_party/tbb/{linux,windows}` |
| HDF5 | Data/model storage | Free, permissive BSD-style HDF5 license | Prebuilt; upstream at [hdfgroup.org](https://www.hdfgroup.org/) (v1.8.17 here) | `third_party/hdf5/{linux,windows,linux_1.8.17,windows_1.8.17}` |
| zlib 1.2.6 | Generic compression, used by Blosc/HDF5 | Free, permissive zlib license | Source bundled; compiled as part of the qmake/MSBuild graph; upstream at [zlib.net](https://zlib.net/) | `third_party/zlib-1.2.6` |
| Blosc (c-blosc 1.9.0) | Fast block compression for mesh/result data | Free, BSD (bundles LZ4/Snappy/etc., each permissive — see `LICENSES/`) | Source bundled; built via `blosc_shared.vcxproj` / qmake; upstream at [github.com/Blosc/c-blosc](https://github.com/Blosc/c-blosc) | `third_party/c-blosc-1.9.0` |
| Google Test / Google Mock 1.7.0 | C++ unit testing framework | Free, BSD 3-clause | Source bundled; upstream at [github.com/google/googletest](https://github.com/google/googletest) (a much newer version exists upstream if ever upgraded) | `third_party/gmock-1.7.0` (includes `gtest`) |
| Parallel STL (pstl) | Parallel algorithm backend (Intel) | Free, Apache 2.0 | Prebuilt; upstream at [github.com/oneapi-src/oneDPL](https://github.com/oneapi-src/oneDPL) (successor project) | `third_party/pstl/{linux,windows}` |
| CORA (Deltares CORA1) | Probabilistic/reliability analysis tool integrated via `geomec_cora_wrapper`/`geomec_cora_library` | **Proprietary — verify licensing terms with Deltares** before further distribution | Vendor-supplied binaries/installer; not something to rebuild | `third_party/CORA1` |
| SVS (`SVS64.exe`) | Unidentified Qt-based executable/utility | **Unknown — could not identify vendor/license from the binary; check procurement records** | Vendor-supplied binary | `third_party/SVS` |
| .NET Framework 4.0 redistributable | Runtime prerequisite for `CORA1` (a .NET/Excel-interop application) | Free to redistribute for installation purposes (Microsoft) | Official redistributable installer from Microsoft | `third_party/dotNet` |
| Point-element-set bitmaps | UI icon/bitmap assets for the point-elements-sets feature | In-house assets (not vendored code) made with the free "Greenfish Icon Editor Pro" tool | N/A — static assets | `third_party/point_elementsets_bitmaps` |

## 2. Required but NOT bundled

These aren't in the repo — they must be installed on the build machine and
pointed to via the listed environment variable(s).

| Dependency | Used for | License | Download / build | Where the build expects it |
| --- | --- | --- | --- | --- |
| Diana | The core FEA/geomechanics solver Geomec integrates with — the app's central external dependency | **Commercial — requires a Diana vendor license** | Obtained directly from the Diana distributor, not compiled here | `DIAROOT` (Windows: contains `release`/`debug`/`share`); `$GM_SW_TOOL_PATH/diana` (Linux) |
| Qt | GUI framework, XML/OpenGL support | Free (LGPL, if dynamically linked and terms followed) or paid Commercial license | Qt Online Installer from [qt.io](https://www.qt.io/download); note `BUILD_AND_ARCHITECTURE.md` flags a version mismatch — `Geomec.vcxproj` links Qt5, but `geomec/INSTALL.txt` describes Qt 4.8.5 — confirm which your target project actually needs | `QTDIR` (Windows); `$GM_SW_TOOL_PATH/qt-4.8.5` (Linux) |
| Qwt | Plotting/widgets (Windows only) | Free (LGPL 2.1 with exception) | Build from source or download prebuilt, matching your Qt version, from [qwt.sourceforge.io](https://qwt.sourceforge.io/) | `QWTDIR` |
| Open Inventor (OIV) | 3D scene-graph visualization | **Commercial — requires a purchased license + license file** | Purchased SDK (v9.3.1 per docs), historically from VSG/FEI | `OIVHOME`, license file via `VSG_LICENSE_FILE` |
| Intel runtime libraries | C++/Fortran runtime support | Runtime redistribution is free; compiling *with* the Intel compiler requires a paid Intel license | Intel oneAPI HPC Toolkit / redistributable libraries from [intel.com](https://www.intel.com/) | `INTEL` |
| Boost | General-purpose C++ libraries (e.g. `boost_date_time`) | Free, permissive Boost Software License | Download source / prebuilt from [boost.org](https://www.boost.org/); build only the needed compiled libs | `$(BOOST)` (Linux qmake graph) |
| GLib | Low-level utility library (Linux build only) | Free, LGPL | Install via distro package manager (e.g. `libglib2.0-dev`) — the repo currently hardcodes a site-specific path in `qmake/basepath.pri` that should be replaced with a local path | Referenced via `GLIB2PATH` in `qmake/basepath.pri` (currently a hardcoded internal path — needs updating for a portable build) |
| Python | Scripting/integration for selected tools and plugins (`py/`) | Free, PSF license | Standard Python install | Referenced via `PYTHONROOT` in `qmake/basepath.pri` (currently a hardcoded internal path) |
| FlexLM | Application licensing/DRM for Geomec itself | **Requires a Flexera vendor agreement** to embed/ship | Supplied by Flexera as part of a licensing integration agreement | `FLEXLM` (derived from `DIAROOT`/`GMROOT`) |
| Hasp (Sentinel HASP) | Hardware-dongle-based licensing, alternative/complement to FlexLM | **Requires a Thales/Sentinel vendor agreement** | Supplied by Thales as part of a licensing integration agreement | Driver + license installed per `geomec/INSTALL.txt` |

## 3. Microsoft libraries and tools

| Item | Used for | License | Download / build | Where the build expects it |
| --- | --- | --- | --- | --- |
| MFC (Microsoft Foundation Classes) | Legacy Windows GUI framework used by `platform/MfcGuiFrame`, `platform/MfcModelFrame`, `platform/WindowsRegistry`, and wrapped by `platform/qtwinmigrate` | Free — included with Visual Studio (any edition, including free Community) as long as the "MFC and ATL" optional component is installed | Installed via the Visual Studio Installer, not downloaded separately | Comes from the VS/Windows SDK install; no repo path |
| Windows SDK | Win32 headers (`windows.h`, etc.) needed by the MFC/registry code and native Windows integrations | Free (Microsoft) | Bundled with Visual Studio, or standalone from Microsoft | Comes from the VS install; no repo path |
| .NET Framework 4.0 | Runtime for `third_party/CORA1` (see above) | Free to redistribute | Bundled installer already in the repo | `third_party/dotNet` |
| WiX Toolset | Builds the Windows `.msi`/bundle installers under `wix/` | Free, open source (originally Microsoft-originated, now community/.NET Foundation project) | v3.7 per docs — download from [wixtoolset.org](https://wixtoolset.org/) or its GitHub releases | `WixToolPath` env var points at its `bin/` directory |
| Visual C++ / MSVC toolset (v140) | Compiler/linker for the Windows build | Free (Community edition) or paid (Professional/Enterprise) | Visual Studio 2012/2015+ Installer, with the v140 toolset component | Selected as the platform toolset in `Geomec.vcxproj` |

## 4. Which of these could modern C++ replace?

Short answer: two are genuinely replaceable by standard C++ features
(one of them almost fully already superseded), one is *partially*
replaceable with an important caveat, and everything else solves a
problem the C++ standard library doesn't address at all, regardless of
version.

**Clarifying which C++ version actually did what, since it's easy to
conflate "C++20" with "modern C++" generally:** TBB's overlap
(`parallel_for`/`parallel_reduce`-style loops) is with **C++17**'s
`<execution>` parallel algorithms, not C++20 — C++20 itself added no new
parallel algorithms. What C++20 *did* add on the concurrency side is
lower-level: `<chrono>` calendar/time-zone support (the piece relevant to
replacing `boost_date_time`, row below), plus `std::jthread`,
`std::latch`, `std::barrier`, and `std::counting_semaphore` — useful
building blocks, but none of them replace TBB's actual value (a
work-stealing scheduler, concurrent containers, the flow graph). So: the
one piece of TBB with a standard replacement is a C++17 feature; nothing
in C++20/23 changes that picture for TBB specifically.

| Dependency | Replaceable by modern C++? | With what | Caveat |
| --- | --- | --- | --- |
| Parallel STL (`third_party/pstl`) | **Yes, largely already obsolete** | C++17 `<execution>` parallel algorithms (`std::execution::par`, `std::for_each`/`std::transform`/`std::reduce` with an execution policy) | `pstl` was Intel's own early implementation of what C++17 later standardized into the language itself; current compilers ship this natively, so a dedicated `pstl` dependency is largely redundant on a modern toolchain |
| Boost (`boost_date_time`) | **Yes, on a current-enough compiler** | C++20 `<chrono>` calendar and time-zone support (`std::chrono::year_month_day`, `std::chrono::zoned_time`, IANA tz database) | Needs a genuinely current compiler: GCC only fully supports this since GCC 14.2 (this machine's GCC 15 qualifies), MSVC since VS2019 16.10. Older compilers can't do this — check before relying on it |
| TBB | **Partially, and with a real catch on Linux** | C++17 `<execution>` parallel algorithms cover TBB's simplest use case (`parallel_for`/`parallel_reduce`-style loops over a range) | On GCC/libstdc++, the C++17 parallel algorithms are themselves implemented *using* TBB — merely `#include <execution>` creates a link-time dependency on TBB. So on Linux this doesn't remove TBB, it just makes the dependency indirect. On MSVC, the STL's parallel algorithms have their own backend and genuinely don't need TBB. Either way, TBB's other features — concurrent containers (`concurrent_hash_map`, `concurrent_queue`), the flow graph, `task_arena`/`task_group`, the scalable allocator (`tbbmalloc`) — have **no standard C++ equivalent at any version**, including C++20/23 |
| HDF5, zlib, Blosc (c-blosc) | No | — | File formats and compression are outside the C++ standard's scope entirely; there is no `std::compress` or standard scientific-data-file API |
| Google Test/Mock | No | — | C++ still has no standard built-in testing framework; a replacement would be another third-party library (Catch2, doctest), not a language feature |
| Qt, Qwt, MFC | No | — | C++ has no standard GUI toolkit at any version |
| Open Inventor | No | — | No standard 3D scene-graph/rendering API exists in C++ (the graphics-related standardization proposals were abandoned) |
| Diana | No | — | A whole external FEA solver product; not a language-level concern at all |
| FlexLM, Hasp | No | — | Licensing/DRM infrastructure, unrelated to language features |
| CORA1, SVS, dotNet, WiX | No | — | External applications/tools and an installer toolchain; not language-related |
| Intel runtime libraries | Indirectly | — | Only relevant if compiling with the Intel compiler; switching to MSVC/GCC/Clang removes this as a toolchain choice, not a language-feature substitution |

## 5. Why each dependency was used instead of the standard library

Where a genuine "vs. the standard library" question applies, with real
published numbers rather than vague claims. Several rows have no
meaningful speed comparison at all, because no standard alternative
exists (or existed) to compare against — that's noted explicitly rather
than skipped. The last column calls out, specifically, when the better
option today is the C++ standard itself or another free/open-source
project, versus cases with no realistic free alternative at all.

| Dependency | Standard-library-only alternative | Why the dependency was chosen instead | Quantitative data | Better/current option available today |
| --- | --- | --- | --- | --- |
| HDF5 | Plain `fstream`/raw binary read-write with hand-rolled indexing | Self-describing hierarchical format (data + metadata together), partial/random access into a huge array without loading the whole file, built-in chunking and compression filters, portable across platforms and languages — none of which raw binary I/O provides at any speed | For large, contiguous, well-tuned writes, HDF5 reaches **~70–95% of raw filesystem throughput** (5–30% overhead). Poorly-tuned access (small/misaligned writes, bad chunk sizing) can be **2–3× slower** than a flat raw-array format in the same test. The overhead buys structure and portability that raw I/O can't provide at any speed | Current **HDF5 1.14.x** (free, same license) is a straightforward version upgrade from the bundled 1.8.17. **Zarr** (free, BSD) is a newer alternative better suited to parallel/cloud-native chunked access, if ever redesigning the storage layer from scratch |
| TBB | `std::thread`; C++17 `<execution>` on MSVC only (see §4) | A work-stealing scheduler that load-balances parallel loops automatically, higher-level patterns (`parallel_for`, pipelines, flow graphs) `std::thread` doesn't provide, and a scalable allocator (`tbbmalloc`) that avoids heap-lock contention in multithreaded, allocation-heavy code | Intel's own figures: relinking with `tbbmalloc` alone typically gives **20–30% faster parallel sections**, up to **~4×** in extreme cases; one measured case went from 94ms to 50ms (**~1.9×**) on a quad-core; a 60-core Xeon Phi case saw one phase run **up to 56× faster** | **oneTBB** (Intel's current free, Apache-2.0-licensed rebrand) is the direct, actively-maintained successor to this old bundle. On GCC/Clang, **oneDPL** is the free successor to the bundled `pstl` and covers similar ground |
| zlib / Blosc (`c-blosc`) | No standard compression facility exists at any C++ version | Shrinks on-disk/in-memory size for large mesh/result datasets; Blosc specifically adds a byte-shuffling pre-filter tuned for typed numeric arrays (exactly the kind Geomec stores) plus multithreading | Blosc's own benchmarks: compression up to **~8 GB/s** (faster than a plain `memcpy` at ~7.5 GB/s in the same test); decompression up to **~35–50 GB/s** with PGO, with single-threaded decompression faster than `memcpy()` at every compression level; Blosc's Zstd codec reaches **>10 GB/s** decompression, over **2×** zlib's own ~4 GB/s peak | **zlib-ng** (free, SIMD-accelerated, drop-in zlib replacement) and **Zstd** (free, better ratio *and* speed than zlib, already usable as a Blosc codec) both beat the bundled zlib 1.2.6 (2012). **Blosc2**/`c-blosc2` (free) is the current, actively-maintained successor to the bundled 1.9.0 |
| Google Test/Mock | Hand-rolled `assert()`/`if`-based checks | Standardized assertions with readable failure output, test discovery/registration, and generated mock objects | Not applicable — a build-time/dev-time dependency with zero runtime footprint in the shipped application | Current **GoogleTest** (gtest+gmock merged into one free repo) is a straightforward upgrade from the bundled 1.7.0 (2013). **Catch2** (free, BSD-2) is a popular modern alternative with a more ergonomic, macro-light assertion style |
| Boost (`boost_date_time`) | None, pre-C++20 | No standard C++ library had timezone-aware date/calendar arithmetic before C++20 `<chrono>` — Boost was the only option when this code was written | Not a speed question — an existence question. See §4 for the current C++20 `<chrono>` replacement option | **C++20 `<chrono>`** — free, because it's the standard itself, once on a current-enough compiler (see §4 for the version cutoffs) |
| pstl | C++17 `<execution>`, now built into the compiler (see §4) | At the time it was adopted, C++17 parallel algorithms didn't exist in any compiler yet; `pstl` was Intel's own early reference implementation of the same idea | Same performance profile as native C++17 `<execution>` today, since that's literally what it became | Superseded — see §4. On GCC/Clang, **oneDPL** is the free, actively-maintained successor |
| Qt, Qwt | None | No C++ standard GUI toolkit or scientific-plotting-widget library exists at any version | Not a speed comparison — a "does not exist" comparison | Both already free (LGPL). **Qt 6** is the actively-developed current line — Qt 5.15's standard support ended 26 May 2025; open-source users get no further official patches (only a community patch collection maintained by KDE). Current Qwt tracks whichever Qt line is in use |
| Open Inventor | None | No standard 3D scene-graph/retained-mode rendering API exists in C++ | Not a speed comparison — a "does not exist" comparison | No free equivalent with the same full feature set, but **VTK** (free, BSD, widely used in exactly this scientific/engineering visualization space) is the most credible free alternative if ever reconsidering the whole visualization stack — not a drop-in swap, a rewrite |
| Diana | None | A complete external FEA solver product — not something a library or language feature substitutes for | N/A | No realistic free swap without effectively becoming a different product. Free/open FEA solvers exist generally (Code_Aster, CalculiX) but don't match Diana's specific geomechanics feature set |
| FlexLM, Hasp | None | Licensing/DRM infrastructure, unrelated to the standard library at any version | N/A | Free/open licensing options exist (e.g. `licensecc`), but enterprises typically keep commercial ones for support/compliance reasons — not really a technical upgrade question |
| MFC | None (until Qt was adopted alongside it) | The native Windows GUI framework at the time this code was originally written; no cross-platform or standard C++ GUI toolkit existed to use instead | N/A — see `MFC_MODERNIZATION.md` for the modern (Qt) alternative available now | **Qt** (free, LGPL, already used elsewhere in this app) — see `MFC_MODERNIZATION.md` for the full migration path |
| WiX | None | Installer tooling, not a language-level concern | N/A | Already free; current **WiX v7** (annual release cadence, April 2026) is the actively-maintained line vs. the v3.7 this repo's docs reference — several major versions behind |
| CORA1 | None | Deltares's own probabilistic/reliability analysis tool; a specific engineering product, not a library | N/A | No drop-in free equivalent. **OpenTURNS** (free, LGPL) covers general uncertainty-quantification but isn't a domain match for CORA's specific workflow |
| SVS | None | Unidentified vendor/purpose (see §1) | N/A | Can't recommend an alternative without knowing what it actually does — resolve the "what is SVS" question first |
| dotNet Framework 4.0 | None | Runtime prerequisite for `CORA1`, a .NET/Excel-interop application | N/A | Significantly outdated (2010-era) — Microsoft has already retired support for .NET Framework versions earlier than 4.5.2, and .NET Framework as a whole is "effectively in maintenance mode" per Microsoft's own lifecycle docs. Current **.NET** (the free, cross-platform, actively-developed successor, formerly ".NET Core") is Microsoft's own recommended target for anything new |
| Intel runtime libraries | None | Only needed when code is compiled with the Intel compiler — a toolchain requirement, not a library choice | N/A | Current **Intel oneAPI** runtime is the modern equivalent if staying on the Intel compiler; switching to MSVC/GCC/Clang removes this dependency entirely (all free) |

## 6. Notes

- "Requires a license" above means a *commercial/legal* license
  (something to purchase or contract for), distinct from open-source
  licenses (BSD/Apache/LGPL/zlib/etc.) which are free but still carry
  attribution/compliance obligations — check each package's actual
  license file before redistributing.
- `CORA1` and `SVS` are flagged as needing verification because nothing
  in the repository (no license file, no clear vendor string in the
  binary for `SVS`) confirms their exact terms — don't assume they're
  freely redistributable without checking with whoever originally sourced
  them.
- The Linux build's hardcoded paths for GLib and Python
  (`qmake/basepath.pri`) point at a specific internal machine layout
  (`/glb/data/...`); a portable Linux build needs those replaced with
  paths valid on the local machine, as already called out in
  `BUILD_AND_ARCHITECTURE.md` section 5.3.

## 7. Sources

Facts in §4 about compiler support and the GCC/TBB relationship, and §5's
HDF5/Blosc/TBB performance figures, current as of September 2026:

- [GCC 9 libstdc++ Lands The C++17 Parallel Algorithms Implementation From Intel](https://www.phoronix.com/forums/forum/software/programming-compilers/1088024-gcc-9-libstdc-lands-the-c-17-parallel-algorithms-implementation-from-intel)
- [C++17 STL Parallel Algorithms - with GCC 9.1 and Intel TBB on Linux and macOS — isocpp.org](https://isocpp.org/blog/2019/05/cpp17-stl-parallel-algorithms-with-gcc-9.1-and-intel-tbb-on-linux-and-macos)
- [[gcc r13-4853] libstdc++: Implement C++20 time zone support in \<chrono\>](https://gcc.gnu.org/pipermail/libstdc++-cvs/2022q4/039034.html)
- [Around the World in C++: Exploring Time Zones with std::chrono — C++ Stories](https://www.cppstories.com/2024/zones_around_world_chrono/)
- [zoned_traits struct — Microsoft Learn](https://learn.microsoft.com/hu-hu/cpp/standard-library/zoned-traits-struct?view=msvc-170)
- [Qt 5.15 Standard Support for Legacy License Holders Ends Today — Qt blog](https://www.qt.io/blog/qt-5.15-support-ends)
- [Qt | endoflife.date](https://endoflife.date/qt)
- [Microsoft .NET Framework — Microsoft Lifecycle](https://learn.microsoft.com/en-us/lifecycle/products/microsoft-net-framework)
- [Lifecycle FAQ - .NET Framework — Microsoft Learn](https://learn.microsoft.com/en-us/lifecycle/faq/dotnet-framework)
- [Release notes — WiX Toolset / FireGiant Docs](https://docs.firegiant.com/wix/whatsnew/releasenotes/)
- [GitHub - wixtoolset/wix](https://github.com/wixtoolset/wix)
- [HDF5: Improving I/O Performance When Working with HDF5 Compressed Datasets](https://support.hdfgroup.org/documentation/hdf5/latest/improve_compressed_perf.html)
- [HDF5 Write Speeds: Matching Underlying Raw I/O — VargaLABS](https://steven-varga.ca/blog/hdf5-write-performance-vs-raw/)
- [RawArray: A Simple, Fast, and Extensible Archival Format for Numeric Data (arXiv)](https://arxiv.org/pdf/2112.01273)
- [GitHub - Blosc/c-blosc: faster than `memcpy()`](https://github.com/Blosc/c-blosc)
- [Blosc, an extremely fast, multi-threaded, meta-compressor library](https://blosc.org/pages/)
- [Scalable Memory Allocator — Intel oneTBB documentation](https://www.intel.com/content/www/us/en/docs/onetbb/developer-guide-api-reference/2021-6/scalable-memory-allocator.html)
- [Why Effective Parallel Programming Must Include Scalable Memory Allocation — InfoWorld](https://www.infoworld.com/article/2253457/why-effective-parallel-programming-must-include-scalable-memory-allocation.html)
