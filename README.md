# HR Management System

Система управления персоналом с возможностями найма, увольнения и анализа данных сотрудников.

## 📌 Особенности

- Управление сотрудниками разных должностей (директора, бухгалтеры, программисты и др.)
- Расчет заработной платы
- Поиск и фильтрация сотрудников
- Генерация отчетов
- Работа с базой данных MySQL

## 🛠 Технологии

- C++17
- MySQL Connector/C++ 8.0
- CMake (сборка)
- Linux

## ⚙️ Установка

### Требования
- Компилятор с поддержкой C++17 (GCC 10+, Clang 12+, MSVC 2019+)
- MySQL Server 8.0
- CMake 3.15+

### 1. Установка зависимостей

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install g++ cmake libmysqlcppconn-dev mysql-server
```
**Windows:**

- Установите MySQL Community Server
- Установите MySQL Connector/C++
- Установите CMake

### 2. Настройка базы данных

```
sudo mysql -u root -p < init_db.sql
```
### 3. Сборка проекта
```
mkdir build && cd build
cmake ..
make
```
### 🚀 Запуск
```
./build/program
```
## 🗄  Структура базы данных
Основные таблицы:

- directors - директора

- employees - основные данные сотрудников

- programmer_details - программисты

- driver_details - водители
