# NotATop v1.18.1

Choose language / Выберите язык:
* [English](#english)
* [Русский](#русский)

---

## English

A console utility designed to read PC sensors (supports up to 10 sensors).

### What's new?
* **Load average**: now you can see what system uses cores (sorry for bad english)

### Features
* **No arguments needed:** Runs very simply. Just use `./nat`, or run `nat` if installed via `make install` to `/usr/local/bin`.
* **Powered by NCurses:** Clean and beautiful terminal user interface.

### System Requirements
* **Operating System:** Linux, mabye MacOS and WSL
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
cmake -B build -G ninja
cmake --build build
```

After a successful build, the executable binary will appear in the `build` directory.

(i love ninja)

### About the Project
Why **NotATop**? Because RAM benchmarks, performance tests, and other advanced features will be added soon. As you know, standard `top` utilities usually cannot do that. However, since this is just the first version, I am still figuring out the best way to implement these ideas.

### SPOILERS
* **First Test:** coming soon...
---

## Русский

Консольная утилита, созданная для считывания датчиков ПК (поддерживает до 10 штук).

### ЧТО НОВОГО
* **Средняя загрузка:** теперь вы видите насколько система пользуется ядрами

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
cmake -B build -G ninja
cmake --build build
```

После сборки готовый бинарный файл появится в директории `build`.

(я обожаю ninja)

### О проекте
Почему **NotATop**? Потому что в скором времени туда будут добавлены тесты оперативной памяти, производительности и другие фичи. Вы же знаете, что далеко не каждый `top` так умеет. Но так как это пока первая версия, я еще думаю над тем, как лучше всего это реализовать.

### СПОЙЛЕРЫ
* **Первые Тесты:** скоро...
