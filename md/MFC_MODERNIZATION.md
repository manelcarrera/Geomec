# Moving Off MFC — Feasibility Notes

Reference notes on how complex it would be to move the Windows GUI off
MFC (Microsoft Foundation Classes) to something modern, and what that
target would actually be. This is a feasibility opinion, not a plan —
nothing has been changed.

## Table of contents

1. [How deep does MFC actually go?](#1-how-deep-does-mfc-actually-go)
2. [The target isn't an open choice — it's already Qt](#2-the-target-isnt-an-open-choice--its-already-qt)
3. [What the actual work would look like](#3-what-the-actual-work-would-look-like)
4. [Scale](#4-scale)

## 1. How deep does MFC actually go?

Checked rather than assumed. This is not a peripheral dependency:

- **The application shell itself is MFC.** `geomec/src/Geomec.cpp` is a
  `CWinApp`, `geomec/src/MainFrm.cpp` is the main frame window — the root
  of the whole application, not a legacy corner of it.
- **186** `BEGIN_MESSAGE_MAP` blocks across the first-party codebase.
- **98** classes derived from `CDialog`/`CWnd`.
- **36** files directly include an MFC header (`afxwin.h`, etc.).
- **51 files inside `geomec/OpenInventor` itself use MFC** — the 3D
  visualization viewport is hosted in an MFC window, not something
  separable from the MFC question. `geomec/OpenInventor` has no
  `SoQt`/`SoWin` usage today (checked, zero hits).

## 2. The target isn't an open choice — it's already Qt

`platform/qtwinmigrate` is a real, known Qt Solutions library whose
*entire purpose* is bridging MFC and Qt for incremental migration
(`QMfcApp` runs a Qt event loop from inside an MFC app; `QWinWidget` hosts
Qt widgets as children of MFC windows). Its presence here means someone
already started exactly this migration once, embedding Qt widgets inside
the MFC shell rather than building a separate Qt app alongside it.

Consistent with that: **7 files already use `QSettings`** (Qt's
cross-platform settings API) alongside **8 still using the MFC-adjacent
`platform/WindowsRegistry`** — a small, real, already-in-progress shift
toward the Qt-native equivalent.

Since Qt is already used everywhere else in the app (`Qt5Core`,
`Qt5Widgets`, `Qt5Xml`, Qwt), **Qt is the natural target** — not native
Win32, not WinUI3, not wxWidgets. Picking anything else would mean
maintaining *two* GUI toolkits' worth of idioms instead of finishing the
one already half-adopted.

## 3. What the actual work would look like

In dependency order (later steps depend on earlier ones being done):

1. Port each of the 98 `CDialog`/`CWnd`-derived classes to
   `QDialog`/`QWidget`: `.rc` dialog templates → Qt Designer `.ui` files
   (or code), `BEGIN_MESSAGE_MAP` handlers → Qt signal/slot connections,
   `CString` → `QString`, `CArray`/`CList` → `QVector`/`QList`, GDI
   `OnPaint`/`CDC` → `QPainter`, `CArchive` serialization → a Qt/custom
   equivalent, `CWinThread` → `QThread`.
2. Re-host the Open Inventor viewport using `SoQt` (Open Inventor's own
   Qt integration component, confirmed to exist — see
   `GETTING_STARTED_MODERN_TOOLCHAIN.md` §2) instead of the current
   MFC-hosted window, across the 51 affected files in
   `geomec/OpenInventor`.
3. Finish retiring `platform/WindowsRegistry` in favor of `QSettings`
   (already 7 of 15 call sites converted).
4. Convert the application shell last, once nothing under it still needs
   MFC: `CWinApp`/`CMainFrame` (`geomec/src/Geomec.cpp`,
   `geomec/src/MainFrm.cpp`) → `QApplication`/`QMainWindow`.
5. Retire `platform/qtwinmigrate`, `platform/MfcGuiFrame`,
   `platform/MfcModelFrame` once nothing references them — a natural side
   effect of moving three of the fifteen `platform/`-only (Windows-only)
   modules back to cross-platform status, which would also help the
   Linux-build effort discussed separately.

## 4. Scale

Not scriptable like the folder restructuring done earlier in this
project — that was mechanical (move a directory, fix a path string,
verify by grepping). MFC→Qt dialog conversion is done by hand, dialog by
dialog: layouts, resource IDs, and control-specific behavior differ
enough that the few automated MFC-to-Qt converters that exist still need
substantial manual cleanup per dialog. Realistically multi-month-to-
multi-year work for a codebase with 98 dialog classes and a shell-level
dependency — not something to start casually, and not something to
attempt before the app even compiles (see
`GETTING_STARTED_MODERN_TOOLCHAIN.md` §8 for the current compile-readiness
gap).
