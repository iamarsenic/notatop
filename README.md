# NotATop

Choose language / Выберите язык / Selecciona el idioma:
* [English](#english)
* [Русский](#русский)
* [Español](#español)

---

## English

A console utility designed to read PC sensors (supports up to 10 sensors).

> **Note:** Developed and tested on Gentoo Linux.

### Features
* **No arguments needed:** Runs very simply. Just use `./nat`, or run `nat` if installed via `make install` to `/usr/local/bin`.
* **Powered by NCurses:** Clean and beautiful terminal user interface.

### System Requirements
* **Operating System:** Linux
* **Build Tools:** CMake, a C compiler (`gcc`, `clang`, etc.), and standard `make` utility.

### Build and Installation

Clone the repository and compile the source code:

```bash
git clone https://github.com/iamarsenic/notatop.git
cd notatop
mkdir build && cd build
cmake ..
make
```

After a successful build, the executable binary will appear in the `build` directory.

### About the Project
Why **NotATop**? Because RAM benchmarks, performance tests, and other advanced features will be added soon. As you know, standard `top` utilities usually cannot do that. However, since this is just the first version, I am still figuring out the best way to implement these ideas.

---

## Русский

Консольная утилита, созданная для считывания датчиков ПК (поддерживает до 10 штук).

> **Примечание:** Разработано и протестировано на дистрибутиве Gentoo Linux.

### Особенности
* **Без аргументов:** Запускается максимально просто и понятно с помощью `./nat`. Если утилита установлена в `/usr/local/bin` через `make install`, достаточно ввести `nat`.
* **Использует NCurses:** Красивый и отзывчивый интерфейс прямо в терминале.

### Системные требования
* **Операционная система:** Linux
* **Инструменты сборки:** CMake, компилятор с поддержкой C (`gcc`, `clang` и т.д.), стандартная утилита `make`.

### Сборка и установка

Клонируйте репозиторий и соберите проект:

```bash
git clone https://github.com/iamarsenic/notatop.git
cd notatop
mkdir build && cd build
cmake ..
make
```

После сборки готовый бинарный файл появится в директории `build`.

### О проекте
Почему **NotATop**? Потому что в скором времени туда будут добавлены тесты оперативной памяти, производительности и другие фичи. Вы же знаете, что далеко не каждый `top` так умеет. Но так как это пока первая версия, я еще думаю над тем, как лучше всего это реализовать.

---

## Español

Una utilidad de consola diseñada para leer los sensores de la PC (soporta hasta 10 sensores).

> **Nota:** Desarrollado y probado en la distribución Gentoo Linux.

### Características
* **Sin argumentos:** Se ejecuta de forma muy simple y clara usando `./nat`, o simplemente `nat` si se instala en `/usr/local/bin` mediante `make install`.
* **Basado en NCurses:** Interfaz de usuario limpia y atractiva directamente en la terminal.

### Requisitos del Sistema
* **Sistema Operativo:** Linux
* **Herramientas de Construcción:** CMake, un compilador de C (`gcc`, `clang`, etc.) y la utilidad estándar `make`.

### Compilación e Instalación

Clona el repositorio y compila el código fuente:

```bash
git clone https://github.com/iamarsenic/notatop.git
cd notatop
mkdir build && cd build
cmake ..
make
```

Después de una compilación exitosa, el archivo binario ejecutable aparecerá en el directorio `build`.

### Sobre el Proyecto
¿Por qué **NotATop**? Porque pronto se añadirán pruebas de memoria RAM, tests de rendimiento y otras funciones avanzadas. Como sabes, no todas las utilidades tipo `top` pueden hacer eso. Sin embargo, dado que esta es solo la primera versión, todavía estoy pensando en la mejor manera de implementar estas ideas.

