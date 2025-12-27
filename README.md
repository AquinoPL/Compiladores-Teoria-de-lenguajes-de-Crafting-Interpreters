# Desarrollo de un Intérprete basado en Crafting Interpreters (C++)

## Descripción
Implementación de un intérprete siguiendo el libro *Crafting Interpreters* de Robert Nystrom, desarrollado en lenguaje C++. El proyecto consiste en la construcción progresiva de un lenguaje de programación propio, abordando las etapas fundamentales de un compilador/intérprete hasta el capítulo 12 del libro, incluyendo análisis léxico, sintáctico y semántico.

## Tecnologías usadas
- Lenguaje: C++
- Herramientas: Visual Studio Code
- Control de versiones: Git, GitHub

## Desarrollo por capítulos
El proyecto se desarrolló de manera incremental, siguiendo la estructura del libro:

### Capítulo 1 – Introducción
- Comprensión de la arquitectura general de un intérprete.
- Definición de los objetivos y alcance del lenguaje.

### Capítulo 2 – Un lenguaje completo
- Análisis del lenguaje Lox y sus características principales.
- Preparación del entorno de desarrollo en C++.

### Capítulo 3 – El lenguaje Lox
- Definición de la sintaxis y semántica básica del lenguaje.
- Identificación de tokens, expresiones y sentencias.

### Capítulo 4 – Escaneo (Scanner)
- Implementación del análisis léxico en C++.
- Conversión del código fuente en una secuencia de tokens.
- Manejo de errores léxicos.

### Capítulo 5 – Representación de expresiones
- Construcción de árboles de sintaxis abstracta (AST).
- Uso de clases y herencia para representar expresiones.

### Capítulo 6 – Parseo de expresiones
- Implementación de un parser recursivo descendente.
- Validación de la estructura sintáctica del código fuente.

### Capítulo 7 – Evaluación de expresiones
- Implementación del intérprete para evaluar expresiones.
- Soporte para operadores aritméticos, lógicos y comparaciones.

### Capítulo 8 – Ejecución de sentencias
- Implementación de sentencias como `print` y expresiones.
- Ejecución secuencial del programa.

### Capítulo 9 – Control de flujo
- Implementación de estructuras de control como `if` y `while`.
- Manejo de bloques y alcance local.

### Capítulo 10 – Entornos
- Implementación de entornos para el manejo de variables.
- Soporte para alcance (scope) y encadenamiento de entornos.

### Capítulo 11 – Funciones
- Implementación de funciones definidas por el usuario.
- Manejo de parámetros, retorno de valores y pila de llamadas.

### Capítulo 12 – Resolución y enlaces
- Implementación del resolvedor semántico.
- Enlace de identificadores con sus declaraciones.
- Detección de errores semánticos antes de la ejecución.

## Cómo ejecutar el proyecto
1. Clonar el repositorio desde GitHub.
2. Compilar el proyecto utilizando un compilador compatible con C++ (g++ o similar).
3. Ejecutar el intérprete desde la línea de comandos.
4. Probar el lenguaje ingresando código fuente desde archivos o entrada estándar.

## Autor
Pedro Aquino
