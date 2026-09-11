# Poniendo en Marcha un Toolchain Moderno

Una referencia para levantar un build usando un **Visual Studio actual** y
**licencias de prueba** para las dependencias comerciales, dirigida
específicamente al objetivo discutido con el usuario: hacer funcionar la
**visualización** 3D de modelos (Open Inventor) sin necesidad de
realmente **ejecutar/resolver** un modelo (Diana). Esto complementa, y no
reemplaza, `BUILD_AND_ARCHITECTURE.md` (que documenta el camino original
VS2012/OIV 9.3.1 tal como el repositorio lo espera hoy).

**Estado: referencia de planificación, aún no intentado.** Todo lo de
abajo proviene de documentación de proveedores a septiembre de 2026 (ver
Fuentes al final) más lo que ya dicen los propios documentos de este
repositorio — no se ha verificado con un build real.

## Tabla de contenidos

1. [La pregunta central: ¿es Diana realmente evitable?](#1-la-pregunta-central-es-diana-realmente-evitable)
2. [Open Inventor: Visual Studio vs. Qt — una falsa disyuntiva](#2-open-inventor-visual-studio-vs-qt--una-falsa-disyuntiva)
3. [Matriz de dependencias de versiones](#3-matriz-de-dependencias-de-versiones)
4. [Licencias de prueba](#4-licencias-de-prueba)
5. [Artefactos de licencia existentes encontrados en este repositorio](#5-artefactos-de-licencia-existentes-encontrados-en-este-repositorio)
6. [Usar un Visual Studio actual en vez de VS2012](#6-usar-un-visual-studio-actual-en-vez-de-vs2012)
7. [Camino recomendado](#7-camino-recomendado)
8. [Comprobación de preparación del entorno actual](#8-comprobacion-de-preparacion-del-entorno-actual)
9. [Riesgos abiertos / incógnitas](#9-riesgos-abiertos--incognitas)
10. [Fuentes](#10-fuentes)

## 1. La pregunta central: ¿es Diana realmente evitable?

**Parcialmente, no del todo.** "No voy a ejecutar un modelo" reduce
cuánta *licencia* de Diana se necesita, pero probablemente no elimina a
Diana del **build**. Según `BUILD_AND_ARCHITECTURE.md`, la app enlaza
directamente contra las bibliotecas del SDK de Diana en tiempo de
compilación/enlazado — `lib/DianaCore`, `DianaGlue`, `DianaSignals`,
`DianaStartup` envuelven todos cabeceras/bibliotecas de importación de
Diana, y `DIAROOT` (que contiene `release`/`debug`/`share`) es un
prerrequisito de build obligatorio, no solo de tiempo de ejecución. Así
que:

- Para **compilar** `Geomec.exe` tal cual, casi con certeza sigues
  necesitando el **SDK** de Diana (cabeceras + bibliotecas de
  importación) desde `DIAROOT`.
- Para **ejecutar un análisis real**, necesitarías una licencia completa
  de **runtime** de Diana — esta es la parte que "no ejecutar un modelo"
  sí te permite genuinamente evitar.

**Acción:** pregunta específicamente a ventas de Diana FEA por una
evaluación solo de desarrollo/SDK (compilar contra la API, sin licencia
de resolución en producción) — es una distinción común que hacen los
proveedores pero no está documentada públicamente; se confirma
preguntando, no asumiendo.

## 2. Open Inventor: Visual Studio vs. Qt — una falsa disyuntiva

No son alternativas — Open Inventor necesita **ambos**, en capas
distintas:

- **Visual Studio / toolset de MSVC** = el compilador con el que se
  compilaron las propias bibliotecas precompiladas de Open Inventor. Los
  SDKs de C++ de proveedores en Windows suelen ser sensibles a la ABI
  según la versión del compilador; generalmente hay que compilar el
  propio código con un toolset compatible para enlazar contra ellos sin
  problemas.
- **Qt** = el toolkit de GUI/ventanas en el que Open Inventor renderiza.
  Open Inventor distribuye un componente dedicado (`SoQt`) específicamente
  para incrustar su área de render 3D dentro de una aplicación Qt — el
  módulo `geomec/OpenInventor` de este repositorio es exactamente esa
  integración. Sin Qt (u otro toolkit de ventanas soportado), Open
  Inventor no tiene dónde dibujar.

Así que el requisito real es: **una versión de Visual Studio compatible
con la build de Open Inventor que consigas, *y* una versión de Qt
compatible con ambos.**

## 3. Matriz de dependencias de versiones

| Componente | Qué espera este repositorio hoy | Qué está confirmado sobre las versiones *actuales* |
| --- | --- | --- |
| Visual Studio / toolset de MSVC | `geomec/INSTALL.txt` dice VS2012 Premium Update 4; los archivos `.vcxproj` en realidad usan el toolset `v140` (≈ VS2015) — **los propios documentos del repositorio ya señalan esto como una inconsistencia** | VS2015 a VS2022 comparten una ABI de runtime de C++/STL binariamente compatible, así que recambiar el toolset en `.vcxproj` (v140 → v143 para VS2022) suele ser de bajo riesgo para *nuestro propio* código |
| Qt | `.vcxproj` enlaza `Qt5Core`/`Qt5Widgets`/`Qt5Xml`; `INSTALL.txt` describe por separado compilar Qt 4.8.5 — **también ya señalado como inconsistente** en `BUILD_AND_ARCHITECTURE.md` | La documentación actual de Open Inventor (era 2023.2) referencia Qt 5.9 para sus propias demos; cada build de Qt está ligada a una versión específica de MSVC, así que la versión de Qt y la de VS deben elegirse juntas |
| Open Inventor | Los documentos especifican 9.3.1 para VS x64 (circa ~2013) | Una versión actual de Open Inventor (2023.2 o posterior) requiere en cambio un runtime de C++ compatible con VS2017 — es decir, una prueba *actual* te da un SDK mucho más nuevo que aquel contra el que se escribió este código |
| Qwt | Versión sin especificar, plotting "para Windows" | Debe compilarse/descargarse a juego con la build de Qt que se elija |
| Diana | Versión sin especificar; distribuye su propio árbol `release`/`debug`/`share`, mayormente precompilado | La compatibilidad de compilador/toolset no está documentada públicamente — preguntar directamente al soporte de Diana |
| TBB, HDF5, zlib, Blosc (c-blosc), Google Test/Mock | Versiones antiguas fijadas ya vendorizadas en `third_party/` (ver `THIRD_PARTY_DEPENDENCIES.md`) | Todas son C/C++ portables y de código abierto activamente mantenido — deberían recompilar sin problemas contra un toolset MSVC actual; la parte segura de todo este rompecabezas |

**La consecuencia práctica:** conseguir una prueba *actual* de Open
Inventor no simplemente "encaja" en este código de época ~2013. El código
de integración de `geomec/OpenInventor` se escribió contra la API de
Open Inventor 9.3.1, y hay más de una década de evolución de la API entre
eso y una versión 2023.2+ — espera trabajo real de migración, no solo un
cambio de ruta/variable de entorno.

## 4. Licencias de prueba

Ambas dependencias comerciales sí ofrecen acceso de evaluación, mediado
por un formulario de ventas/solicitud en lugar de una descarga instantánea
de autoservicio:

- **Open Inventor** — Thermo Fisher Scientific (proveedor actual, vía el
  linaje VSG → FEI → Thermo Fisher) gestiona un formulario de solicitud
  de prueba. No publica públicamente la duración/alcance de la prueba;
  eso se revela después de enviar el formulario y ser contactado.
- **Diana** — DIANA FEA BV tiene un flujo de solicitud de prueba "Try
  DIANA" y distingue por separado categorías de licencia Comercial /
  Académica / Investigación. Su equipo de ventas ofrece explícitamente
  adaptar una propuesta a necesidades específicas — vale la pena
  preguntar explícitamente por una evaluación solo de build/SDK según
  §1.

Vale la pena perseguir ambas en paralelo dado el tiempo de respuesta
mediado por ventas — iniciar las conversaciones de solicitud de prueba
antes de invertir tiempo en recambiar el toolchain.

## 5. Artefactos de licencia existentes encontrados en este repositorio

Esto confirma, a partir del contenido real del repositorio, cómo se
autentica cada sistema — y señala un problema real de sensibilidad de
datos.

**Diana — archivo de clave local, sin necesidad de red.** Hay tres
archivos de clave de licencia incluidos en el repositorio, todos
vencidos, todos con el mismo cliente real:

| Archivo | Vence | Cliente | Módulos |
| --- | --- | --- | --- |
| `geomec/keyfile/Linux/diafgv.k72` | 31/3/2020 | Shell | Geomec |
| `geomec/keyfile/Linux/diafgv.k72.2019_03_31` | 31/3/2019 | Shell | Geomec |
| `archive/old/DSealect/keyfile/diafgv.k72` | 28/2/2014 | Shell Research bv KSEPL | Linear Static and Eigenvalue Analysis |

El esquema de Diana (al menos tal como está configurado aquí) es
"presentar un archivo de clave local válido" — no requiere contacto con
servidor. Los tres están vencidos hace tiempo y no sirven como licencias
ahora.

**Open Inventor — servidor de licencias por red, ni siquiera es una
clave incrustada.** `wix/WixInclude/password.dat` contiene:

```text
SERVER muntse-s-09786.europe.shell.com ANY 3015
USE_SERVER
```

Un puntero de estilo FlexLM que le dice al cliente de OIV que se conecte
(TCP, no literalmente HTTP, pero la misma idea — un intercambio con un
servidor de licencias por red) a un servidor interno de Shell. No hay
material de clave aquí, y es inalcanzable fuera de la red de Shell.
**Cualquier prueba de OIV que obtengas necesitará su propio
`password.dat`** apuntando a donde realmente viva tu licencia — un
servidor alojado por el proveedor, tu propio servidor local, o una
licencia basada en dongle, según lo que Thermo Fisher provisione.

**Aviso de sensibilidad:** estos son artefactos de licencia corporativa
reales (aunque vencidos) que nombran a una empresa real (Shell) y un
nombre de host interno, y están en un repositorio ya subido a un remoto
personal de GitHub. Borrar los archivos del árbol de trabajo no los
eliminaría del historial de git — eso necesitaría una operación separada
y más cuidadosa de reescritura de historial si alguna vez se desea. No
se tocó nada como parte de este documento.

## 6. Usar un Visual Studio actual en vez de VS2012

Dos preocupaciones separadas:

1. **Recambiar los propios archivos `.vcxproj` de este repositorio** del
   toolset `v140` a uno actual (p. ej. `v143` para VS2022) — mecánicamente
   sencillo (el propio flujo "Retarget Solution" de VS), y de bajo riesgo
   para el código propio dada la ABI compartida entre VS2015 y VS2022.
2. **Cualquier SDK de Open Inventor/Diana que realmente obtengas debe
   soportar ese toolset.** Esta es la barrera real: el SDK *antiguo* de
   Open Inventor 9.3.1 casi con certeza se compiló para un toolset de la
   era VS2012/2013, que puede no enlazar limpiamente contra código
   compilado con VS2022 (mezclar tipos de STL a través de un límite de
   DLL compilado por generaciones de toolset muy distintas es el modo de
   fallo clásico). Una prueba *actual* de Open Inventor evita esta
   cuestión de ABI pero reabre la cuestión de migración de API del §3.

## 7. Camino recomendado

Dado que el objetivo es solo visualización:

1. Solicitar la prueba de Open Inventor ahora (el plazo más largo,
   mediado por ventas).
2. En paralelo, preguntar al soporte de Diana si es posible una
   evaluación solo de build/SDK (sin derechos de resolución), ya que no
   se ejecutará ningún modelo realmente.
3. Decidir la versión de Qt según la versión de Open Inventor que
   realmente se te conceda (su documentación indicará una versión de Qt
   requerida/probada).
4. Recambiar el toolset de `.vcxproj`/`vsprops` de este repositorio para
   que coincida con la versión de Visual Studio que realmente soporte el
   SDK de Open Inventor concedido — lo cual puede significar terminar en
   VS2017 o VS2019 en vez de VS2022, si eso es lo que requiere el SDK de
   prueba. Confirmarlo antes de asumir que "el VS actual" es compatible.
5. Esperar tener que migrar `geomec/OpenInventor` de la API de la era
   9.3.1 a la API actual que se reciba, si se opta por la ruta de "prueba
   actual" en vez de conseguir una licencia archivada compatible con
   9.3.1.

## 8. Comprobación de preparación del entorno actual

Una respuesta directa a "¿puedo compilar todo ahora mismo?" para la
máquina real en la que se trabajó este repositorio en esta sesión (WSL2
Linux) — **no, y no es solo por las licencias.**

**El camino de build de Windows es categóricamente imposible desde aquí,
punto, sin importar las licencias.** Esto es un sandbox Linux/WSL — no
hay Visual Studio, no hay MSBuild, no hay Windows SDK, ni siquiera un
kernel de Windows. Eso no es un problema de paquete faltante, es el
sistema operativo equivocado. **Esto afecta directamente al objetivo de
visualización planteado: `geomec/OpenInventor` no tiene ningún proyecto
qmake `.pro`** — confirmado revisando `geomec.pro` (cero referencias a
OpenInventor) y el propio contenido del módulo (solo conectado a
`geomec/Geomec.vcxproj`, vía `$(OIVHOME)`). La visualización con Open
Inventor en este código es exclusiva de Windows; no hay camino en Linux
hacia ella. **Lograr el objetivo de visualización requiere una máquina
Windows real con Visual Studio — no alcanzable en este entorno sin
importar qué más se instale.**

**El camino de build de Linux (qmake) tampoco está listo — más allá de
las licencias:**

| Dependencia | Estado en esta máquina | Nota |
| --- | --- | --- |
| g++ | ✅ presente (15.2.0) | Mucho más nuevo que cualquier cosa contra la que se escribió este repositorio |
| qmake | ✅ presente (3.1, Qt 5.15.18) | Coincide con el uso de Qt5 del lado Windows — buena señal, pero sin verificar contra los archivos `.pro` reales |
| TBB, HDF5, zlib, Blosc, gtest | ✅ vendorizados en `third_party/` | Ver el aviso de ABI más abajo |
| Python 3 | ✅ presente | |
| Diana | ❌ **no instalado en absoluto** — no es un problema de licencia, el software en sí no está aquí | Bloquea la compilación, no solo la ejecución, ya que `DianaCore`/`DianaGlue` enlazan contra él |
| Boost | ❌ no instalado | `apt install libboost-*-dev` lo resolvería, pero no se ha hecho |
| Cabeceras de desarrollo de GLib | ❌ no instaladas | Igual — un simple `apt install libglib2.0-dev`, simplemente no hecho |
| `uuid-dev` | ❌ no instalado | Referenciado vía `$GM_SW_TOOL_PATH/extra/include/uuid` en la documentación |
| Open Inventor | ❌ no aplica aquí | No forma parte del grafo de Linux en absoluto (ver arriba) |

**Un riesgo de ABI real y verificado, independiente de todo lo anterior:**
los binarios vendorizados de `third_party/tbb/linux` están precompilados
para `gcc4.1`/`gcc4.4`/`gcc4.7` (visible directamente en los nombres de
sus rutas) — compiladores de aproximadamente 2011–2013. El g++ de esta
máquina es la versión 15 (2025). Esa es una brecha de generación enorme,
que atraviesa una ruptura de ABI de C++ conocida (la ABI dual de
libstdc++, introducida en GCC 5). Incluso después de instalar
Boost/GLib/uuid-dev y de alguna manera obtener Diana, enlazar objetos
recién compilados con GCC 15 contra estos binarios de TBB precompilados
tan antiguos es un riesgo real de fallo — no está garantizado que falle,
pero tampoco es una suposición segura. Habría que probarlo directamente,
o reemplazar el TBB precompilado por una versión compilada contra el GCC
actual.

`qmake/basepath.pri` también fija una ruta específica de un sitio
(`/glb/data/...`) para GLib que habría que reemplazar por la ruta válida
en esta máquina, tal como ya se señala en `BUILD_AND_ARCHITECTURE.md`
§5.3.

**Conclusión:** la cuestión de licencias es real, pero no es la única
brecha. Ahora mismo, ninguno de los dos caminos de build compila en este
entorno — Windows no se puede ni intentar aquí, y Linux carece de Diana
por completo (no solo de su licencia), carece de Boost/GLib/uuid-dev, y
además carga con un riesgo de ABI TBB/GCC sin verificar.

## 9. Riesgos abiertos / incógnitas

- Si la prueba de Open Inventor puede acotarse a una versión de SDK *más
  antigua* (que coincida con el código de integración 9.3.1 existente en
  este repositorio) en vez de la versión actual — sin confirmar,
  preguntar directamente a ventas.
- Compatibilidad exacta de compilador/toolset de Diana — no documentada
  públicamente.
- Si Diana de verdad no se puede excluir del paso de enlazado en
  absoluto (se necesitaría una auditoría desde cero de los puntos de uso
  de `DianaCore`/`DianaGlue`/`DianaSignals` para saber si un reemplazo
  simulado/stub es viable en vez del SDK real — no intentado aquí).

## 10. Fuentes

- [Open Inventor Software trial request — Thermo Fisher Scientific](https://www.thermofisher.com/us/en/reference-components/MSD-reference-components/instruments-reference-components/open-inventor-software-trial.html)
- [Licensing – Activate Open Inventor Licenses – Open Inventor 9 Developer Zone](https://developer9.openinventor.com/index.php/general-documentation/licensing-activate-open-inventor-licenses/)
- [Open Inventor: Platforms of Open Inventor 2023.2](https://developer.openinventor.com/refmans/latest/RefManCpp/_inventor_platforms_2023_2.html)
- [System Requirements – Open Inventor 9.9 – Open Inventor 9 Developer Zone](https://developer9.openinventor.com/index.php/system-requirements-open-inventor-9-9/)
- [SoQt Class Reference [Qt]](https://www.openinventor.com/reference-manuals/NewRefMan1030/RefManCpp/class_so_qt.html)
- [Licenses & Modules - DIANA FEA](https://dianafea.com/licenses-modules/)
- [DIANA FEA](https://en.wikipedia.org/wiki/DIANA_FEA)
