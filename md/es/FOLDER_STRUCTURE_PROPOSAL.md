# Propuesta de Estructura de Carpetas

Este documento evalúa la estructura del repositorio que existía antes de
esta reestructuración (tal como se describe en el `BUILD_AND_ARCHITECTURE.md`
original) y registra qué se cambió realmente. Las cuatro fases descritas
abajo ya se aplicaron al árbol de trabajo — ver "Estado" al final para qué
significa eso y qué no significa.

## Tabla de contenidos

1. [¿Estaba bien la estructura original?](#1-estaba-bien-la-estructura-original)
2. [Estructura tal como se implementó](#2-estructura-tal-como-se-implemento)
   - 2.1 [Desviaciones respecto al diagrama original](#21-desviaciones-respecto-al-diagrama-original)
3. [Qué se hizo](#3-que-se-hizo)
4. [Estado](#4-estado)

## 1. ¿Estaba bien la estructura original?

No estaba mal, pero tenía problemas reales y medibles:

- **`lib/` mezcla código propio con binarios vendorizados de escalas muy
  distintas.** Contiene ~50 módulos propios (en su mayoría decenas–cientos
  de KB cada uno, ~30MB en total) junto a SDKs de terceros vendorizados:
  `lib/tbb` (**330MB**), `lib/hdf5` (**130MB**), `lib/gmock-1.7.0` (10MB),
  `lib/zlib-1.2.6`, `lib/c-blosc-1.9.0`, `lib/pstl`. Nada en el nombre del
  directorio distingue cuál es cuál.
- **`Tests/` pesa 423MB, y 421MB de eso es `Tests/Models`** (datos
  binarios de modelos/entrada). El código fuente de pruebas real
  (`TestLib`, `Tests`, `TestRunner`, `Fw51`, `Materials`,
  `ModelOperations`, `SanityChecks`) suma bastante menos de 1MB. También
  existe un *segundo* árbol con nombre distinto, `test/` (minúsculas),
  para pruebas de integración basadas en scripts — que `Tests/` y `test/`
  difieran solo en mayúsculas es un riesgo real en checkouts de
  Windows/macOS que no distinguen mayúsculas de minúsculas.
- **Código heredado/muerto vive dentro del árbol activo**: ~20
  subcarpetas `old/` anidadas dentro de módulos activos (`geomec/*/old`,
  `lib/*/old`), más un `old/` raíz que contiene toda una aplicación
  hermana histórica (`DSealect`), más `Prototypes/` para un esfuerzo
  abandonado. Nada de esto está separado de lo que realmente se compila
  hoy.
- Efecto neto: de los 1.7GB del repositorio, **~475MB son SDKs
  vendorizados y ~421MB son datos de prueba** — más de la mitad del árbol
  no es código fuente en absoluto, pero está al mismo nivel que el
  código fuente.

**Buena noticia que reduce el riesgo de arreglar esto:** los grafos de
build de Windows (`vsprops/BuildCommon.props`, `$(LBROOT)`) y de Linux
(`qmake/*.pri`, `$$LBROOT`/`$$DLLROOT`) resuelven las rutas mediante un
pequeño número de variables raíz definidas centralmente, no mediante
rutas relativas fijas repartidas en cada archivo de proyecto. Una
reorganización de nivel superior toca principalmente un puñado de
archivos centrales, no los más de 100 archivos `.vcxproj`/`.pro`.

**Sobre la separación Windows/Linux en concreto**: la mayoría de los
módulos `lib/*` que compilan en ambas plataformas comparten un único
árbol `h/`+`src/`, referenciado tanto por un `.pro` como por un
`.vcxproj` — código fuente genuinamente compartido y multiplataforma.
Separarlo por sistema operativo crearía una duplicación innecesaria. Solo
un subconjunto real ya es exclusivo de Windows (basado en MFC:
`MfcGuiFrame`, `MfcModelFrame`, `WindowsRegistry`, `qtwinmigrate`,
`GmDatabase`, etc.) — simplemente no están etiquetados como tal hoy.

## 2. Estructura tal como se implementó

```text
Geomec/
├── lib/                     Bibliotecas propias de Geomec/Diana/FW51 que compilan
│                            en ambas plataformas (ubicación sin cambios — los módulos
│                            vendorizados se movieron HACIA AFUERA en vez de mover los
│                            propios a un nuevo wrapper src/; ver "Desviaciones")
├── platform/                Bibliotecas propias sin proyecto qmake (.pro) hoy,
│                            es decir, solo Windows: CutSurfaceUtil,
│                            DoubleEditLib, FemAppGUI, GlobalMessage, GmDatabase,
│                            IT, MPKernel, MfcGuiFrame, MfcModelFrame,
│                            OpenInventorDataInterface, Plugins, PyLib,
│                            WellSchemeUtils, WindowsRegistry, XMLLib, qtwinmigrate
├── third_party/             SDKs e instaladores vendorizados, antes repartidos entre
│                            lib/ y el 3rdParty/ raíz: tbb, hdf5, gmock-1.7.0,
│                            zlib-1.2.6, c-blosc-1.9.0, pstl, CORA1, SVS, dotNet,
│                            point_elementsets_bitmaps
├── Tests/                   Solo código fuente de pruebas en C++ (TestLib, Tests, TestRunner,
│                            Fw51, Materials, ModelOperations, SanityChecks, app)
├── TestData/                Los 421MB de datos binarios de modelos/entrada, antes
│                            Tests/Models
├── IntegrationTests/        Pruebas de integración basadas en scripts, antes el
│                            test/ en minúsculas (renombrado para eliminar la
│                            colisión de mayúsculas/minúsculas con Tests/)
├── archive/
│   └── old/                 El directorio old/ raíz (la app histórica DSealect),
│                            que no tenía ninguna referencia desde el build activo
├── geomec/, fw51/, app/, geomec_shell/, gm_diana_app/,
│   geomec_cora_wrapper/, geomec_selector/, Prototypes/, wix/, qmake/,
│   vsprops/, py/, res/, version/, changelog/, img/, plugins/    sin cambios
```

**Justificación:**
- `lib/` frente a `third_party/` es la separación de mayor valor y menor
  ambigüedad — separa ~25MB de código realmente mantenido aquí de ~570MB
  de SDKs que no lo son.
- `Tests/` frente a `TestData/` hace lo mismo para el árbol `Tests/` de
  423MB, y renombrar `test/` a `IntegrationTests/` elimina la arriesgada
  colisión de nombres que solo difería en mayúsculas/minúsculas con
  `Tests/`.
- Sin duplicación de código fuente Windows/Linux — solo se movieron los
  módulos genuinamente exclusivos de Windows, a una carpeta `platform/`
  claramente etiquetada; todo lo multiplataforma sigue siendo un único
  árbol.
- `archive/` saca del árbol el material heredado claramente muerto.

### 2.1 Desviaciones respecto al diagrama original

La estructura realmente implementada difiere del primer borrador de este
documento en algunas formas deliberadas, descubiertas al hacer el
trabajo:

- **Sin wrapper `src/`.** El diagrama original anidaba `lib/`, `fw51/`,
  `geomec/`, etc. bajo un nuevo `src/` de nivel superior. Eso se
  descartó: tanto el grafo de qmake (`qmake/globals.pri`) como las
  propiedades de Windows (`vsprops/Environment.props`) calculan las rutas
  a `lib/` mediante variables raíz (`LBROOT`, `$(LBROOT)`) con atajos
  relativos a la profundidad para algunos contextos de build (p. ej.
  `equals(TYPE,'lib'): LBROOT=..`). Añadir un nivel más de anidamiento
  habría cambiado el número de saltos que esos atajos asumen, por una
  ganancia puramente cosmética. El material vendorizado y exclusivo de
  Windows se movió *hacia afuera* de `lib/` a nuevos hermanos de nivel
  superior (`third_party/`, `platform/`) en su lugar — misma
  profundidad, sin aritmética de rutas relativas que rehacer.
- **`TestData/` e `IntegrationTests/` en vez de `tests/data/` y
  `tests/integration/`.** Anidar los datos de prueba y las pruebas de
  integración bajo un nuevo `tests/` en minúsculas habría puesto a
  `Tests/` y `tests/` uno junto al otro — reintroduciendo exactamente la
  colisión de mayúsculas/minúsculas que este cambio buscaba eliminar.
  Ambos terminaron como hermanos de nivel superior en su lugar.
- **`Prototypes/` se dejó en su sitio**, no se movió a `archive/`. A
  diferencia del `old/` raíz (cero referencias desde el build activo),
  `Prototypes/` es un target `SUBDIRS` de qmake activo (`ModGMBus`,
  conectado desde `geomec.pro`) con una anulación de `ROOT` relativa a la
  profundidad (`contains(SPLIT,'Prototypes'): ROOT = ../../..`) ligada a
  su nivel de anidamiento actual. Moverlo bajo `archive/` habría añadido
  un nivel de directorio, invalidando esa anulación. `Prototypes/`
  también ya es autodescriptivo como experimental, así que la ganancia
  de etiquetado no justificaba el riesgo.
- **No se crearon `build/windows/`, `build/linux/`, `tools/`, `docs/`.**
  Formaban parte del diagrama aspiracional original pero nunca se
  ofrecieron como una de las cuatro fases de implementación y no se
  aplicaron.

## 3. Qué se hizo

Las cuatro fases se implementaron en una sola pasada:

1. Dividir `lib/` en `lib/` (propio) y `third_party/` (SDKs
   vendorizados), incluyendo fusionar el antiguo `3rdParty/` raíz en
   `third_party/`.
2. Dividir `Tests/` en `Tests/` (código fuente) y `TestData/` (datos), y
   renombrar `test/` a `IntegrationTests/`.
3. Mover el `old/` raíz a `archive/old/`.
4. Extraer `platform/` de `lib/` para los 15 módulos que hoy no tienen
   proyecto qmake.

Para cada una, se actualizó cada variable raíz de estilo
`$(LBROOT)`/`$$LBROOT`, ruta relativa fija, entrada de proyecto en
`.sln`, referencia de instalador WiX, y ruta en script de shell/Python
que se pudo encontrar mediante búsqueda estática (qmake `.pro`/`.pri`,
MSBuild `.vcxproj`/`.props`, `.sln`, `.wixproj`/`.wxs`/`.wxi`, y scripts
de build/despliegue) para que apuntara a las nuevas ubicaciones. Se
introdujeron nuevas variables raíz junto al patrón existente
`LBROOT`/`TESTSROOT`: `THIRDPARTYROOT`, `TESTDATAROOT`, y
`PLATFORMROOT`, cada una definida una sola vez en `qmake/globals.pri` y
`vsprops/Environment.props`.

Se dejó deliberadamente sin tocar: referencias incrustadas en formatos de
proyecto ya muertos y reemplazados por `.vcxproj` (`.vcproj`, `.dsp`,
`.dsw` — ninguno referenciado por `Geomec.sln`), el árbol histórico
`old/DSealect` (cuyas rutas relativas internas ya eran inconsistentes con
la estructura actual antes de este cambio), una URL de checkout SVN
remota en `geomec/script/get_shell_code.bat` (apunta a la propia
estructura de directorios de un servidor externo, no relacionada con
este renombrado local), y un par de comentarios que referencian rutas
locales absolutas históricas de otras personas (`plugins/TEST`, un
comentario en
`IntegrationTests/geomec_rgi/create_installation.py`).

## 4. Estado

Los movimientos y actualizaciones de referencias descritos arriba se han
aplicado al árbol de trabajo (aún no confirmados con commit). **Esto no
se ha verificado con un build real en ninguna de las dos plataformas** —
el entorno donde se hizo este trabajo no tiene ni el toolchain de Windows
(Visual Studio, Diana, Open Inventor, Qt, WiX) ni un entorno Linux
qmake/Diana completamente configurado, y la configuración existente ya
requiere una configuración de entorno extensa y específica del sitio
para compilar siquiera (ver el `BUILD_AND_ARCHITECTURE.md` principal).
Cada cambio se comprobó mediante búsqueda estática — haciendo grep
repetidamente en todo el árbol buscando las rutas antiguas después de
cada edición — pero eso no puede sustituir una compilación real en ambas
plataformas. **Antes de confiar en esto, compila tanto la solución de
Windows como el grafo de qmake de Linux, y corrige lo que un build real
revele y que la búsqueda estática no haya detectado.**
