# Clasificación automatizada de cobertura superficial mediante imágenes adquiridas por drones

Proyecto desarrollado en el marco de un **Trabajo Final de Carrera**, orientado al procesamiento y clasificación automatizada de imágenes adquiridas mediante drones para estimar la cobertura superficial de un terreno.

El objetivo final del sistema es identificar y cuantificar tres clases principales:

- **Vegetación viva**
- **Vegetación muerta**
- **Suelo desnudo**

El proyecto se desarrolla en el contexto de una actividad de vinculación tecnológica entre el **Departamento de Ciencias e Ingeniería de la Computación (DCIC) de la Universidad Nacional del Sur** y el **Instituto Nacional de Tecnología Agropecuaria (INTA)**.

## Objetivo

Desarrollar un sistema capaz de procesar imágenes aéreas de alta resolución y obtener automáticamente el porcentaje de superficie correspondiente a vegetación viva, vegetación muerta y suelo desnudo.

Durante el desarrollo se estudiará el uso de imágenes **RGB** y, posteriormente, la incorporación de información **multiespectral** proveniente de las bandas:

- Green
- Red
- Red Edge
- NIR

También se evaluará el aporte de índices espectrales como **NDVI** y **NDRE**, así como diferentes técnicas de clasificación.

## Tecnologías

Actualmente el prototipo utiliza:

- **C++**
- **OpenCV**
- **Dear ImGui**
- **GLFW**
- **OpenGL**
- **Visual Studio**

## Estado actual

El proyecto se encuentra en una etapa inicial de prototipado orientada a comprender y validar el manejo de las imágenes proporcionadas por el dron.

La versión actual permite:

- Seleccionar imágenes mediante una interfaz gráfica.
- Cargar archivos `.TIF`, `.TIFF`, `.JPG` y `.PNG`.
- Mantener la profundidad original de las imágenes mediante OpenCV.
- Obtener resolución, cantidad de canales y profundidad de bits.
- Leer los valores de un píxel de la imagen.
- Obtener los valores mínimo y máximo de imágenes monocanal.
- Visualizar imágenes RGB y bandas multiespectrales dentro de la aplicación.
- Visualizar correctamente imágenes de 16 bits mediante normalización para su representación en pantalla.

Las pruebas iniciales se están realizando sobre imágenes adquiridas mediante un **DJI Mavic 3 Multispectral**.

## Próximas etapas

El desarrollo continuará incorporando progresivamente:

1. Procesamiento y análisis de imágenes RGB.
2. Generación de histogramas y extracción de características.
3. Desarrollo de una primera clasificación de vegetación viva, vegetación muerta y suelo desnudo.
4. Lectura simultánea de bandas multiespectrales.
5. Cálculo de índices como NDVI y NDRE.
6. Comparación entre clasificación RGB y clasificación multiespectral.
7. Evaluación de métodos de clasificación automática.
8. Procesamiento eficiente de ortomosaicos de gran tamaño.
9. Integración con una plataforma web para almacenamiento y visualización de resultados.

## Datos

Las imágenes originales utilizadas durante el desarrollo no se incluyen en este repositorio debido a su tamaño y a que corresponden a datos provistos para el proyecto.

El repositorio contiene únicamente el código fuente y los recursos necesarios para el desarrollo del sistema.

## Estructura del proyecto

```text
clasificacion-cobertura-INTA/
│
├── Suelo_INTA_Pilot/
│   ├── Suelo_INTA_Pilot.cpp
│   ├── Suelo_INTA_Pilot.vcxproj
│   └── Suelo_INTA_Pilot.vcxproj.filters
│
├── .gitattributes
├── .gitignore
├── README.md
└── Suelo_INTA_Pilot.slnx
```

La estructura podrá modificarse a medida que avance el desarrollo.

## Autor

Valentina Navarro - Trabajo Final de Carrera – Universidad Nacional del Sur  
Departamento de Ciencias e Ingeniería de la Computación
