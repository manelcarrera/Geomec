# Dependencias de Terceros y de Microsoft

Cada biblioteca/SDK externo que usa este proyecto: para qué sirve, si
requiere una licencia, cómo obtenerlo o compilarlo, y dónde espera el
build que resida. Ver `BUILD_AND_ARCHITECTURE.md` para la lista completa
de variables de entorno y `REPO_LAYOUT.md` para la visión general de
propiedad de carpetas.

Dos categorías:
- **Vendorizado** — ya incluido en `third_party/` en este repositorio.
- **No vendorizado** — el build espera que esto exista en la máquina
  (vía instalación de SDK + variable de entorno), no está incluido.

## Tabla de contenidos

1. [Vendorizado en `third_party/`](#1-vendorizado-en-third_party)
2. [Requerido pero NO vendorizado](#2-requerido-pero-no-vendorizado)
3. [Bibliotecas y herramientas de Microsoft](#3-bibliotecas-y-herramientas-de-microsoft)
4. [¿Cuáles de estos podría reemplazar el C++ moderno?](#4-cuales-de-estos-podria-reemplazar-el-c-moderno)
5. [Por qué se usó cada dependencia en vez de la biblioteca estándar](#5-por-que-se-uso-cada-dependencia-en-vez-de-la-biblioteca-estandar)
6. [Notas](#6-notas)
7. [Fuentes](#7-fuentes)

## 1. Vendorizado en `third_party/`

| Biblioteca | Se usa para | Licencia | Descarga / compilación | Dónde reside |
| --- | --- | --- | --- | --- |
| TBB (Threading Building Blocks) | Soporte de paralelismo/runtime para el build de Linux (qmake) | Libre, código abierto (Apache 2.0) | Binarios + cabeceras precompilados ya incluidos; upstream en [github.com/oneapi-src/oneTBB](https://github.com/oneapi-src/oneTBB) si alguna vez hace falta recompilar | `third_party/tbb/{linux,windows}` |
| HDF5 | Almacenamiento de datos/modelos | Libre, licencia HDF5 permisiva estilo BSD | Precompilado; upstream en [hdfgroup.org](https://www.hdfgroup.org/) (v1.8.17 aquí) | `third_party/hdf5/{linux,windows,linux_1.8.17,windows_1.8.17}` |
| zlib 1.2.6 | Compresión genérica, usada por Blosc/HDF5 | Libre, licencia zlib permisiva | Código fuente incluido; se compila como parte del grafo qmake/MSBuild; upstream en [zlib.net](https://zlib.net/) | `third_party/zlib-1.2.6` |
| Blosc (c-blosc 1.9.0) | Compresión rápida por bloques para datos de malla/resultados | Libre, BSD (incluye LZ4/Snappy/etc., cada una permisiva — ver `LICENSES/`) | Código fuente incluido; compilado vía `blosc_shared.vcxproj` / qmake; upstream en [github.com/Blosc/c-blosc](https://github.com/Blosc/c-blosc) | `third_party/c-blosc-1.9.0` |
| Google Test / Google Mock 1.7.0 | Framework de pruebas unitarias en C++ | Libre, BSD de 3 cláusulas | Código fuente incluido; upstream en [github.com/google/googletest](https://github.com/google/googletest) (existe una versión mucho más nueva upstream si alguna vez se actualiza) | `third_party/gmock-1.7.0` (incluye `gtest`) |
| Parallel STL (pstl) | Backend de algoritmos paralelos (Intel) | Libre, Apache 2.0 | Precompilado; upstream en [github.com/oneapi-src/oneDPL](https://github.com/oneapi-src/oneDPL) (proyecto sucesor) | `third_party/pstl/{linux,windows}` |
| CORA (Deltares CORA1) | Herramienta de análisis probabilístico/de confiabilidad integrada vía `geomec_cora_wrapper`/`geomec_cora_library` | **Propietaria — verificar los términos de licencia con Deltares** antes de redistribuir | Binarios/instalador provistos por el proveedor; no es algo para recompilar | `third_party/CORA1` |
| SVS (`SVS64.exe`) | Ejecutable/utilidad basado en Qt sin identificar | **Desconocido — no se pudo identificar el proveedor/licencia a partir del binario; revisar los registros de adquisición** | Binario provisto por el proveedor | `third_party/SVS` |
| Redistribuible de .NET Framework 4.0 | Prerrequisito de runtime para `CORA1` (una aplicación de interoperabilidad .NET/Excel) | Libre de redistribuir con fines de instalación (Microsoft) | Instalador redistribuible oficial de Microsoft | `third_party/dotNet` |
| Bitmaps de conjuntos de puntos-elemento | Assets de íconos/bitmaps de UI para la función de conjuntos de puntos-elemento | Assets propios (no código vendorizado) hechos con la herramienta gratuita "Greenfish Icon Editor Pro" | N/A — assets estáticos | `third_party/point_elementsets_bitmaps` |

## 2. Requerido pero NO vendorizado

Esto no está en el repositorio — debe instalarse en la máquina de build y
apuntarse mediante la(s) variable(s) de entorno listada(s).

| Dependencia | Se usa para | Licencia | Descarga / compilación | Dónde lo espera el build |
| --- | --- | --- | --- | --- |
| Diana | El solver central de FEA/geomecánica con el que se integra Geomec — la dependencia externa central de la app | **Comercial — requiere una licencia del proveedor de Diana** | Se obtiene directamente del distribuidor de Diana, no se compila aquí | `DIAROOT` (Windows: contiene `release`/`debug`/`share`); `$GM_SW_TOOL_PATH/diana` (Linux) |
| Qt | Framework de GUI, soporte de XML/OpenGL | Libre (LGPL, si se enlaza dinámicamente y se cumplen los términos) o licencia Comercial de pago | Qt Online Installer desde [qt.io](https://www.qt.io/download); nótese que `BUILD_AND_ARCHITECTURE.md` señala un desajuste de versión — `Geomec.vcxproj` enlaza Qt5, pero `geomec/INSTALL.txt` describe Qt 4.8.5 — confirmar cuál necesita realmente tu proyecto objetivo | `QTDIR` (Windows); `$GM_SW_TOOL_PATH/qt-4.8.5` (Linux) |
| Qwt | Plotting/widgets (solo Windows) | Libre (LGPL 2.1 con excepción) | Compilar desde código fuente o descargar precompilado, según tu versión de Qt, desde [qwt.sourceforge.io](https://qwt.sourceforge.io/) | `QWTDIR` |
| Open Inventor (OIV) | Visualización 3D basada en scene-graph | **Comercial — requiere una licencia comprada + archivo de licencia** | SDK comprado (v9.3.1 según la documentación), históricamente de VSG/FEI | `OIVHOME`, archivo de licencia vía `VSG_LICENSE_FILE` |
| Bibliotecas de runtime de Intel | Soporte de runtime de C++/Fortran | La redistribución del runtime es libre; compilar *con* el compilador de Intel requiere una licencia de Intel de pago | Intel oneAPI HPC Toolkit / bibliotecas redistribuibles desde [intel.com](https://www.intel.com/) | `INTEL` |
| Boost | Bibliotecas de C++ de propósito general (p. ej. `boost_date_time`) | Libre, Boost Software License permisiva | Descargar código fuente / precompilado desde [boost.org](https://www.boost.org/); compilar solo las bibliotecas compiladas necesarias | `$(BOOST)` (grafo qmake de Linux) |
| GLib | Biblioteca de utilidades de bajo nivel (solo build de Linux) | Libre, LGPL | Instalar vía el gestor de paquetes de la distro (p. ej. `libglib2.0-dev`) — el repositorio actualmente fija una ruta específica de un sitio en `qmake/basepath.pri` que debería reemplazarse por una ruta local | Referenciado vía `GLIB2PATH` en `qmake/basepath.pri` (actualmente una ruta interna fija — necesita actualizarse para un build portable) |
| Python | Scripting/integración para herramientas y plugins seleccionados (`py/`) | Libre, licencia PSF | Instalación estándar de Python | Referenciado vía `PYTHONROOT` en `qmake/basepath.pri` (actualmente una ruta interna fija) |
| FlexLM | Licenciamiento/DRM de la aplicación para el propio Geomec | **Requiere un acuerdo con el proveedor Flexera** para incrustar/distribuir | Provisto por Flexera como parte de un acuerdo de integración de licenciamiento | `FLEXLM` (derivado de `DIAROOT`/`GMROOT`) |
| Hasp (Sentinel HASP) | Licenciamiento basado en dongle de hardware, alternativa/complemento a FlexLM | **Requiere un acuerdo con el proveedor Thales/Sentinel** | Provisto por Thales como parte de un acuerdo de integración de licenciamiento | Driver + licencia instalados según `geomec/INSTALL.txt` |

## 3. Bibliotecas y herramientas de Microsoft

| Elemento | Se usa para | Licencia | Descarga / compilación | Dónde lo espera el build |
| --- | --- | --- | --- | --- |
| MFC (Microsoft Foundation Classes) | Framework de GUI de Windows heredado, usado por `platform/MfcGuiFrame`, `platform/MfcModelFrame`, `platform/WindowsRegistry`, y envuelto por `platform/qtwinmigrate` | Libre — incluido con Visual Studio (cualquier edición, incluida la Community gratuita) siempre que esté instalado el componente opcional "MFC y ATL" | Se instala vía el Instalador de Visual Studio, no se descarga por separado | Viene de la instalación de VS/Windows SDK; sin ruta en el repositorio |
| Windows SDK | Cabeceras de Win32 (`windows.h`, etc.) necesarias para el código de MFC/registro e integraciones nativas de Windows | Libre (Microsoft) | Incluido con Visual Studio, o independiente desde Microsoft | Viene de la instalación de VS; sin ruta en el repositorio |
| .NET Framework 4.0 | Runtime para `third_party/CORA1` (ver arriba) | Libre de redistribuir | Instalador ya incluido en el repositorio | `third_party/dotNet` |
| WiX Toolset | Construye los instaladores `.msi`/bundle de Windows en `wix/` | Libre, código abierto (originado en Microsoft, ahora proyecto comunitario/de .NET Foundation) | v3.7 según la documentación — descargar desde [wixtoolset.org](https://wixtoolset.org/) o sus releases de GitHub | La variable de entorno `WixToolPath` apunta a su directorio `bin/` |
| Toolset de Visual C++ / MSVC (v140) | Compilador/enlazador para el build de Windows | Libre (edición Community) o de pago (Professional/Enterprise) | Instalador de Visual Studio 2012/2015+, con el componente de toolset v140 | Seleccionado como el toolset de plataforma en `Geomec.vcxproj` |

## 4. ¿Cuáles de estos podría reemplazar el C++ moderno?

Respuesta corta: dos son genuinamente reemplazables por características
estándar de C++ (uno de ellos casi totalmente ya superado), uno es
*parcialmente* reemplazable con una salvedad importante, y todo lo demás
resuelve un problema que la biblioteca estándar de C++ no aborda en
absoluto, sin importar la versión.

**Aclarando qué hizo realmente cada versión de C++, ya que es fácil
confundir "C++20" con "C++ moderno" en general:** la superposición de TBB
(bucles de estilo `parallel_for`/`parallel_reduce`) es con los algoritmos
paralelos de `<execution>` de **C++17**, no C++20 — C++20 en sí no añadió
ningún algoritmo paralelo nuevo. Lo que C++20 *sí* añadió en el lado de
concurrencia es de nivel más bajo: soporte de calendario/zona horaria en
`<chrono>` (la pieza relevante para reemplazar `boost_date_time`, fila de
abajo), más `std::jthread`, `std::latch`, `std::barrier`, y
`std::counting_semaphore` — bloques de construcción útiles, pero ninguno
reemplaza el valor real de TBB (un planificador work-stealing,
contenedores concurrentes, el flow graph). Así que: la única pieza de TBB
con un reemplazo estándar es una característica de C++17; nada en
C++20/23 cambia ese panorama para TBB específicamente.

| Dependencia | ¿Reemplazable por C++ moderno? | Con qué | Salvedad |
| --- | --- | --- | --- |
| Parallel STL (`third_party/pstl`) | **Sí, ya en gran parte obsoleto** | Algoritmos paralelos `<execution>` de C++17 (`std::execution::par`, `std::for_each`/`std::transform`/`std::reduce` con una política de ejecución) | `pstl` fue la propia implementación temprana de Intel de lo que C++17 luego estandarizó en el propio lenguaje; los compiladores actuales lo distribuyen de forma nativa, así que una dependencia dedicada a `pstl` es en gran parte redundante en un toolchain moderno |
| Boost (`boost_date_time`) | **Sí, en un compilador suficientemente actual** | Soporte de calendario y zona horaria de `<chrono>` de C++20 (`std::chrono::year_month_day`, `std::chrono::zoned_time`, base de datos de zonas horarias IANA) | Necesita un compilador genuinamente actual: GCC solo lo soporta completamente desde GCC 14.2 (el GCC 15 de esta máquina califica), MSVC desde VS2019 16.10. Los compiladores más antiguos no pueden hacer esto — comprobarlo antes de depender de ello |
| TBB | **Parcialmente, y con una salvedad real en Linux** | Los algoritmos paralelos `<execution>` de C++17 cubren el caso de uso más simple de TBB (bucles de estilo `parallel_for`/`parallel_reduce` sobre un rango) | En GCC/libstdc++, los propios algoritmos paralelos de C++17 están implementados *usando* TBB — simplemente `#include <execution>` crea una dependencia de enlazado con TBB. Así que en Linux esto no elimina TBB, solo hace la dependencia indirecta. En MSVC, los algoritmos paralelos de la STL tienen su propio backend y genuinamente no necesitan TBB. En cualquier caso, las otras características de TBB — contenedores concurrentes (`concurrent_hash_map`, `concurrent_queue`), el flow graph, `task_arena`/`task_group`, el asignador escalable (`tbbmalloc`) — **no tienen equivalente estándar de C++ en ninguna versión**, incluyendo C++20/23 |
| HDF5, zlib, Blosc (c-blosc) | No | — | Los formatos de archivo y la compresión están completamente fuera del alcance del estándar de C++; no existe un `std::compress` ni una API estándar de archivos de datos científicos |
| Google Test/Mock | No | — | C++ todavía no tiene un framework de pruebas incorporado estándar; un reemplazo sería otra biblioteca de terceros (Catch2, doctest), no una característica del lenguaje |
| Qt, Qwt, MFC | No | — | C++ no tiene ningún toolkit de GUI estándar en ninguna versión |
| Open Inventor | No | — | No existe una API estándar de scene-graph/renderizado en modo retenido en C++ (las propuestas de estandarización relacionadas con gráficos fueron abandonadas) |
| Diana | No | — | Todo un producto externo de solver de FEA; no es algo de nivel de lenguaje en absoluto |
| FlexLM, Hasp | No | — | Infraestructura de licenciamiento/DRM, no relacionada con características del lenguaje |
| CORA1, SVS, dotNet, WiX | No | — | Aplicaciones/herramientas externas y un toolchain de instaladores; no relacionado con el lenguaje |
| Bibliotecas de runtime de Intel | Indirectamente | — | Solo relevante si se compila con el compilador de Intel; cambiar a MSVC/GCC/Clang elimina esto como elección de toolchain, no como sustitución de una característica del lenguaje |

## 5. Por qué se usó cada dependencia en vez de la biblioteca estándar

Donde aplica una pregunta genuina de "vs. la biblioteca estándar", con
cifras publicadas reales en vez de afirmaciones vagas. Varias filas no
tienen ninguna comparación de velocidad significativa, porque no existe
(o no existió) una alternativa estándar contra la cual comparar — eso se
señala explícitamente en vez de omitirse. La última columna indica,
específicamente, cuándo la mejor opción hoy es el propio estándar de C++
u otro proyecto libre/de código abierto, frente a los casos sin ninguna
alternativa libre realista.

| Dependencia | Alternativa solo con la biblioteca estándar | Por qué se eligió la dependencia en su lugar | Datos cuantitativos | Mejor opción actual disponible hoy |
| --- | --- | --- | --- | --- |
| HDF5 | `fstream` simple/lectura-escritura binaria cruda con indexación hecha a mano | Formato jerárquico autodescriptivo (datos + metadatos juntos), acceso parcial/aleatorio a un array enorme sin cargar el archivo completo, chunking y filtros de compresión incorporados, portable entre plataformas y lenguajes — nada de lo cual ofrece el I/O binario crudo a ninguna velocidad | Para escrituras grandes, contiguas y bien ajustadas, HDF5 alcanza **~70–95% del rendimiento del sistema de archivos crudo** (5–30% de sobrecarga). Un acceso mal ajustado (escrituras pequeñas/desalineadas, mal tamaño de chunk) puede ser **2–3× más lento** que un formato de array crudo plano en la misma prueba. La sobrecarga compra estructura y portabilidad que el I/O crudo no puede ofrecer a ninguna velocidad | El **HDF5 1.14.x** actual (libre, misma licencia) es una simple actualización de versión desde el 1.8.17 vendorizado. **Zarr** (libre, BSD) es una alternativa más nueva mejor adaptada a acceso en chunks paralelo/nativo de nube, si alguna vez se rediseña la capa de almacenamiento desde cero |
| TBB | `std::thread`; `<execution>` de C++17 solo en MSVC (ver §4) | Un planificador work-stealing que balancea automáticamente la carga de bucles paralelos, patrones de más alto nivel (`parallel_for`, pipelines, flow graphs) que `std::thread` no ofrece, y un asignador escalable (`tbbmalloc`) que evita la contención de bloqueo del heap en código multihilo con muchas asignaciones | Cifras del propio Intel: reenlazar solo con `tbbmalloc` típicamente da **20–30% de secciones paralelas más rápidas**, hasta **~4×** en casos extremos; un caso medido pasó de 94ms a 50ms (**~1.9×**) en un quad-core; un caso en un Xeon Phi de 60 núcleos vio una fase correr **hasta 56× más rápido** | **oneTBB** (el rebranding actual, libre y con licencia Apache-2.0, de Intel) es el sucesor directo y activamente mantenido de este paquete antiguo. En GCC/Clang, **oneDPL** es el sucesor libre del `pstl` vendorizado y cubre un terreno similar |
| zlib / Blosc (`c-blosc`) | No existe ninguna facilidad de compresión estándar en ninguna versión de C++ | Reduce el tamaño en disco/memoria de grandes conjuntos de datos de malla/resultados; Blosc específicamente añade un pre-filtro de byte-shuffling ajustado para arrays numéricos tipados (exactamente el tipo que almacena Geomec) más multihilo | Benchmarks del propio Blosc: compresión hasta **~8 GB/s** (más rápido que un `memcpy` simple a ~7.5 GB/s en la misma prueba); descompresión hasta **~35–50 GB/s** con PGO, con descompresión de un solo hilo más rápida que `memcpy()` en todos los niveles de compresión; el códec Zstd de Blosc alcanza **>10 GB/s** de descompresión, más del **2×** el propio pico de ~4 GB/s de zlib | **zlib-ng** (libre, acelerado con SIMD, reemplazo directo de zlib) y **Zstd** (libre, mejor ratio *y* velocidad que zlib, ya usable como códec de Blosc) superan ambos al zlib 1.2.6 vendorizado (2012). **Blosc2**/`c-blosc2` (libre) es el sucesor actual y activamente mantenido del 1.9.0 vendorizado |
| Google Test/Mock | Comprobaciones hechas a mano con `assert()`/`if` | Aserciones estandarizadas con salida de fallo legible, descubrimiento/registro de pruebas, y objetos mock generados | No aplica — una dependencia de tiempo de build/desarrollo sin huella en tiempo de ejecución en la aplicación distribuida | El **GoogleTest** actual (gtest+gmock fusionados en un solo repositorio libre) es una simple actualización desde el 1.7.0 vendorizado (2013). **Catch2** (libre, BSD-2) es una alternativa moderna popular con un estilo de aserción más ergonómico y con menos macros |
| Boost (`boost_date_time`) | Ninguna, antes de C++20 | Ninguna biblioteca estándar de C++ tenía aritmética de fecha/calendario consciente de zona horaria antes de `<chrono>` de C++20 — Boost era la única opción cuando se escribió este código | No es una pregunta de velocidad — es una pregunta de existencia. Ver §4 para la opción de reemplazo actual con `<chrono>` de C++20 | **`<chrono>` de C++20** — libre, porque es el propio estándar, una vez en un compilador suficientemente actual (ver §4 para los umbrales de versión) |
| pstl | `<execution>` de C++17, ahora incorporado en el compilador (ver §4) | Cuando se adoptó, los algoritmos paralelos de C++17 todavía no existían en ningún compilador; `pstl` fue la propia implementación de referencia temprana de Intel de la misma idea | Mismo perfil de rendimiento que el `<execution>` nativo de C++17 hoy, ya que eso es literalmente en lo que se convirtió | Superado — ver §4. En GCC/Clang, **oneDPL** es el sucesor libre y activamente mantenido |
| Qt, Qwt | Ninguna | No existe ningún toolkit de GUI estándar de C++ ni biblioteca de widgets de plotting científico en ninguna versión | No es una comparación de velocidad — es una comparación de "no existe" | Ambos ya libres (LGPL). **Qt 6** es la línea actual activamente desarrollada — el soporte estándar de Qt 5.15 terminó el 26 de mayo de 2025; los usuarios de código abierto ya no reciben más parches oficiales (solo una colección de parches comunitaria mantenida por KDE). Qwt actual sigue la línea de Qt que se esté usando |
| Open Inventor | Ninguna | No existe una API estándar de scene-graph/renderizado en modo retenido en C++ | No es una comparación de velocidad — es una comparación de "no existe" | No hay un equivalente libre con el mismo conjunto completo de características, pero **VTK** (libre, BSD, ampliamente usado justo en este espacio de visualización científica/de ingeniería) es la alternativa libre más creíble si alguna vez se reconsidera todo el stack de visualización — no es un reemplazo directo, es una reescritura |
| Diana | Ninguna | Todo un producto externo de solver de FEA — no es algo que una biblioteca o característica del lenguaje sustituya | N/A | No hay un reemplazo libre realista sin efectivamente convertirse en un producto distinto. Existen solvers de FEA libres/abiertos en general (Code_Aster, CalculiX) pero no igualan el conjunto de características geomecánicas específico de Diana |
| FlexLM, Hasp | Ninguna | Infraestructura de licenciamiento/DRM, no relacionada con la biblioteca estándar en ninguna versión | N/A | Existen opciones de licenciamiento libres/abiertas (p. ej. `licensecc`), pero las empresas suelen mantener las comerciales por soporte/cumplimiento normativo — no es realmente una pregunta de actualización técnica |
| MFC | Ninguna (hasta que se adoptó Qt junto a él) | El framework de GUI nativo de Windows en la época en que se escribió originalmente este código; no existía ningún toolkit de GUI multiplataforma o estándar de C++ para usar en su lugar | N/A — ver `MFC_MODERNIZATION.md` para la alternativa moderna (Qt) disponible ahora | **Qt** (libre, LGPL, ya usado en el resto de esta app) — ver `MFC_MODERNIZATION.md` para el camino de migración completo |
| WiX | Ninguna | Herramienta de instaladores, no es una cuestión de nivel de lenguaje | N/A | Ya libre; el **WiX v7** actual (cadencia de release anual, abril de 2026) es la línea activamente mantenida frente al v3.7 que referencian los documentos de este repositorio — varias versiones mayores por detrás |
| CORA1 | Ninguna | La propia herramienta de análisis probabilístico/de confiabilidad de Deltares; un producto de ingeniería específico, no una biblioteca | N/A | No hay un equivalente libre directo. **OpenTURNS** (libre, LGPL) cubre la cuantificación de incertidumbre en general pero no coincide con el dominio específico del flujo de trabajo de CORA |
| SVS | Ninguna | Proveedor/propósito sin identificar (ver §1) | N/A | No se puede recomendar una alternativa sin saber qué hace realmente — resolver primero la pregunta de "qué es SVS" |
| .NET Framework 4.0 | Ninguna | Prerrequisito de runtime para `CORA1`, una aplicación de interoperabilidad .NET/Excel | N/A | Significativamente desactualizado (era 2010) — Microsoft ya retiró el soporte de versiones de .NET Framework anteriores a 4.5.2, y .NET Framework en su conjunto está "efectivamente en modo de mantenimiento" según la propia documentación de ciclo de vida de Microsoft. El **.NET** actual (el sucesor libre, multiplataforma y activamente desarrollado, antes ".NET Core") es el objetivo recomendado por la propia Microsoft para cualquier cosa nueva |
| Bibliotecas de runtime de Intel | Ninguna | Solo necesario cuando el código se compila con el compilador de Intel — un requisito de toolchain, no una elección de biblioteca | N/A | El runtime **Intel oneAPI** actual es el equivalente moderno si se sigue con el compilador de Intel; cambiar a MSVC/GCC/Clang elimina esta dependencia por completo (todos libres) |

## 6. Notas

- "Requiere una licencia" arriba significa una licencia *comercial/legal*
  (algo para comprar o contratar), distinto de las licencias de código
  abierto (BSD/Apache/LGPL/zlib/etc.) que son libres pero aun así
  conllevan obligaciones de atribución/cumplimiento — revisar el archivo
  de licencia real de cada paquete antes de redistribuir.
- `CORA1` y `SVS` están señalados como necesitados de verificación porque
  nada en el repositorio (sin archivo de licencia, sin una cadena de
  proveedor clara en el binario de `SVS`) confirma sus términos exactos —
  no asumir que son libremente redistribuibles sin verificar con quien
  originalmente los obtuvo.
- Las rutas fijas del build de Linux para GLib y Python
  (`qmake/basepath.pri`) apuntan a una disposición de máquina interna
  específica (`/glb/data/...`); un build de Linux portable necesita que
  se reemplacen por rutas válidas en la máquina local, tal como ya se
  señala en `BUILD_AND_ARCHITECTURE.md` sección 5.3.

## 7. Fuentes

Hechos en §4 sobre soporte de compiladores y la relación GCC/TBB, y las
cifras de rendimiento de HDF5/Blosc/TBB de §5, vigentes a septiembre de
2026:

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
