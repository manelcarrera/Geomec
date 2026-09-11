# Guía de Arquitectura, Build y Despliegue de Geomec

Este documento describe el repositorio tal como existe en este árbol de código fuente. Cubre la aplicación principal, las bibliotecas compartidas, los componentes de terceros vendorizados, las pruebas, y los caminos de build separados de Windows y Linux.

## Tabla de contenidos

1. [Visión General del Repositorio](#1-vision-general-del-repositorio)
2. [Aplicación Principal y Módulos](#2-aplicacion-principal-y-modulos)
   - 2.1 [Aplicación principal de Windows: `geomec/`](#21-aplicacion-principal-de-windows-geomec)
   - 2.2 [Bibliotecas de framework y de dominio](#22-bibliotecas-de-framework-y-de-dominio)
     - 2.2.1 [`fw51/`](#221-fw51)
     - 2.2.2 [`lib/`](#222-lib)
   - 2.3 [Ejecutables e integraciones](#23-ejecutables-e-integraciones)
3. [Dependencias Externas](#3-dependencias-externas)
   - 3.1 [Dependencias requeridas de plataforma/runtime](#31-dependencias-requeridas-de-plataformaruntime)
   - 3.2 [Dependencias vendorizadas bajo `third_party/`](#32-dependencias-vendorizadas-bajo-third_party)
4. [Build de Windows](#4-build-de-windows)
   - 4.1 [Prerrequisitos de toolchain y SDK](#41-prerrequisitos-de-toolchain-y-sdk)
   - 4.2 [Variables de entorno requeridas](#42-variables-de-entorno-requeridas)
   - 4.3 [Configurar licenciamiento y Diana](#43-configurar-licenciamiento-y-diana)
   - 4.4 [Compilar con Visual Studio](#44-compilar-con-visual-studio)
   - 4.5 [Despliegue en Windows](#45-despliegue-en-windows)
5. [Build de Linux](#5-build-de-linux)
   - 5.1 [Prerrequisitos de Linux](#51-prerrequisitos-de-linux)
   - 5.2 [Configurar y compilar el grafo de qmake](#52-configurar-y-compilar-el-grafo-de-qmake)
   - 5.3 [Advertencias del build de Linux](#53-advertencias-del-build-de-linux)
   - 5.4 [Despliegue en Linux](#54-despliegue-en-linux)
6. [Pruebas y Comprobaciones Básicas](#6-pruebas-y-comprobaciones-basicas)
7. [Orden de Build Recomendado](#7-orden-de-build-recomendado)
8. [Archivos para Leer Primero](#8-archivos-para-leer-primero)
9. [Limitaciones Actuales del Repositorio](#9-limitaciones-actuales-del-repositorio)

## 1. Visión General del Repositorio

Geomec es una aplicación grande en C++ compuesta de varias capas:

```text
Geomec/
|-- geomec/                 Aplicación principal de Geomec para Windows y código de GUI
|-- geomec.pro              Superproyecto principal de qmake
|-- Geomec.sln              Solución de Visual Studio
|-- lib/                    Bibliotecas propias de Geomec/Diana/FW51 (multiplataforma)
|-- platform/               Bibliotecas propias que hoy solo compilan en Windows
|-- third_party/            SDKs e instaladores vendorizados (HDF5, TBB, zlib, Blosc,
|                            gmock/gtest, CORA, SVS, .NET, datos de bitmaps)
|-- fw51/                   Código de geometría, malla, tensores y modelos de Framework 51
|-- app/                    Aplicaciones auxiliares y proyectos de aplicaciones heredadas
|-- geomec_shell/           Ejecutable de consola/shell
|-- gm_diana_app/           Aplicación orientada a Diana
|-- geomec_cora_wrapper/    Wrapper de integración con CORA
|-- geomec_selector/        Aplicación selectora
|-- Tests/                  Código fuente de pruebas en C++ (TestLib, TestRunner, Fw51, Materials, ...)
|-- TestData/               Grandes conjuntos de datos de modelos/entrada usados por las pruebas
|-- IntegrationTests/       Soporte de pruebas de integración basadas en scripts (antes `test/`)
|-- archive/                Código histórico/huérfano que ya no forma parte de ningún target de build
|-- wix/                    Proyectos de instalador de Windows
|-- qmake/                  Configuración de qmake compartida y definiciones de dependencias
|-- vsprops/                Hojas de propiedades de Visual Studio
|-- py/                     Herramientas Python y ayudantes de integración
|-- res/                    Scripts de build, control de versiones y utilidades
|-- version/                Cabeceras de versión y metadatos de versión
```

`lib/`, `platform/`, y `third_party/` solían ser un único directorio (`lib/`) que mezclaba código propio con binarios vendorizados de tamaños muy distintos. Se separaron para que el código propio (`lib/`, `platform/`) esté claramente separado del material vendorizado (`third_party/`) que no se mantiene aquí. `platform/` contiene el subconjunto de bibliotecas propias que hoy no tienen proyecto qmake (`.pro`) y por tanto solo compilan en Windows (p. ej. las basadas en MFC `MfcGuiFrame`/`MfcModelFrame`); todo lo que sigue en `lib/` compila en ambas plataformas desde el mismo árbol de código fuente. De forma similar, `Tests/` solía contener tanto el (diminuto) código fuente de pruebas en C++ como un subdirectorio `Models` de más de 400 MB de datos binarios de prueba; esos datos ahora viven en el directorio de nivel superior `TestData/`. El antiguo directorio `test/` en minúsculas se renombró a `IntegrationTests/` para eliminar un par de directorios que solo diferían en mayúsculas/minúsculas (`Tests/` vs `test/`), lo cual es un riesgo en checkouts de Windows/macOS que no distinguen mayúsculas de minúsculas. Ver `FOLDER_STRUCTURE_PROPOSAL.md` para la justificación completa. `Prototypes/` se dejó intencionalmente en su sitio aunque es heredado/experimental, porque sigue siendo un target `SUBDIRS` de qmake activo (`ModGMBus`) con rutas de include relativas sensibles a la profundidad; mover esto se consideró de mayor riesgo que su beneficio de etiquetado.

El repositorio contiene tanto código fuente como material precompilado/de runtime. El directorio `third_party/` no es código fuente que mantenga Geomec; contiene binarios específicos de plataforma, archivos de SDK, instaladores, y ejemplos vendorizados de proyectos upstream.

## 2. Aplicación Principal y Módulos

### 2.1 Aplicación principal de Windows: `geomec/`

El directorio `geomec/` es la aplicación de escritorio principal de Geomec. Sus módulos están organizados por dominio:

| Módulo | Responsabilidad |
| --- | --- |
| `BaseModel` | Modelo base e infraestructura común de modelos |
| `Geology` | Objetos y operaciones geológicas |
| `HexaModel` | Soporte de modelos hexaédricos |
| `TetraModel` | Soporte de modelos tetraédricos y ayudantes de importación/exportación |
| `Material` | Materiales y edición de materiales |
| `Value` | Objetos de valor y UI relacionada con valores |
| `View` | Vista, visualización, y visualización de tensores |
| `OpenInventor` | Integración con Open Inventor |
| `GUI`, `dlg`, `delegate`, `observer` | Componentes de GUI Qt/MFC y manejo de eventos |
| `FlexLM` | Integración de licenciamiento FlexLM |
| `DStaborLink` | Integración externa/de análisis |
| `IT` | Soporte de integración y plug-ins |
| `Test`, `doc`, `Help` | Pruebas y documentación de usuario/desarrollador |

El proyecto de Visual Studio correspondiente es `geomec/Geomec.vcxproj`. La solución raíz es `Geomec.sln`.

### 2.2 Bibliotecas de framework y de dominio

#### 2.2.1 `fw51/`

FW51 contiene estructuras de datos geométricas y numéricas reutilizables:

- `FieldValueFile`: manejo de archivos de campo/valor.
- `Geometry`: entidades y operaciones geométricas.
- `ModelFrame`: abstracciones de model-frame.
- `OpenGLFrame`: código de framework orientado a OpenGL.
- `Tensor`: matemática de tensores y datos de visualización.
- `TetMesh`: soporte de malla tetraédrica.
- `Triangle`: soporte de triángulos y malla de superficie.
- `GuiFrame`: código de framework de GUI; el proyecto qmake lo marca como no usado normalmente.

#### 2.2.2 `lib/`

Los principales grupos de bibliotecas listados por `geomec.pro` son:

- **Núcleo/dominio:** `GeomecModel`, `FemAppModel`, `MaterialLibrary`, `MaterialLibraryXML`, `Quantity`, `NewWellScheme`, `WellScheme`, `CommonWellScheme`, `DCasing`, `CutSurfaceUtilModel`, `MeshDataCacher`.
- **Integración con Diana:** `DianaCore`, `DianaSignals`, `DianaStartup`, `DianaGlue`, `ConsoleLibrary`.
- **Integraciones de Geomec:** `geomec_rgi_library`, `geomec_cora_library`, `RGInterface`, `GMSkuaConnector`.
- **Utilidades:** `Util`, `QUtil`, `RPN`, `GeomecStringTable`, `qtiocompressor`.
- **Malla/compresión/almacenamiento:** `TSMesh`, `blosc`, `blosc_filter`, HDF5, zlib, y soporte relacionado.
- **Pruebas:** Google Mock/gtest bajo `third_party/gmock-1.7.0` y bibliotecas de pruebas bajo `Tests/`.

Las listas de dependencias de qmake están centralizadas en `qmake/libs.pri`, mientras que las rutas de include y las rutas de plataforma las ensamblan `qmake/all.pri`, `qmake/globals.pri`, y `qmake/basepath.pri`.

### 2.3 Ejecutables e integraciones

El superproyecto de qmake define o referencia estos proyectos a nivel de ejecutable:

- `geomec/Geomec.vcxproj`: aplicación de escritorio principal de Windows.
- `geomec_shell/geomec_shell.pro`: aplicación de consola/shell que usa las bibliotecas de Geomec, FW51, Diana, y de terceros.
- `gm_diana_app/gm_diana_app.pro`: aplicación de Diana que usa `DianaStartup`, `DianaCore`, `QUtil`, y `Util`.
- `geomec_cora_wrapper/geomec_cora_wrapper.pro`: frontera ejecutable/biblioteca del wrapper de CORA.
- `geomec_selector/geomec_selector.pro`: aplicación selectora.
- `app/`: aplicaciones auxiliares como `geomec_launcher`, `diana_app`, `createfilos_app`, `geomec_rgi`, `geomec_cora`, y `MPKernelApp`.

También hay un grafo de qmake heredado más pequeño en `app.pro` que compila `lib/lib01` antes de `app/app01`. Es independiente del grafo principal de `geomec.pro`.

## 3. Dependencias Externas

### 3.1 Dependencias requeridas de plataforma/runtime

La aplicación está fuertemente acoplada a la distribución de Diana y sus bibliotecas de runtime. También usa:

- Qt para la GUI y soporte de XML/OpenGL.
- Qwt para plotting/widgets en Windows.
- Open Inventor para visualización 3D.
- Bibliotecas de runtime de Intel.
- Componentes de licenciamiento FlexLM y/o Hasp.
- HDF5 para almacenamiento de datos.
- TBB para soporte de paralelismo/runtime en la configuración de qmake de Linux.
- Boost, GLib, zlib, Blosc, y otras bibliotecas de soporte.
- Componentes de integración de Python para herramientas/plugins seleccionados.

Muchas de estas dependencias se referencian mediante variables de entorno en vez de descargarse por el build. Una compilación exitosa por tanto requiere cabeceras de SDK, bibliotecas de importación/estáticas, DLLs/shared objects de runtime, y archivos de licencia que coincidan.

### 3.2 Dependencias vendorizadas bajo `third_party/`

El repositorio contiene copias específicas de plataforma o insumos de build para HDF5, TBB, zlib, Blosc, y Google Test/Mock bajo `third_party/`. La compresión de I/O de Qt (`qtiocompressor`) es un pequeño wrapper propio y permanece bajo `lib/`. `third_party/` también contiene CORA, SVS, .NET, datos de bitmaps de elementos-punto, y material de instalador/runtime (antes el directorio separado `3rdParty/`).

No asumir que cada binario vendorizado se usa en cada target. El conjunto de dependencias activo depende de la plataforma, configuración, y ejecutable seleccionados. Las hojas de propiedades de Visual Studio son la fuente autoritativa para las rutas de bibliotecas de Windows; los archivos `.pri` de qmake son la fuente autoritativa para el grafo qmake/Linux.

## 4. Build de Windows

El camino de Windows es el camino de build más completo y explícito de este repositorio.

### 4.1 Prerrequisitos de toolchain y SDK

Las instrucciones históricas de instalación en `geomec/INSTALL.txt` especifican:

- Visual Studio 2012 Premium con Update 4.
- Herramientas de build nativas x64 y el toolset compatible con Visual Studio v140.
- Bibliotecas de runtime de Intel C++/Fortran.
- WiX Toolset 3.7 para instaladores.
- Qt y Qwt.
- Open Inventor SDK 9.3.1 para Visual Studio x64.
- Distribución de Diana con directorios `release`, `debug`, y `share`.
- Driver de Hasp y/o el sistema de licenciamiento configurado.

Los archivos de Visual Studio incluidos en el repositorio contienen un desajuste de versión importante: `Geomec.vcxproj` usa Visual Studio v140 y enlaza nombres de biblioteca de Qt5 como `Qt5Core`, `Qt5Widgets`, y `Qt5Xml`, mientras que el texto más antiguo en `geomec/INSTALL.txt` describe compilar Qt 4.8.5. Confirmar la versión de Qt esperada por el proyecto/hojas de propiedades seleccionados antes de instalar o recompilar Qt.

### 4.2 Variables de entorno requeridas

Las hojas de propiedades de Windows esperan variables que incluyen:

```text
INTEL              Directorio de runtime de Intel
QTDIR              Directorio de instalación de Qt
QWTDIR             Directorio de instalación de Qwt
WixToolPath        Directorio bin de WiX
DIAROOT            Raíz de Diana que contiene release/debug/share
OIVHOME            Directorio del SDK de Open Inventor
VSG_LICENSE_FILE   Ruta al password.dat de Open Inventor
VSG_LICENSE_DEBUG  Ruta opcional al log de licencia de Open Inventor
```

Añadir al menos estos directorios al `PATH`:

```text
%QTDIR%\bin
%QWTDIR%\lib
%INTEL%
```

El proyecto deriva valores como `DIAPATH`, `DIASHARE`, `DIAOBJ`, `FWDIR`, `FLEXLM`, y `USERDLLS` a partir de `DIAROOT` y la estructura del repositorio. Ver `geomec/Environment.props` y `vsprops/BuildCommon.props`.

Open Inventor puede poner sus directorios de DLL de Qt al principio del `PATH`. Mover esos directorios después de `%QTDIR%\bin` para que se encuentren primero las DLLs de Qt previstas.

### 4.3 Configurar licenciamiento y Diana

1. Instalar/configurar el driver de Hasp si la versión de Diana seleccionada lo requiere.
2. Configurar el servidor de licencias o `hasplm.ini` según lo requiera la instalación local.
3. Descomprimir la distribución de Diana bajo el directorio referenciado por `DIAROOT`.
4. Verificar que la DLL de Hasp requerida esté presente en `release\bin` y `debug\bin` de Diana.
5. Proveer la ruta del `password.dat` de Open Inventor mediante `VSG_LICENSE_FILE`.
6. Revisar las entradas de registro descritas en `geomec/INSTALL.txt`, especialmente `DIAPATH`, `DIASHARE`, `DianaExeName`, `FgvExeName`, y `InputExeName`.

### 4.4 Compilar con Visual Studio

Usar la solución raíz:

```text
Geomec.sln
```

En Visual Studio:

1. Abrir `Geomec.sln`.
2. Seleccionar la plataforma `x64`.
3. Seleccionar `Debug`, `ReleaseWithDebug`, o `Release`.
4. Compilar la solución.
5. Establecer `Geomec` como proyecto de inicio al depurar la aplicación de escritorio.
6. Para depuración Debug y Release, configurar el entorno del ejecutable para incluir las rutas representadas por `$(PATHPREFIX);$(PATH);$(OIVPATH)`.

La solución incluye la aplicación principal, los proyectos de FW51, los proyectos de bibliotecas, las aplicaciones auxiliares, y las pruebas. Los archivos `.vcxproj` individuales se pueden compilar con MSBuild cuando no hace falta un build de toda la solución. Un comando representativo desde una consola de desarrollador de Visual Studio es:

```bat
msbuild Geomec.sln /m /p:Configuration=Release /p:Platform=x64
```

Usar la versión de Visual Studio instalada para la solución. Los proyectos más antiguos pueden requerir conversión de proyecto o el toolset v140 original.

### 4.5 Despliegue en Windows

Los proyectos de instalador están bajo `wix/`, incluyendo:

- `WixInstall32` y `WixInstall64` para proyectos de instalación de 32/64 bits.
- `WixBundledInstall` para instalación empaquetada (bundle).
- `WixPostInstall` y `WixHaspPostInstall` para acciones post-instalación.
- `WixInclude` para fragmentos de instalador compartidos y scripts auxiliares.

Un despliegue debe contener más que `Geomec.exe`:

- El ejecutable de Geomec correspondiente y sus DLLs dependientes.
- Las DLLs de runtime correctas de Qt y Qwt.
- DLLs de runtime de Open Inventor y configuración de licencia.
- Binarios de runtime de Diana y el directorio de datos `share` de Diana.
- Bibliotecas de FW51 y Geomec requeridas por el ejecutable.
- HDF5/TBB/Blosc/zlib u otras bibliotecas de runtime usadas por el build.
- Componentes de licenciamiento FlexLM/Hasp donde aplique.
- Archivos de configuración, entradas de registro, recursos, ayuda, y versión.

Usar el proyecto WiX para un instalador repetible en vez de copiar DLLs manualmente. Probar la aplicación instalada en una máquina sin el entorno de desarrollador, porque una ejecución exitosa en la máquina de desarrollo puede depender accidentalmente de entradas de `PATH` que no forman parte del despliegue.

## 5. Build de Linux

El camino de Linux se basa en qmake y Makefiles. Es menos autocontenido que la solución de Windows y asume una disposición interna de Diana/toolchain.

### 5.1 Prerrequisitos de Linux

`README.linux` requiere una variable de entorno llamada `GM_SW_TOOL_PATH`. Se espera que el directorio contenga estos subdirectorios o enlaces:

```text
$GM_SW_TOOL_PATH/diana       Distribución de Diana: release, debug, share
$GM_SW_TOOL_PATH/GCC         GCC 4.7.2 o más nuevo, incluyendo include y lib64
$GM_SW_TOOL_PATH/extra       Biblioteca UUID, incluyendo include/uuid y lib
$GM_SW_TOOL_PATH/qt-4.8.5   Instalación de Qt
```

Los archivos de qmake también referencian o esperan valores como:

```text
DIAPATH       Ruta de build/runtime de Diana
DIASRC        Ruta de código fuente/share de Diana
QTDIR         Ruta de Qt
INTEL         Ruta de runtime de Intel, si la usa el build local de Diana
BOOST         Instalación de Boost
LD_LIBRARY_PATH  Debe incluir Diana y las rutas de compilador/runtime requeridas
```

Ejecutar el script de comprobación del repositorio antes de configurar:

```sh
cd /path/to/Geomec
. ./qmake/check.environment.sh
```

El script actualmente reporta variables faltantes pero no hace fallar la shell porque sus líneas `exit 1` están comentadas. Tratar cualquier reporte como un error de configuración real.

### 5.2 Configurar y compilar el grafo de qmake

El punto de entrada previsto es `geomec.pro`, no el `app.pro` heredado más pequeño:

```sh
cd /path/to/Geomec
qmake -r geomec.pro CONFIG+=release BUILD_TYPE=release
make -j"$(nproc)"
```

Para un build de debug:

```sh
qmake -r geomec.pro CONFIG+=debug BUILD_TYPE=debug
make -j"$(nproc)"
```

Si la versión de qmake no entiende la sintaxis del proyecto o los módulos de Qt esperados, usar la versión de Qt/qmake asociada con la configuración objetivo. Los archivos de qmake usan `CONFIG += debug_and_release` y generan la salida bajo un árbol `bin/<configuration>` específico del build. `qmake/destination.pri` coloca los objetos intermedios bajo `.obj/<configuration>` y las bibliotecas bajo el árbol de salida correspondiente.

El grafo de qmake compila las bibliotecas antes que las aplicaciones. El target principal del shell enlaza las bibliotecas de Geomec, las bibliotecas de FW51, las bibliotecas de Diana, HDF5, Blosc, TBB, GLib, zlib, Boost, `dl`, y `rt` en Unix. El conjunto exacto de enlazado está definido en `geomec_shell/geomec_shell.pro`, `gm_diana_app/gm_diana_app.pro`, `qmake/libs.pri`, y los archivos `.pri` incluidos.

### 5.3 Advertencias del build de Linux

La configuración de Linux no es un build portable de una distribución moderna:

- `qmake/basepath.pri` contiene rutas específicas de un sitio como `/glb/data/...` y una ruta de GLib de RHEL/EasyBuild.
- Varias variables deben ser provistas por la instalación local de Diana/toolchain.
- La documentación de Linux incluida describe Qt 4.8.5, mientras que partes del código fuente usan módulos de Qt y configuraciones de proyecto asociadas con versiones más nuevas de Qt.
- `IntegrationTests/Tests/make.sh` contiene nombres de target y rutas antiguos y fijos, y debe tratarse como un ayudante histórico, no como el punto de entrada principal de build.

Antes de intentar un build completo, actualizar `qmake/basepath.pri` o proveer rutas compatibles mediante una configuración local, y luego verificar que la versión de Qt seleccionada, la ABI del compilador, las bibliotecas de Diana, HDF5, TBB, Boost, GLib, y el runtime de Python coincidan todos.

### 5.4 Despliegue en Linux

No hay una definición de instalador de Linux completa y autocontenida comparable a los proyectos WiX. El despliegue debe ensamblarse como un directorio que contenga:

```text
bin/                  Ejecutable(s) compilado(s)
lib/                  Bibliotecas compartidas de Geomec/FW51 compiladas, si se compilaron como shared
diana/                Runtime y datos share de Diana release/debug correspondientes
Qt runtime            Bibliotecas/plugins de Qt para la build de Qt seleccionada
HDF5/TBB/Blosc/etc.   Bibliotecas compartidas de runtime requeridas por ldd
resources             Recursos, scripts, y configuración de Geomec
license files         Archivos de licencia de Diana/Open Inventor/licenciamiento según aplique
```

Después de compilar, inspeccionar las dependencias con:

```sh
ldd path/to/geomec_shell
ldd path/to/gm_diana_app
```

Configurar `LD_LIBRARY_PATH` para la disposición desplegada, o instalar las bibliotecas compartidas en una ubicación estándar del cargador y añadir un rpath apropiado. Un despliegue no debería depender de las rutas `/glb/data` del desarrollador.

## 6. Pruebas y Comprobaciones Básicas

El repositorio incluye proyectos de pruebas unitarias e integración bajo `Tests/`, más archivos de modelos grandes bajo `TestData/`.

Para una prueba básica (smoke test) en Windows, las instrucciones históricas recomiendan compilar x64 y ejecutar la aplicación con `TestData\\TestLoadAndCalculate.gm4`, y luego probar un pequeño análisis no lineal.

Para Linux, compilar los targets de prueba relevantes desde el grafo de qmake una vez que las bibliotecas estén disponibles. No asumir que toda prueba es portable: algunas pruebas dependen de Diana, licenciamiento, archivos de modelo externos, Python, o bibliotecas de runtime específicas de plataforma.

## 7. Orden de Build Recomendado

Para cualquiera de las dos plataformas, usar este orden al diagnosticar fallos:

1. Verificar el compilador, Qt, Diana, Open Inventor, licenciamiento, y el entorno de runtime.
2. Compilar las bibliotecas de terceros y utilidades de bajo nivel.
3. Compilar las bibliotecas de FW51.
4. Compilar las bibliotecas de dominio de Geomec y de integración con Diana.
5. Compilar el ejecutable seleccionado (`Geomec`, `geomec_shell`, o una app especializada).
6. Compilar/ejecutar el target de prueba relevante más pequeño.
7. Ensamblar los archivos de despliegue y probar en una máquina o contenedor limpio.

## 8. Archivos para Leer Primero

- Grafo principal de qmake: `geomec.pro`
- Prerrequisitos de Linux: `README.linux`
- Prerrequisitos de Windows y configuración de runtime: `geomec/INSTALL.txt`
- Solución de Windows: `Geomec.sln`
- Proyecto principal de Windows: `geomec/Geomec.vcxproj`
- Macros de entorno de Windows: `geomec/Environment.props` y `vsprops/BuildCommon.props`
- Lista de dependencias de qmake: `qmake/libs.pri`
- Rutas de qmake y directorios de salida: `qmake/globals.pri`, `qmake/basepath.pri`, y `qmake/destination.pri`
- Proyectos de instalador: `wix/`

## 9. Limitaciones Actuales del Repositorio

Este árbol de código fuente es un código histórico multiplataforma más que un entorno de build limpio y reproducible. La solución de Windows y los proyectos WiX ofrecen el camino de producción más claro. El camino de qmake de Linux es útil como referencia de build, pero requiere adaptar rutas específicas de un sitio y hacer coincidir cuidadosamente versiones antiguas de dependencias. Una mejora de mantenimiento futura sería añadir manifiestos de dependencias explícitos por plataforma, eliminar rutas de infraestructura fijas, y proveer un script de empaquetado de Linux reproducible.
