#ifndef MAIN_HPP    
#define MAIN_HPP  
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <functional>
#include <cstdlib>
#include <unordered_map>
#include <string>

char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
    perror("tcgetattr()");


old.c_lflag &= ~ICANON;
old.c_lflag &= ~ECHO;
old.c_cc[VMIN] = 1;
old.c_cc[VTIME] = 0;  

if (tcsetattr(0, TCSANOW, &old) < 0)
    perror("tcsetattr ICANON");

if (read(0, &buf, 1) < 0)
    perror("read()");

old.c_lflag |= ICANON;
old.c_lflag |= ECHO;
if (tcsetattr(0, TCSADRAIN, &old) < 0)
    perror("tcsetattr ~ICANON");

return buf;
}
const std::string COLOR_RESET = "\033[0m";
const std::string PLAYER_COLOR = "\033[1;32m";  
const std::string WALL_COLOR = "\033[1;31m";  
class Entity {
    private:
        int x;
        int y;
        bool isAlive;
        std::string tile;
    public:
        int GetX() { return x; }
        int GetY() { return y; }
        std::string GetEntityTile() { return tile; }
        bool GetAlive() { return isAlive; }
        void SetX(int newX) { x = newX; }
        void SetY(int newY) { y = newY; }
        void SetY(std::string t) { tile = t; }
        void SetAlive(bool newAlive) { isAlive = newAlive; }
        
        Entity(int px, int py, std::string t) {
            x = px;
            y = py;
            tile = t;
            isAlive = true;
        }
    };
    class Keybind {
        private:
            std::unordered_map<char, std::function<void()>> bindings;  
            
        public:
            void bind(char key, std::function<void()> func) {  
                bindings[key] = func;
            }
            
            bool handle(char key) {
                auto it = bindings.find(key);
                if (it != bindings.end()) {
                    it->second();  
                    return true;
                }
                return false;
            }
        };
class Game {
    private:
        
        int HEIGHT;
        int WIDTH;
        std::string** map;
    public:
    
    int GetH() const { return HEIGHT; }
        int GetW() const { return WIDTH; }
        std::string** GetMap() const { return map; }
        Game(int h, int w) : HEIGHT(h), WIDTH(w) {
            map = new std::string*[HEIGHT];
            for (int i = 0; i < HEIGHT; ++i) {
                map[i] = new std::string[WIDTH];
            }
        }
void LoadMap(const std::initializer_list<std::initializer_list<std::string>>& data) {
    int i = 0;
    for (const auto& row : data) {
        int j = 0;
        for (const auto& cell : row) {
            if (i < HEIGHT && j < WIDTH) {
                map[i][j] = cell;
            }
            j++;
        }
        i++;
    }
}

~Game() {
    for (int i = 0; i < HEIGHT; ++i) {
        delete[] map[i]; 
    }
    delete[] map; 
}
};
class sound {
    public:
    
        static void play(const std::string& filepath, bool interrupt = false) {
            if (interrupt) {
                system("pkill -f 'play' 2>/dev/null");
            }
                std::string command = "play " + filepath + " 2>/dev/null &";
                system(command.c_str());
        }
};
class Render {
    public:
            static void Rend(Game& g) {
            system("clear");
            std::string** map = g.GetMap(); 
            int h = g.GetH();
            int w = g.GetW();
            for (int i{}; i < h; i++) {
                for (int z{}; z < w; z++) {
                    std::cout << map[i][z];
                    }
                    std::cout << std::endl; 
                }   
                std::cout << std::endl; 

            }
            static void UIRender(const std::initializer_list<std::string>& items) {
                for (const auto& item : items) {
                    std::cout << item << "\n";
                }
            }
        
            static void UIRender(const std::string& item) {
                std::cout << item << "\n";
            }
        
            static void UIRender(const char* item) {
                UIRender(std::string(item));
            }
};

#endif