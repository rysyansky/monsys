# Monitor

[English](#english) | [Русский](#русский)

---

## English

### Table of contents

- [Description](#description)
- [Technologies](#technologies)
- [Dependencies installation (Ubuntu)](#dependencies-installation-ubuntu)
- [Release build](#release-build)
- [Debug build](#debug-build)
- [Run and usage](#usage)
- [Launch flags](#launch-flags)

### Description

This repository contains a small test system-monitor application. It includes a C++ backend that collects system metrics and serves them via HTTP/WebSocket, and a TypeScript frontend that displays those metrics in a browser.

### Technologies

- Backend:
  - C++17/CMake
  - vcpkg used for third-party libraries (submodule under `backend/vcpkg`)
  - Crow
  - nlohmann/json
  - cxxopts
- Frontend:
  - TypeScript + Vite
  - CSS

### Dependencies installation (Ubuntu)

Open a terminal and run:

```bash
# System packages
sudo apt update
sudo apt install -y build-essential cmake git curl python3 unzip clang pkg-config

# Install Node.js
curl -fsSL https://deb.nodesource.com/setup_26.x | sudo -E bash -
sudo apt-get install -y nodejs
```

Clone this repo to your machine:

```bash
git clone --recurse-submodules https://github.com/rysyansky/monsys.git
cd monsys
# if you forgot to include --recurse-submodules you can initialize download:
# git submodule update --init
```

For `vcpkg` (backend dependency manager):

```bash
cd backend/vcpkg
./bootstrap-vcpkg.sh
# CMake presets in this repo will use vcpkg and download dependencies automatically.
```

Then install frontend dependencies:

```bash
cd frontend
npm install
```

### Release build

From the project root run the provided script `build.sh` which builds backend (release) and frontend (production) and copies artifacts into `monsys`:

```bash
# from project root
./build.sh

# then run the packaged app
./monsys/app
```

The script builds the backend using the `release` preset and runs `npm run build` for the frontend. After success, `monsys/static/` will contain built frontend files and `monsys/app` will be the backend executable.

### Debug build

Backend (terminal A):

```bash
cd backend
# using CMake presets
cmake --preset debug
cmake --build --preset debug
# run the debug binary
./build/debug/app
```

Frontend (terminal B):

```bash
cd frontend
# run the dev environment
npm run dev
```

Open the frontend dev server in your browser (Vite default: `http://localhost:5173`).

### Usage

- Production: `./monsys/app` then open `http://localhost:8080` in a browser.
- Dev: run backend debug build and `npm run dev` for frontend; open Vite URL (usually `http://localhost:5173`).

The frontend connects to the backend WebSocket endpoint at `ws://localhost:8080/ws` by default. UI basics:

- Left sidebar: CPU, Disk, Memory, Network, Uptime.
- Right area: process table with filter and sort controls.

#### Launch flags

The backend executable accepts the following command-line options (use `--help` to list them):

- `-p, --port <number>` - set HTTP/WebSocket listening port (default: `8080`). # available only on release build
- `-d, --debug` - enable debug logging output (flag; default: off).
- `-h, --help` - print help and exit.

Examples:

```bash
# run on port 9090 with debug logging
./monsys/app --port 9090 --debug

# show help
./monsys/app --help
```

---

## Русский

### Оглавление

- [Описание](#описание)
- [Технологии (Backend / Frontend)](#технологии)
- [Установка зависимостей (Ubuntu)](#установка-зависимостей-ubuntu)
- [Release сборка](#release-сборка)
- [Debug сборка](#debug-сборка)
- [Запуск и использование](#запуск-и-использование)
- [Флаги запуска](#флаги-запуска)

### Описание

Репозиторий содержит тестовое приложение для мониторинга системы. Включает бэкенд на C++, который собирает метрики и отдаёт их по HTTP/WebSocket, и фронтенд на TypeScript, который отображает метрики в браузере.

### Технологии

- Бэкенд:
  - C++17/CMake
  - vcpkg для сторонних библиотек (субмодуль в `backend/vcpkg`)
  - Crow
  - nlohmann/json
  - cxxopts
- Фронтенд:
  - TypeScript + Vite
  - CSS

### Установка зависимостей (Ubuntu)

В терминале выполните:

```bash
# Системные пакеты
sudo apt update
sudo apt install -y build-essential cmake git curl python3 unzip clang pkg-config

# Node.js
sudo apt install -y nodejs npm
```

Склонируйте этот репозиторий к себе на компьютер:

```bash
git clone --recurse-submodules https://github.com/rysyansky/monsys.git
cd monsys
# если вы забыли указать --recurse-submodules, вы можете инициализировать загрузку:
# git submodule update --init
```

Для `vcpkg` (менеджер зависимостей бэка):

```bash
cd backend/vcpkg
./bootstrap-vcpkg.sh
```

Установка зависимостей фронтенда:

```bash
cd frontend
npm install
```

### Release сборка

Из корня проекта выполните скрипт сборки `build.sh`:

```bash
./build.sh

# затем запустите приложение
./monsys/app
```

Скрипт соберёт backend в режиме release, соберёт frontend и разместит результаты в `monsys/static/`.

### Debug сборка

Бэкенд (терминал A):

```bash
cd backend
cmake --preset debug
cmake --build --preset debug
./build/debug/app
```

Фронтенд (терминал B):

```bash
cd frontend
npm run dev
```

Откройте фронтенд по адресу dev-сервера Vite (по умолчанию `http://localhost:5173`).

### Запуск и использование

- [Release](#release-сборка): `./monsys/app`, затем откройте `http://localhost:8080`.
- [Dev](#debug-сборка): запустите бэкенд и `npm run dev`, затем откройте URL Vite (по умолчанию `http://localhost:5173`).

Фронтенд подключается к WebSocket на `ws://localhost:8080/ws` по умолчанию. Интерфейс: левая панель - метрики (CPU, Disk, Memory, Network, Uptime), справа - таблица процессов с фильтром и сортировкой.

#### Флаги запуска

Исполняемы файл запуска бэкенда поддерживает данные флаги (используйте `--help` чтобы вывести их):

- `-p, --port <number>` - установить порт HTTP/WebSocket (по умолчанию: `8080`). # доступно только в релизной версии
- `-d, --debug` - показывать отладочный вывод (по умолчанию выключен).
- `-h, --help` - вывести эти флаги и завершить программу.

Примеры:

```bash
# запустить на порте 9090 с отладочным выводом
./monsys/app --port 9090 --debug

# показать флаги
./monsys/app --help
```
