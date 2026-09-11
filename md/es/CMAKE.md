# Migrar a CMake — Notas de Viabilidad

Notas de referencia sobre si/cuándo migrar los dos grafos de build de este
repositorio (qmake `.pro`/`.pri` y MSBuild `.vcxproj`/`.sln`/`.props`) a
CMake, el generador de sistemas de build estándar de facto. Esto es una
opinión sobre viabilidad, no un plan — no se ha migrado nada.

## Tabla de contenidos

1. [¿Es técnicamente posible?](#1-es-tecnicamente-posible)
2. [El riesgo principal: migrar un build que no se puede validar](#2-el-riesgo-principal-migrar-un-build-que-no-se-puede-validar)
3. [Esfuerzo comparado con la reestructuración de carpetas](#3-esfuerzo-comparado-con-la-reestructuracion-de-carpetas)
4. [Recomendación](#4-recomendacion)

## 1. ¿Es técnicamente posible?

Sí. CMake puede expresar todo lo que hacen ambos grafos de build actuales:

- MFC (usado por `platform/MfcGuiFrame`, `platform/MfcModelFrame`, etc.)
  mediante `CMAKE_MFC_FLAG`.
- Qt (usado en toda la app) mediante `find_package(Qt5 ...)` más
  `AUTOMOC`/`AUTOUIC` — la integración de Qt en CMake es madura y muy
  probada.
- Diana, Open Inventor y Qwt necesitarían cada uno un módulo
  `Find<X>.cmake` escrito a mano, ya que ninguno distribuye una
  configuración oficial de CMake — las variables de entorno de estilo
  `$(LBROOT)`/`$(OIVHOME)`/`$(QTDIR)` que ya existen en este repo se
  corresponden bastante directamente con lo que buscaría tal módulo.

## 2. El riesgo principal: migrar un build que no se puede validar

Según `GETTING_STARTED_MODERN_TOOLCHAIN.md` §8, ninguno de los dos caminos
de build actuales compila hoy en el entorno donde se hizo este trabajo —
Windows no se puede ni intentar (no hay Visual Studio aquí), y el camino
Linux/qmake carece de Diana por completo además de tener un riesgo de ABI
TBB/GCC sin verificar.

Una migración de sistema de build solo se sabe *correcta* compilando el
resultado y comparando el comportamiento contra el original. Sin una base
de referencia que funcione para comparar, migrar el equivalente a ~100
archivos de proyecto en rutas de include, defines de preprocesador y orden
de enlazado se haría a ciegas, sin forma de detectar un error hasta mucho
después (o nunca, si cambia silenciosamente qué se compila).

## 3. Esfuerzo comparado con la reestructuración de carpetas

Considerablemente mayor, y un tipo de trabajo distinto:

- La reestructuración de carpetas hecha antes en este proyecto fue
  mecánica — mover un directorio, corregir una cadena de ruta, verificar
  con grep buscando la ruta antigua. Poca ambigüedad, barato de comprobar.
- Una migración a CMake implica re-derivar la semántica real de
  compilación/enlazado por módulo — no solo dónde vive un archivo, sino
  exactamente con qué se compila.
- Los dos grafos de build actuales ya **están en desacuerdo entre sí** en
  puntos ya señalados en `BUILD_AND_ARCHITECTURE.md`: Qt4 (según
  `geomec/INSTALL.txt`) frente a Qt5 (según `Geomec.vcxproj`), y VS2012
  (según `INSTALL.txt`) frente al toolset `v140` realmente usado. CMake
  forzaría una decisión en cada una de esas ambigüedades en lugar de
  dejarlas latentes — útil a largo plazo, pero trabajo real, no traducción
  mecánica.

## 4. Recomendación

Primero, lograr que el build de Windows compile de verdad — es el camino
más completo de los dos existentes según `BUILD_AND_ARCHITECTURE.md`.
Migrar a CMake *después* de eso, con una base de referencia funcional
contra la cual validar cada pieza migrada, en lugar de intentar la
migración primero y esperar que sea equivalente.
