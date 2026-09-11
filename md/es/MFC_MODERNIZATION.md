# Dejar Atrás MFC — Notas de Viabilidad

Notas de referencia sobre qué tan complejo sería sacar la GUI de Windows
de MFC (Microsoft Foundation Classes) hacia algo moderno, y cuál sería
realmente ese destino. Esto es una opinión sobre viabilidad, no un plan —
no se ha cambiado nada.

## Tabla de contenidos

1. [¿Qué tan profundo llega MFC realmente?](#1-que-tan-profundo-llega-mfc-realmente)
2. [El destino no es una elección abierta — ya es Qt](#2-el-destino-no-es-una-eleccion-abierta--ya-es-qt)
3. [Cómo sería el trabajo real](#3-como-seria-el-trabajo-real)
4. [Escala](#4-escala)

## 1. ¿Qué tan profundo llega MFC realmente?

Comprobado, no asumido. Esto no es una dependencia periférica:

- **El shell de la aplicación en sí es MFC.** `geomec/src/Geomec.cpp` es
  un `CWinApp`, `geomec/src/MainFrm.cpp` es la ventana de marco
  principal — la raíz de toda la aplicación, no un rincón heredado de
  ella.
- **186** bloques `BEGIN_MESSAGE_MAP` en todo el código propio.
- **98** clases derivadas de `CDialog`/`CWnd`.
- **36** archivos incluyen directamente una cabecera de MFC (`afxwin.h`,
  etc.).
- **51 archivos dentro de `geomec/OpenInventor` usan MFC** — el viewport
  de visualización 3D está alojado en una ventana MFC, algo no separable
  de la cuestión de MFC. `geomec/OpenInventor` no usa `SoQt`/`SoWin` hoy
  (comprobado, cero coincidencias).

## 2. El destino no es una elección abierta — ya es Qt

`platform/qtwinmigrate` es una biblioteca real y conocida de Qt Solutions
cuyo *propósito completo* es conectar MFC y Qt para una migración
incremental (`QMfcApp` ejecuta un bucle de eventos de Qt desde dentro de
una app MFC; `QWinWidget` aloja widgets de Qt como hijos de ventanas MFC).
Su presencia aquí significa que alguien ya empezó exactamente esta
migración una vez, incrustando widgets de Qt dentro del shell MFC en
lugar de construir una app Qt separada en paralelo.

Consistente con eso: **7 archivos ya usan `QSettings`** (la API de
configuración multiplataforma de Qt) junto a **8 que todavía usan el
`platform/WindowsRegistry`** ligado a MFC — un cambio pequeño, real, y ya
en marcha hacia el equivalente nativo de Qt.

Dado que Qt ya se usa en todo el resto de la app (`Qt5Core`, `Qt5Widgets`,
`Qt5Xml`, Qwt), **Qt es el destino natural** — no Win32 nativo, no
WinUI3, no wxWidgets. Elegir cualquier otra cosa significaría mantener
los modismos de *dos* toolkits de GUI en lugar de terminar el que ya está
medio adoptado.

## 3. Cómo sería el trabajo real

En orden de dependencia (los pasos posteriores dependen de que los
anteriores estén hechos):

1. Migrar cada una de las 98 clases derivadas de `CDialog`/`CWnd` a
   `QDialog`/`QWidget`: plantillas de diálogo `.rc` → archivos `.ui` de
   Qt Designer (o código), manejadores `BEGIN_MESSAGE_MAP` → conexiones
   signal/slot de Qt, `CString` → `QString`, `CArray`/`CList` →
   `QVector`/`QList`, GDI `OnPaint`/`CDC` → `QPainter`, serialización
   `CArchive` → un equivalente Qt/personalizado, `CWinThread` →
   `QThread`.
2. Realojar el viewport de Open Inventor usando `SoQt` (el propio
   componente de integración con Qt de Open Inventor, confirmado que
   existe — ver `GETTING_STARTED_MODERN_TOOLCHAIN.md` §2) en lugar de la
   ventana alojada en MFC actual, en los 51 archivos afectados dentro de
   `geomec/OpenInventor`.
3. Terminar de retirar `platform/WindowsRegistry` en favor de
   `QSettings` (ya 7 de 15 puntos de uso convertidos).
4. Convertir el shell de la aplicación al final, una vez que nada debajo
   de él siga necesitando MFC: `CWinApp`/`CMainFrame`
   (`geomec/src/Geomec.cpp`, `geomec/src/MainFrm.cpp`) →
   `QApplication`/`QMainWindow`.
5. Retirar `platform/qtwinmigrate`, `platform/MfcGuiFrame`,
   `platform/MfcModelFrame` una vez que nada los referencie — un efecto
   secundario natural de devolver tres de los quince módulos exclusivos
   de `platform/` (solo Windows) a un estado multiplataforma, lo cual
   también ayudaría al esfuerzo de build en Linux discutido por separado.

## 4. Escala

No es algo scriptable como la reestructuración de carpetas hecha antes en
este proyecto — aquella fue mecánica (mover un directorio, corregir una
cadena de ruta, verificar con grep). La conversión de MFC a Qt en
diálogos se hace a mano, diálogo por diálogo: los layouts, los IDs de
recursos y el comportamiento específico de cada control difieren lo
suficiente como para que los pocos conversores automáticos de MFC a Qt
que existen todavía necesiten una limpieza manual sustancial por diálogo.
Realista y en la práctica, trabajo de varios meses a varios años para un
código con 98 clases de diálogo y una dependencia a nivel de shell — no
algo para empezar de forma casual, ni algo para intentar antes de que la
app siquiera compile (ver `GETTING_STARTED_MODERN_TOOLCHAIN.md` §8 para
la brecha actual de preparación para compilar).
