#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <string>
#include <fstream>
const std::string COLOR_RESET = "\033[0m";
const std::string PLAYER_COLOR = "\033[1;32m";  
const std::string WALL_COLOR = "\033[1;31m";   
const std::string DIRT = " ";
const std::string PLAYER = "☻";
const std::string WALL = "█";
const std::string SHIP = "✵";
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
class Program {
    private: 
        int HEIGHT;
        int x;
        int y;
        int WIDTH;
        std::string** map; 
        
    public:
        Program(int h, int w) :  x(1), y(1) {
            HEIGHT = h;
            WIDTH = w;
            

            map = new std::string*[HEIGHT];
            for (int i = 0; i < HEIGHT; i++) {
                map[i] = new std::string[WIDTH];
            }

            generate();
        }
        int getX() { return x; }
        int getY() { return y; }
        
        void generate() {

            for (int y = 0; y < HEIGHT; y++) {
                for (int x = 0; x < WIDTH; x++) {
                    map[y][x] = DIRT;
                }
            }
            
            for (int y = 0; y < HEIGHT; y++) {
                map[y][0] = WALL;
                map[y][WIDTH - 1] = WALL;
            }
            
            for (int x = 0; x < WIDTH; x++) {
                map[0][x] = WALL;
                map[HEIGHT - 1][x] = WALL;
            }
        }
        
        void render() {
            system("clear");
            for (int i = 0; i < HEIGHT; i++) {
                for (int z = 0; z < WIDTH; z++) {
                    std::cout << map[i][z];
                }   
                std::cout << std::endl; 
            }
        }  
        void setTile(std::string type) {
            map[y][x] = type;
        }

        std::string** getMap() { 
            return map; 
        }
        void mvr() {
            if (x + 1 < WIDTH - 1 && map[y][x + 1] != WALL) {
                if (map[y][x] != WALL && map[y][x] != SHIP) map[y][x] = DIRT;  // Только если не стена
                x += 1;
                map[y][x] = PLAYER;
                usleep(10000);
            }
        }
        

        void mvl() {
            if (x - 1 > 0 && map[y][x - 1] != WALL) {
                if (map[y][x] != WALL && map[y][x] != SHIP) map[y][x] = DIRT;
                x -= 1;
                map[y][x] = PLAYER;
                usleep(10000);
            }
        }
        
        void mvt() {
            if (y - 1 > 0 && map[y - 1][x] != WALL) {
                if (map[y][x] != WALL && map[y][x] != SHIP) map[y][x] = DIRT;
                y -= 1;
                map[y][x] = PLAYER;
                usleep(10000);
            }
        }
        

        void mvd() {
            if (y + 1 < HEIGHT - 1 && map[y + 1][x] != WALL) {
                if (map[y][x] != WALL && map[y][x] != SHIP) map[y][x] = DIRT;
                y += 1;
                map[y][x] = PLAYER;
                usleep(10000);
            }
        }
        void ExportMapToFile(const std::string& filename) {
            std::ofstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Не удалось открыть файл для записи!" << std::endl;
                return;
            }
            
            file << "std::string map[" << HEIGHT << "][" << WIDTH << "] = {\n";
            
            for (int i = 0; i < HEIGHT; ++i) {
                file << "    {";
                for (int j = 0; j < WIDTH; ++j) {
                    file << "\"" << map[i][j] << "\"";
                    if (j < WIDTH - 1) {
                        file << ", ";
                    }
                }
                file << "}";
                if (i < HEIGHT - 1) {
                    file << ",";
                }
                file << "\n";
            }
            
            file << "};\n";
            file.close();
            
            std::cout << "Карта экспортирована в " << filename << std::endl;
        }
    };


int main() {
    int h;
    int w;
    std::cout << "Please enter the map height: ";
    std::cin >> h;
    std::cout << "Please enter the map width: ";
    std::cin >> w;
    Program pr(h, w);
    pr.render();
    while(1) {
        char input = getch();
    switch(input) {
        case 'd': 
            pr.mvr();
            pr.render();
            break;
        case 'a': 
            pr.mvl();
            pr.render();
            break;
        case 'w': 
            pr.mvt();
            pr.render();
            break;
        case 's': 
            pr.mvd();
            pr.render();
            break;
        case 'q':
            return 0;
        case '1':
            pr.setTile(WALL);
            break;
        case '2':
            pr.setTile(SHIP);
            break;    
        case 'c':
            pr.ExportMapToFile("Map");
            break;
    }
    }

}


