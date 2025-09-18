## WqTerms — библиотека для игр в терминале (C++)

WqTerms — это лёгкая заголовочная библиотека для создания простых ASCII/Unicode-игр в терминале Linux. В комплекте есть утилита редактора карт (`MapMaker`).

- **Рендер**: вывод 2D-карт из строк напрямую в терминал
- **Ввод**: обработка нажатий через биндинги `Keybind`
- **Сущности**: класс `Entity` с координатами и тайлом
- **Игровое поле**: класс `Game` с выделением карты и загрузкой содержимого
- **Звук**: `sound::play()` (требует SoX `play`)

### Требования
- Linux, bash/любая оболочка
- Компилятор C++17+ (например, g++)
- Для звука: установленный SoX (`play`)

Установка SoX (Arch/Manjaro):
```bash
sudo pacman -S sox
```

### Структура
- `wqterm.hpp` — основной заголовок (header-only)
- `MapMaker.cpp` — исходник утилиты редактора карт
- `Map` — пример экспортированной карты (2D-массив `std::string`)

---

## Быстрый старт

Подключите заголовок и соберите ваш проект.

Сборка и запуск простого примера:
```bash
g++ -std=gnu++17 -O2 your_game.cpp -o your_game && ./your_game
```

Минимальный пример использования `Game` и `Render`:
```cpp
#include "wqterm.hpp"

int main() {
    Game game(4, 6); //height, width

    // Загрузка карты строками (каждая ячейка — std::string)
    game.LoadMap({
        {"█", "█", "█", "█", "█", "█"},
        {"█", " ", " ", " ", " ", "█"},
        {"█", "☻", " ", "✵", " ", "█"},
        {"█", "█", "█", "█", "█", "█"}
    });

    Render::Rend(game);
}
```

Обработка нажатий через `Keybind`:
```cpp
Keybind input;
 input.bind('s', [&]() { movePlayer(0, 1); });
 input.bind('w', [&]() { movePlayer(0, -1); });
 input.bind('d', [&]() { movePlayer(1, 0); });
 input.bind('a', [&]() { movePlayer(-1, 0); });
    while (1)
    {
        Render::Rend(g);
        char key = getch();
        input.handle(key);
    }
```
Воспроизведение звука:
```cpp
sound::play("./laser.wav", /*interrupt*/ true);
```

---

## Обзор API


- **Entity**:
  - Данные: `x`, `y`, `isAlive`, `tile`
  - Методы: `GetX/SetX`, `GetY/SetY`, `GetEntityTile`, `GetAlive/SetAlive`
  - Конструктор: `Entity(int x, int y, std::string tile)`

- **Keybind**:
  - `bind(char key, std::function<void()>)`
  - `handle(char key)` — вызывает привязанную функцию, если есть

- **Game**:
  - Конструктор: `Game(int height, int width)` — выделяет `HEIGHT x WIDTH` карту строк
  - `GetH()`, `GetW()`, `GetMap()` — доступ к размерам и сырым данным карты (`std::string**`)
  - `LoadMap(std::initializer_list<std::initializer_list<std::string>>)` — заполнение карты

- **Render**:
  - `Render::Rend(Game&)` — очистка экрана и вывод всей карты
  - `Render::UIRender(...)` — перегружен для `initializer_list<string>`, `string`, `const char*`

- **sound**:
  - `sound::play(filepath, interrupt=false)` — проигрывает файл через SoX `play`

Замечание: `Render::Rend` печатает содержимое ячеек как есть. Используйте Unicode (например, "█", "☻", "✵") и ANSI-цвета прямо в строках.

---

## Entity: создание, перемещение, отрисовка

Создание игрока и размещение на карте `Game`:
```cpp
Entity player(1, 1, PLAYER_COLOR + "☻" + COLOR_RESET);
Game game(12, 12);

game.LoadMap({
    {"█","█","█","█","█","█","█","█","█","█","█","█"},
    {"█","☻"," "," "," "," "," "," "," "," "," ","█"},
    {"█"," "," "," "," "," "," "," "," "," "," ","█"},
    {"█"," "," "," "," "," "," "," "," "," "," ","█"},
    {"█"," "," "," "," "," "," "," "," "," "," ","█"},
    {"█"," "," "," "," "," "," "," "," "," "," ","█"},
    {"█"," "," "," "," "," "," "," "," "," "," ","█"},
    {"█"," "," "," "," "," "," "," "," "," "," ","█"},
    {"█"," "," "," "," "," "," "," "," "," "," ","█"},
    {"█"," "," "," "," "," "," "," "," "," "," ","█"},
    {"█"," "," "," "," "," "," "," "," "," "," ","█"},
    {"█","█","█","█","█","█","█","█","█","█","█","█"}
});


Render::Rend(game);
```


Проверка состояния жизни и смена тайла:
```cpp
if (!player.GetAlive()) {
    // например, показать надпись
    Render::UIRender("Game Over");
    break;
}

```

---

## Работа с MapMaker (редактор карт)

`MapMaker` — консольная утилита для интерактивного создания карт и экспорта в файл `Map`.

Сборка:
```bash
g++ -std=gnu++17 -O2 MapMaker.cpp -o MapMaker
```

Управление (по умолчанию):
- **w/a/s/d**: перемещение курсора по карте
- **1**: поставить стену `"█"`
- **2**: поставить объект `"✵"`
- **c**: экспортировать карту в файл `Map`
- **q**: выход

Экспорт создаёт файл вида:
```cpp
std::string map[12][12] = {
    {"█", "█", "█", ...},
    {"█", " ", " ", ...},
    // ...
};
```



---
## Пример игры
```cpp
#include "wqterm.hpp"
Game g(20, 30);
Entity player(3, 15, "☻");


std::string tileUnderPlayer = " ";
void movePlayer(int dx, int dy) {
    std::string** map = g.GetMap();
    int py = player.GetY();
    int px = player.GetX();
    int newPy = py + dy;
    int newPx = px + dx;
    std::string targetTile = map[newPy][newPx];
    if (targetTile == "█" || targetTile == "░" || targetTile == "\033[31m█\033[0m") return; 
    if (targetTile == "✵") {
        targetTile = " ";
    }
    if (targetTile == "‖") {
        targetTile = "‖";
    }
    map[py][px] = tileUnderPlayer;
    tileUnderPlayer = targetTile;
    player.SetY(newPy);
    player.SetX(newPx);
    map[newPy][newPx] = player.GetEntityTile();
}


int main() {
    
    
    Keybind input;
    g.LoadMap({}); // Ваша карта
    input.bind('v', []() { sound::play("test.mp3", true); });
    input.bind('s', [&]() { movePlayer(0, 1); });
    input.bind('w', [&]() { movePlayer(0, -1); });
    input.bind('d', [&]() { movePlayer(1, 0); });
    input.bind('a', [&]() { movePlayer(-1, 0); });
    while (1)
    {
        Render::Rend(g);
        char key = getch();
        input.handle(key);
    }
    
    return 0;
}

```
## Сборка и запуск

- Библиотека заголовочная, включите `wqterm.hpp` и соберите ваши `.cpp`.
- Примеры сборки:
```bash
# Ваше приложение
g++ -std=gnu++17 -O2 main.cpp -o app

# Утилита редактора карт
g++ -std=gnu++17 -O2 MapMaker.cpp -o MapMaker
```

Если используете звук:
```bash
# Убедитесь, что команда `play` доступна
play --version | head -n 1
```



