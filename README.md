# NotATop v1.17.0

Choose language / Выберите язык:
* [English](#english)
* [Русский](#русский)

---

## English

A console utility designed to read PC sensors (supports up to 10 sensors).

> **Note:** Developed and tested on Gentoo Linux on Lenovo ThinkPad L14 Gen 1.

### WHAT'S NEW
* **CPU FREQ:** yay we have a cpu freq :3

### Features
* **No arguments needed:** Runs very simply. Just use `./nat`, or run `nat` if installed via `make install` to `/usr/local/bin`.
* **Powered by NCurses:** Clean and beautiful terminal user interface.

### System Requirements
* **Operating System:** Linux (runs on toaster!)
* **Build Tools:** CMake, a C compiler (`gcc`, `clang`, etc.), and standard `make` utility or `Ninja`.

### Build and Installation

Clone the repository and compile the source code:

```bash
git clone https://github.com/iamarsenic/notatop.git
cd notatop
mkdir build && cd build
cmake ..
make
```

* **OR**

```bash
git clone https://github.com/iamarsenic/notatop.git
cd notatop
mkdir build
cmake -B build -G Ninja
cmake --build build
```
After a successful build, the executable binary will appear in the `build` directory.

### About the Project
Why **NotATop**? Because RAM benchmarks, performance tests, and other advanced features will be added soon. As you know, standard `top` utilities usually cannot do that. However, since this is just the first version, I am still figuring out the best way to implement these ideas.

### SPOILERS
* **First Test:** coming soon...
---

## Русский

Консольная утилита, созданная для считывания датчиков ПК (поддерживает до 10 штук).

> **Примечание:** Разработано и протестировано на дистрибутиве Gentoo Linux который на Lenovo ThinkPad L14 Gen 1.

### ЧТО НОВОГО
* **Частота процессора:** теперь у нас есть частота процессора... зашибись.

### Особенности
* **Без аргументов:** Запускается максимально просто и понятно с помощью `./nat`. Если утилита установлена в `/usr/local/bin` через `make install`, достаточно ввести `nat`.
* **Использует NCurses:** Красивый и отзывчивый интерфейс прямо в терминале.

### Системные требования
* **Операционная система:** Linux (может работать даже на тостере!)
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

* **ИЛИ**


```bash
git clone https://github.com/iamarsenic/notatop.git
cd notatop
mkdir build
cmake -B build -G Ninja
cmake --build build
```

После сборки готовый бинарный файл появится в директории `build`.

### О проекте
Почему **NotATop**? Потому что в скором времени туда будут добавлены тесты оперативной памяти, производительности и другие фичи. Вы же знаете, что далеко не каждый `top` так умеет. Но так как это пока первая версия, я еще думаю над тем, как лучше всего это реализовать.

### СПОЙЛЕРЫ
* **Первые Тесты:** скоро...
