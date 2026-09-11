# Estructura del Repositorio

Para qué sirve cada carpeta de nivel superior, y si contiene código/contenido
que mantenemos nosotros ("Nuestro") o material de terceros que no
mantenemos ("Third-party"). Ver `BUILD_AND_ARCHITECTURE.md` para detalles
de compilación y `FOLDER_STRUCTURE_PROPOSAL.md` para cómo se llegó a esta
estructura.

## Tabla de contenidos

1. [Tabla de carpetas](#1-tabla-de-carpetas)
2. [Notas](#2-notas)

## 1. Tabla de carpetas

| Carpeta | Propiedad | Qué es |
| --- | --- | --- |
| `geomec/` | Nuestro | Aplicación de escritorio principal de Geomec para Windows y código de GUI (`Geomec.vcxproj`). |
| `fw51/` | Nuestro | Framework 51 — biblioteca compartida de geometría, malla, tensores y model-frame usada en toda la app. |
| `lib/` | Nuestro | Bibliotecas propias compartidas por la app principal y otros ejecutables, que compilan tanto en Windows como en Linux (p. ej. `GeomecModel`, `DianaCore`, `MaterialLibrary`, `Quantity`). |
| `platform/` | Nuestro | Bibliotecas propias sin proyecto Linux/qmake hoy, es decir, solo Windows (basadas en MFC: `MfcGuiFrame`/`MfcModelFrame`, `WindowsRegistry`, `qtwinmigrate`, `GmDatabase`, etc.). |
| `app/` | Nuestro | Aplicaciones auxiliares y proyectos de aplicaciones heredadas (`geomec_launcher`, `diana_app`, `geomec_rgi`, `geomec_cora`, `MPKernelApp`, y otras). |
| `geomec_shell/` | Nuestro | Variante ejecutable de consola/shell de Geomec. |
| `gm_diana_app/` | Nuestro | Ejecutable de aplicación orientada a Diana. |
| `geomec_cora_wrapper/` | Nuestro | Wrapper de integración con CORA. |
| `geomec_selector/` | Nuestro | Aplicación selectora. |
| `Tests/` | Nuestro | Código fuente de pruebas en C++ (`TestLib`, `TestRunner`, `Fw51`, `Materials`, `ModelOperations`, `SanityChecks`, `app`). |
| `TestData/` | Nuestro | Datos de prueba — archivos de modelos/entrada que cargan las pruebas (no código fuente). |
| `IntegrationTests/` | Nuestro | Pruebas de integración basadas en scripts (shell/Python). |
| `Prototypes/` | Nuestro | Trabajo experimental/inconcluso (`ModularGeomec`). Sigue conectado al build de qmake mediante un target `SUBDIRS`. |
| `archive/` | Nuestro (inactivo) | Código histórico que ya no forma parte de ningún target de build (la aplicación heredada `DSealect`). Se conserva solo como referencia. |
| `plugins/` | Nuestro | Paquetes/instantáneas de plugins distribuibles (p. ej. versiones comprimidas de Inversion Tool) y notas relacionadas. |
| `py/` | Nuestro | Herramientas Python y ayudantes de integración (`InversionTool`, `DependenciesParser`, `autobuild`). |
| `img/` | Nuestro | Íconos e imágenes usados en la interfaz de la aplicación. |
| `version/` | Nuestro | Cabecera de versión y metadatos de versión. |
| `changelog/` | Nuestro | Páginas de novedades por versión (HTML + capturas) mostradas a los usuarios finales. |
| `res/` | Nuestro | Scripts de build, control de versiones y utilidades (`autobuild`, `setenv`, `svn-tools` antiguo). |
| `qmake/` | Nuestro | Configuración de qmake compartida y definiciones de dependencias para el grafo de build de Linux. |
| `vsprops/` | Nuestro | Hojas de propiedades de Visual Studio (configuración MSBuild compartida) para el build de Windows. |
| `wix/` | Nuestro | Proyectos de instalador de Windows (WiX). Los archivos de proyecto `.wxs`/`.wixproj` son nuestros; el *toolset* WiX en sí es una herramienta de terceros invocada en tiempo de build, no está vendorizada aquí. |
| `md/` | Nuestro | Documentación a nivel de repositorio (este archivo, `BUILD_AND_ARCHITECTURE.md`, `FOLDER_STRUCTURE_PROPOSAL.md`, `ACTIONS_LOG.md`). |
| `third_party/` | **Third-party** | SDKs e instaladores de terceros que no mantenemos: TBB, HDF5, zlib, Blosc (`c-blosc-1.9.0`), Google Test/Mock, además de CORA, SVS, redistribuible de .NET, y assets de bitmaps. |

## 2. Notas

- `blosc_filter/` y `qtiocompressor/` viven en `lib/` (nuestro), no en
  `third_party/` — son pequeño código propio de wrapper/integración sobre
  formatos de terceros (Blosc, Qt), no son en sí código de biblioteca
  vendorizada.
- Los archivos de nivel raíz (`geomec.pro`, `Geomec.sln`, `app.pro`,
  `README.linux`, `.editorconfig`, `.clang-format`, `Geomec.code-workspace`,
  etc.) son todos nuestros — configuración de proyecto/build, no
  vendorizados.
