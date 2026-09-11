# Registro de Acciones

Un registro continuo de las acciones realizadas en este repositorio y en
el entorno de desarrollo de esta máquina durante el trabajo con Claude.
Se mantiene actualizado después de cada acción — las entradas más nuevas
van al final de la sección de cada sesión.

## Tabla de contenidos

1. [Sesión 1](#1-sesion-1)
   - 1.1 [Revisión del repositorio](#11-revision-del-repositorio)
   - 1.2 [Reestructuración del repositorio (las 4 fases implementadas)](#12-reestructuracion-del-repositorio-las-4-fases-implementadas)
   - 1.3 [Normalización de indentación](#13-normalizacion-de-indentacion)
   - 1.4 [Commit y push](#14-commit-y-push)
   - 1.5 [Configuración del editor de VS Code (esta máquina, no el repositorio)](#15-configuracion-del-editor-de-vs-code-esta-maquina-no-el-repositorio)
   - 1.6 [IntelliSense de C/C++ (errores de #include)](#16-intellisense-de-cc-errores-de-include)
   - 1.7 [Qt5 instalado para IntelliSense](#17-qt5-instalado-para-intellisense)
   - 1.8 [Este registro](#18-este-registro)
   - 1.9 [Referencia de estructura del repositorio](#19-referencia-de-estructura-del-repositorio)
   - 1.10 [Documentación consolidada en `md/`](#110-documentacion-consolidada-en-md)
   - 1.11 [Referencia de dependencias de terceros](#111-referencia-de-dependencias-de-terceros)
   - 1.12 [Estándar de formato de documentación](#112-estandar-de-formato-de-documentacion)
   - 1.13 [Investigación de toolchain moderno / licencias de prueba](#113-investigacion-de-toolchain-moderno--licencias-de-prueba)
   - 1.14 [Se encontraron artefactos de licencia reales incluidos en el repositorio](#114-se-encontraron-artefactos-de-licencia-reales-incluidos-en-el-repositorio)
   - 1.15 [Comprobación de preparación para compilar en esta máquina](#115-comprobacion-de-preparacion-para-compilar-en-esta-maquina)
   - 1.16 [Notas de viabilidad de migración a CMake](#116-notas-de-viabilidad-de-migracion-a-cmake)
   - 1.17 [Qué dependencias podría reemplazar el C++ moderno](#117-que-dependencias-podria-reemplazar-el-c-moderno)
   - 1.18 [Notas de viabilidad de modernización de MFC](#118-notas-de-viabilidad-de-modernizacion-de-mfc)
   - 1.19 [Por qué cada dependencia vs. la biblioteca estándar, con cifras](#119-por-que-cada-dependencia-vs-la-biblioteca-estandar-con-cifras)
   - 1.20 [Se añadió la columna "mejor opción disponible hoy"](#120-se-anadio-la-columna-mejor-opcion-disponible-hoy)
   - 1.21 [Se aclaró que el reemplazo de TBB es C++17, no C++20](#121-se-aclaro-que-el-reemplazo-de-tbb-es-c17-no-c20)

## 1. Sesión 1

### 1.1 Revisión del repositorio

- Se leyó `BUILD_AND_ARCHITECTURE.md` y se examinó la estructura de
  carpetas existente, los tamaños de archivo, y cómo los grafos de build
  de Windows (`vsprops`/MSBuild) y Linux (`qmake`) resuelven las rutas.
- Se escribió `FOLDER_STRUCTURE_PROPOSAL.md`: evaluación de los problemas
  de la estructura original (binarios vendorizados mezclados en `lib/`,
  datos de prueba mezclados en `Tests/`, una colisión `Tests/` vs `test/`
  que solo difería en mayúsculas, código heredado huérfano) y una
  propuesta de reestructuración, ofrecida en 4 fases.

### 1.2 Reestructuración del repositorio (las 4 fases implementadas)

- **Fase 1** — Se dividió `lib/` en `lib/` propio y un nuevo
  `third_party/` (SDKs vendorizados: `tbb`, `hdf5`, `gmock-1.7.0`,
  `zlib-1.2.6`, `c-blosc-1.9.0`, `pstl`); se fusionó el antiguo
  `3rdParty/` raíz en `third_party/` (`CORA1`, `SVS`, `dotNet`,
  `point_elementsets_bitmaps`).
- **Fase 2** — Se dividió `Tests/` en `Tests/` (solo código fuente) y un
  nuevo `TestData/` (antes `Tests/Models`, ~420MB de datos binarios de
  modelos/entrada); se renombró el `test/` en minúsculas a
  `IntegrationTests/` para eliminar un par de directorios que solo
  diferían en mayúsculas/minúsculas.
- **Fase 3** — Se movió el árbol heredado inerte y sin referencias en el
  build `old/` (la app histórica `DSealect`) a `archive/old/`.
  `Prototypes/` se dejó deliberadamente en su sitio (sigue siendo un
  target `SUBDIRS` de qmake activo con rutas relativas sensibles a la
  profundidad — ver `FOLDER_STRUCTURE_PROPOSAL.md`).
- **Fase 4** — Se extrajo un nuevo `platform/` de `lib/` para los 15
  módulos sin proyecto qmake `.pro` (exclusivos de Windows):
  `CutSurfaceUtil`, `DoubleEditLib`, `FemAppGUI`, `GlobalMessage`,
  `GmDatabase`, `IT`, `MPKernel`, `MfcGuiFrame`, `MfcModelFrame`,
  `OpenInventorDataInterface`, `Plugins`, `PyLib`, `WellSchemeUtils`,
  `WindowsRegistry`, `XMLLib`, `qtwinmigrate`.
- Se actualizó cada referencia en `.pro`/`.pri`, `.vcxproj`/`.props`,
  `.sln`, WiX (`.wixproj`/`.wxs`/`.wxi`), y scripts de shell/Python
  encontrada mediante búsqueda estática para que apuntara a las nuevas
  ubicaciones. Se introdujeron las variables raíz `THIRDPARTYROOT`,
  `TESTDATAROOT`, `PLATFORMROOT` junto al patrón existente
  `LBROOT`/`TESTSROOT`, cada una definida una sola vez en
  `qmake/globals.pri` y `vsprops/Environment.props`.
- Se actualizaron `BUILD_AND_ARCHITECTURE.md` y
  `FOLDER_STRUCTURE_PROPOSAL.md` para documentar la estructura tal como
  se implementó, incluyendo las desviaciones respecto al diagrama
  original (sin wrapper `src/`, `TestData`/`IntegrationTests` como
  directorios de nivel superior, `Prototypes/` dejado en su sitio).
- **No verificado con un build real** — este entorno no tiene ni el
  toolchain de Windows (Visual Studio, Diana, Qt, Open Inventor, WiX) ni
  un entorno Linux/Diana completamente configurado.

### 1.3 Normalización de indentación

- Se reindentaron 3,431 de 4,190 archivos propios `.c`/`.cpp`/`.h`/
  `.hpp`/`.cc` (`lib/`, `platform/`, `geomec/`, `fw51/`, `app/`,
  `geomec_shell/`, `gm_diana_app/`, `geomec_cora_wrapper/`,
  `geomec_selector/`, `Tests/`, `IntegrationTests/`, `Prototypes/`) de
  una mezcla de tabulaciones e indentación de 4 espacios a indentación de
  2 espacios. Solo se tocó el espacio en blanco inicial; `third_party/` y
  `archive/` se dejaron intactos. Los finales de línea (LF/CRLF/mixtos)
  se preservaron exactamente.
- Se añadió `.editorconfig` (`indent_style = space`, `indent_size = 2`,
  `tab_width = 2`) en la raíz del repositorio.
- Se añadieron `editor.tabSize: 2`, `editor.insertSpaces: true`,
  `editor.detectIndentation: false` a `Geomec.code-workspace`.

### 1.4 Commit y push

- Se confirmó (commit) todo lo anterior en un único commit y se subió
  (push) a `origin/main` (`git@github.com:manelcarrera/Geomec.git`):
  `36f1df8` "Restructure repository layout and normalize indentation to
  2 spaces" (10,400 archivos modificados).

### 1.5 Configuración del editor de VS Code (esta máquina, no el repositorio)

- Se comprobó si existía una configuración de `editor.fontSize` — no se
  encontró ninguna (global, de workspace, o en `.vscode/settings.json`);
  VS Code usaba su valor por defecto de 14px.
- Se creó `~/.vscode-server/data/User/settings.json` con
  `editor.fontSize: 12`, luego cambiado a `11`, y después a `10` (valor
  actual) según pedidos posteriores.
- El usuario reportó que el cambio de tamaño de fuente no se aplicaba
  visiblemente. Se confirmó que el propio archivo de configuración era
  correcto y que el proceso de VS Code Server en ejecución leía
  exactamente desde ese `--user-data-dir`. Se sugirieron causas
  probables fuera de la visibilidad de este sistema de archivos: un
  nivel de zoom por ventana (`Ctrl+Scroll`/`Ctrl+=`) que enmascara el
  cambio, o necesitar "Developer: Reload Window" — se recomendó probar
  primero "View: Reset Zoom".

### 1.6 IntelliSense de C/C++ (errores de `#include`)

- Se investigó un error reportado de cpptools ("#include errors
  detected...") en `fw51/FieldValueFile/Base/FvGocadFile.cpp`. No existía
  `c_cpp_properties.json`, así que cpptools no tenía
  `compilerPath`/estándar explícito configurado.
- De paso, se descubrió que `.vscode/settings.json` y `.clang-format`
  ahora existen en el repositorio (no creados por esta sesión) con
  `editor.formatOnSave: true` usando `clang-format` (estilo LLVM,
  `IndentWidth: 2`) para archivos C/C++/Objective-C(++). Se avisó al
  usuario que esto ya había reformateado algunos archivos mucho más allá
  de la reindentación de solo espacios en blanco hecha arriba (p. ej. un
  diff de 503 líneas en `Tests/TestLib/src/TestLib.cpp` contra el commit
  subido) — se dejó tal cual, pendiente de la decisión del usuario sobre
  si conservarlo.
- Se creó `.vscode/c_cpp_properties.json` con `compilerPath:
  /usr/bin/g++`, `cStandard: c17`, `cppStandard: c++14`,
  `intelliSenseMode: linux-gcc-x64`, manteniendo el `includePath`
  existente `${workspaceFolder}/**`. Se señaló que esto arregla
  IntelliSense para archivos multiplataforma pero no para los que
  necesitan cabeceras de Qt/Diana/MFC/Open Inventor/Windows, que no
  están instaladas en este sandbox.

### 1.7 Qt5 instalado para IntelliSense

- El usuario preguntó si era necesario instalar Qt para eliminar las
  advertencias de `#include` relacionadas con Qt
  (`fw51/FieldValueFile/Base/FvTextFile.cpp` y otros: `<QTextStream>`,
  `<qfile.h>`, `<qfileinfo.h>`). Se explicó que es opcional (cosmético,
  no afecta la corrección) y se dieron tres opciones: instalar Qt, dejar
  las advertencias, o suprimir los subrayados de error sin instalar
  nada.
- Se recomendó instalar Qt (`qtbase5-dev`) ya que Qt se usa de forma
  generalizada en todo el código de GUI, frente a una supresión general
  que también ocultaría problemas reales en archivos independientes de
  Qt.
- Se intentó `sudo apt install -y qtbase5-dev` — falló (la autenticación
  interactiva de sudo no está disponible en este entorno de
  herramientas). Se le pidió al usuario que lo ejecutara él mismo.
- El usuario confirmó que estaba hecho. Se verificó que
  `qtbase5-dev`/`qtbase5-dev-tools` estaban instalados (Qt 5.15.18) con
  cabeceras bajo `/usr/include/x86_64-linux-gnu/qt5/`.
- Se actualizó `.vscode/c_cpp_properties.json` para añadir
  `/usr/include/x86_64-linux-gnu/qt5` y
  `/usr/include/x86_64-linux-gnu/qt5/**` a `includePath`.

### 1.8 Este registro

- Se creó este archivo (`ACTIONS_LOG.md`) a pedido del usuario, para
  mantenerlo actualizado después de cada acción realizada de aquí en
  adelante.

### 1.9 Referencia de estructura del repositorio

- Se creó `REPO_LAYOUT.md`: una tabla de cada carpeta de nivel superior,
  para qué sirve, y si es propia ("Nuestro") o vendorizada
  ("Third-party"). Solo `third_party/` está marcado como de terceros;
  `wix/` se anota como nuestro (archivos de proyecto) aunque el toolset
  WiX en sí es una herramienta de terceros invocada en tiempo de build.

### 1.10 Documentación consolidada en `md/`

- Se movió toda la documentación de nivel raíz a una nueva carpeta
  `md/`: `BUILD_AND_ARCHITECTURE.md`, `FOLDER_STRUCTURE_PROPOSAL.md`,
  `ACTIONS_LOG.md` (este archivo), `REPO_LAYOUT.md`. Los README/docs de
  terceros vendorizados dentro de `third_party/` se dejaron donde
  estaban — esto solo cubre nuestros propios documentos de nivel raíz.
  Se actualizó la tabla de carpetas de `REPO_LAYOUT.md` para incluir
  `md/`.

### 1.11 Referencia de dependencias de terceros

- Se creó `THIRD_PARTY_DEPENDENCIES.md`: cada dependencia de terceros
  vendorizada (`third_party/`) y no vendorizada, además de
  bibliotecas/herramientas específicas de Microsoft (MFC, Windows SDK,
  .NET Framework, WiX, toolset de MSVC), cada una con para qué sirve, si
  necesita una licencia comercial, cómo obtenerla/compilarla, y dónde
  espera el build que resida. Se señaló que `CORA1` y `SVS` necesitan
  verificación manual de licencia ya que no se pudo encontrar archivo de
  licencia ni cadena de proveedor clara para ninguno de los dos.

### 1.12 Estándar de formato de documentación

- Se añadió una tabla de contenidos numerada a cada archivo en `md/`
  (`BUILD_AND_ARCHITECTURE.md`, `FOLDER_STRUCTURE_PROPOSAL.md`,
  `ACTIONS_LOG.md`, `REPO_LAYOUT.md`, `THIRD_PARTY_DEPENDENCIES.md`) y
  se numeraron todas sus secciones/subsecciones, según el pedido
  permanente del usuario de que todos los archivos de `md/` lleven una
  tabla de contenidos y secciones numeradas — aplicar este mismo formato
  a cualquier archivo nuevo de `md/` de aquí en adelante.

### 1.13 Investigación de toolchain moderno / licencias de prueba

- Se investigó (mediante búsqueda web) si OpenGL/Open Inventor/Diana
  necesitan licencias comerciales, el proveedor actual de Open Inventor
  (Thermo Fisher Scientific, vía el linaje VSG → FEI) y su integración
  con Qt (`SoQt`), un dato actual de compatibilidad de Open Inventor con
  VS (2023.2 necesita un runtime compatible con VS2017), y los procesos
  de prueba tanto para Open Inventor como para Diana FEA (ambos mediados
  por ventas, no de autoservicio instantáneo).
- Se creó `GETTING_STARTED_MODERN_TOOLCHAIN.md`: aborda si Diana es
  evitable si no se va a ejecutar ningún modelo (probablemente sigue
  siendo necesaria la dependencia de SDK/tiempo de enlazado incluso sin
  una licencia de runtime de resolución), aclara que Open Inventor
  necesita tanto un toolset de VS compatible *como* Qt (no una elección
  entre ambos), una matriz de dependencias de versión entre VS/Qt/Open
  Inventor/Diana/las bibliotecas ya vendorizadas en `third_party/`, y un
  camino recomendado dado el objetivo del usuario (solo visualización,
  sin resolver). Señalado como planificación sin verificar, no un build
  intentado.

### 1.14 Se encontraron artefactos de licencia reales incluidos en el repositorio

- Se buscaron en el repositorio archivos de licencia existentes y se
  encontraron tres archivos de clave de Diana reales (todos vencidos)
  que nombran a "Shell" / "Shell Research bv KSEPL" como cliente
  (`geomec/keyfile/Linux/diafgv.k72` y su hermano fechado,
  `archive/old/DSealect/keyfile/diafgv.k72`), más
  `wix/WixInclude/password.dat` — no es una clave sino un puntero de
  estilo FlexLM a un servidor de licencias por red (`SERVER
  muntse-s-09786.europe.shell.com ANY 3015`) para Open Inventor.
- Se avisó al usuario que estos son artefactos corporativos reales
  (nombre de cliente + nombre de host interno) ya subidos a su GitHub
  personal, y que borrar los archivos del árbol de trabajo no los
  eliminaría del historial de git — para eso haría falta una
  reescritura de historial, no intentada.
- Se documentaron ambos hallazgos en `GETTING_STARTED_MODERN_TOOLCHAIN.md`
  §5 ("Artefactos de licencia existentes encontrados en este
  repositorio"), renumerando las secciones posteriores del archivo
  (6–8) para hacer espacio.

### 1.15 Comprobación de preparación para compilar en esta máquina

- Se comprobó qué está realmente instalado en esta máquina Linux WSL2
  frente a lo que necesita cada camino de build: `g++` 15.2.0, `qmake`
  3.1 (Qt 5.15.18), y Python 3 están presentes; Boost, las cabeceras de
  desarrollo de GLib, y `uuid-dev` no lo están; Diana en sí (no solo su
  licencia) está completamente ausente.
- Se confirmó que `geomec/OpenInventor` no tiene ningún proyecto qmake
  `.pro` (cero referencias en `geomec.pro`) — la visualización con Open
  Inventor es exclusiva de Windows en este código, así que el objetivo
  de visualización planteado no se puede perseguir en Linux sin importar
  qué otra cosa se arregle.
- Se encontró un riesgo de ABI concreto y verificado, independiente del
  licenciamiento: los binarios vendorizados de `third_party/tbb/linux`
  están precompilados para `gcc4.1`/`gcc4.4`/`gcc4.7` (visible en los
  propios nombres de sus rutas), una brecha de generación enorme
  respecto al GCC 15 de esta máquina, que atraviesa la ruptura de ABI
  dual de libstdc++ en GCC 5.
- Se documentó todo esto en `GETTING_STARTED_MODERN_TOOLCHAIN.md` como
  nuevo §8 ("Comprobación de preparación del entorno actual"),
  renumerando "Riesgos abiertos" a §9 y "Fuentes" a §10 (también
  numerado por primera vez, por consistencia con el resto del archivo).

### 1.16 Notas de viabilidad de migración a CMake

- El usuario preguntó si qmake/MSBuild se podían migrar a CMake. Se
  respondió: técnicamente sí (MFC vía `CMAKE_MFC_FLAG`, Qt vía
  `find_package`, módulos `Find<X>.cmake` personalizados necesarios para
  Diana/Open Inventor/Qwt ya que ninguno distribuye configuraciones de
  CMake), pero se recomendó no hacerlo ahora — ninguno de los dos
  caminos de build existentes compila hoy en este entorno (según §1.15),
  así que una migración no tendría una base de referencia funcional
  contra la cual validar. Se recomendó primero lograr que el build de
  Windows compile de verdad, y luego migrar con algo contra qué
  verificar el resultado.
- Se creó `CMAKE.md` con estas notas de viabilidad para referencia
  futura.

### 1.17 Qué dependencias podría reemplazar el C++ moderno

- El usuario preguntó si alguna dependencia de terceros (TBB como
  ejemplo) se podía reemplazar con características de C++20. Se
  investigó el soporte de compiladores actual antes de responder (los
  algoritmos paralelos de C++17 de GCC están implementados usando TBB
  internamente — simplemente incluir `<execution>` crea una dependencia
  de enlazado con TBB en GCC/libstdc++, a diferencia de la STL de MSVC
  que tiene su propio backend; el soporte de zona horaria/calendario de
  `<chrono>` de C++20 alcanzó soporte completo en GCC 14.2 y MSVC VS2019
  16.10+).
- Se añadió el nuevo §4 ("¿Cuáles de estos podría reemplazar el C++
  moderno?") a `THIRD_PARTY_DEPENDENCIES.md`: `pstl` está en gran parte
  obsoleto (superado por el soporte nativo de `<execution>` de C++17),
  `boost_date_time` es reemplazable por `<chrono>` de C++20 en un
  compilador suficientemente actual, TBB es solo parcialmente
  reemplazable (y para nada en GCC/Linux, donde los propios algoritmos
  paralelos estándar dependen de TBB) con sus contenedores
  concurrentes/flow graph/asignador sin equivalente estándar en ninguna
  versión de C++, y todo lo demás (HDF5, zlib/Blosc, gtest, Qt/Qwt/MFC,
  Open Inventor, Diana, FlexLM/Hasp, CORA1/SVS/dotNet/WiX) resuelve
  problemas completamente fuera del alcance del estándar de C++.
  Se renumeró "Notas" a §5 y se añadió un nuevo §6 ("Fuentes") citando
  la investigación de soporte de compiladores.

### 1.18 Notas de viabilidad de modernización de MFC

- El usuario preguntó qué tan complejo sería dejar atrás MFC y cuál
  debería ser el destino. Se cuantificó la profundidad real de MFC
  haciendo grep en el código en vez de adivinar: el shell de la propia
  app es MFC (`CWinApp` en `geomec/src/Geomec.cpp`, `CMainFrame` en
  `MainFrm.cpp`), 186 bloques `BEGIN_MESSAGE_MAP`, 98 clases derivadas de
  `CDialog`/`CWnd`, y 51 archivos dentro de `geomec/OpenInventor` mismo
  usan MFC (el viewport 3D está alojado en MFC, no se encontró uso de
  `SoQt`/`SoWin`).
- Se encontró que `platform/qtwinmigrate` es una biblioteca puente de
  migración MFC↔Qt conocida ya en uso, más 7 archivos que ya usan
  `QSettings` junto a 8 que todavía usan `platform/WindowsRegistry` —
  evidencia de que ya se empezó una migración parcial de MFC a Qt una
  vez. Se concluyó que Qt (no un framework nuevo) es el destino natural,
  ya que se usa en todo el resto de la app.
- Se creó `MFC_MODERNIZATION.md` con los hallazgos de profundidad, los
  pasos de migración en orden de dependencia (diálogos → viewport de OIV
  vía `SoQt` → `WindowsRegistry`→`QSettings` → shell de la app → retirar
  `qtwinmigrate`/`MfcGuiFrame`/`MfcModelFrame`), y una nota de
  escala/esfuerzo (no scriptable, trabajo de varios meses a varios años,
  diálogo por diálogo).

### 1.19 Por qué cada dependencia vs. la biblioteca estándar, con cifras

- El usuario pidió el razonamiento detrás de cada elección de
  dependencia frente a la biblioteca estándar de C++, con datos
  cuantitativos/de velocidad donde fuera posible (HDF5 nombrado como
  ejemplo). Se investigaron cifras de benchmarks publicados en vez de
  estimar: HDF5 alcanza ~70–95% del rendimiento del sistema de archivos
  crudo en escrituras grandes y ajustadas (5–30% de sobrecarga) pero
  puede ser 2–3× más lento que un formato de array crudo cuando está mal
  ajustado; Blosc comprime hasta ~8 GB/s (más rápido que los ~7.5 GB/s
  de memcpy) y descomprime hasta ~35–50 GB/s con PGO, su códec Zstd
  supera el propio pico de ~4 GB/s de zlib por más del doble; el
  asignador escalable de TBB típicamente da 20–30% de secciones
  paralelas más rápidas (hasta ~4× en casos extremos, un caso en Xeon
  Phi hasta 56×).
- Se añadió el nuevo §5 ("Por qué se usó cada dependencia en vez de la
  biblioteca estándar") a `THIRD_PARTY_DEPENDENCIES.md` como una tabla
  que cubre cada dependencia — cuantitativo para HDF5/TBB/Blosc-zlib,
  "no existe como alternativa estándar" para Qt/Qwt/Open
  Inventor/MFC/Diana/herramientas de licenciamiento, y "no aplica" para
  herramientas de solo tiempo de desarrollo como Google Test/Mock. Se
  renumeró Notas a §6 y Fuentes a §7, y se añadieron las nuevas fuentes
  de benchmarks a §7.

### 1.20 Se añadió la columna "mejor opción disponible hoy"

- El usuario pidió extender la tabla de §5 con una columna que señalara
  las mejores opciones libres disponibles actualmente, especialmente las
  de la biblioteca estándar o de código abierto. Se dividió la fila
  combinada "WiX, CORA1, SVS, dotNet" en filas separadas (cada una tiene
  una respuesta distinta) y se añadió la columna para cada dependencia:
  p. ej. oneTBB/oneDPL (los sucesores libres actuales de TBB/pstl),
  zlib-ng/Zstd/Blosc2 (superan al viejo zlib 1.2.6/c-blosc 1.9.0
  vendorizados), `<chrono>` de C++20 (libre, reemplaza a
  `boost_date_time`), Qt 6 (el soporte estándar de Qt5.15 terminó el 26
  de mayo de 2025, verificado mediante búsqueda), VTK (alternativa libre
  a Open Inventor, no es un reemplazo directo), el WiX v7 actual (los
  documentos del repositorio referencian v3.7), y el .NET moderno
  reemplazando al .NET Framework 4.0 vendorizado y hace tiempo retirado.
  `SVS` y `CORA1` recibieron correctamente "sin alternativa
  informada/directa" dado los hallazgos previos de proveedor sin
  resolver y especificidad de dominio. Se añadieron las nuevas fuentes
  (fin de vida de Qt, ciclo de vida de .NET Framework, notas de release
  de WiX) a §7.

### 1.21 Se aclaró que el reemplazo de TBB es C++17, no C++20

- El usuario señaló que la superposición de TBB con la biblioteca
  estándar se había descrito como C++17 en la conversación pero quería
  que también quedara explícito en el documento, ya que la pregunta
  original se planteó en torno a C++20. La tabla del §4 ya decía C++17
  correctamente, así que se añadió un párrafo aclaratorio destacado
  encima de la tabla explicando la distinción directamente: la
  superposición de TBB (`parallel_for`/`parallel_reduce`) es una
  característica de `<execution>` de **C++17**; C++20 no añadió ningún
  algoritmo paralelo nuevo en absoluto, solo primitivas de concurrencia
  de más bajo nivel (`std::jthread`, `std::latch`, `std::barrier`,
  `std::counting_semaphore`) y el soporte de zona horaria de `<chrono>`
  relevante para reemplazar `boost_date_time` — nada de lo cual cambia
  el panorama de TBB.
