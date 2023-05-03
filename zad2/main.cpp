#include <iostream>
#include <chrono>
#include <cmath>
#include <vector>
#if _WIN32
#include <windows.h>
#endif


using namespace std;


class Screen
{
public:
        Screen()
        {
#ifdef _WIN32
                _hConsole= GetStdHandle(STD_OUTPUT_HANDLE);
#endif
                hideCursor();
                saveCursorPosition();
        }


        void clear()
        {
                for(int i = 0; i < height; i++)
                {
                        for(int j = 0; j < width; j++)
                        {
                                _screen[i][j]='x';
                        }
                }
        }


        void draw(char value, float x, float y)
        {
                int xPos = lround(x);
                int yPos = lround(y);
                if(xPos < width && yPos < height)
                {
                        _screen[yPos][xPos]=value;
                }
        }


        void flush()
        {
                restoreCursorPosition();


                for(int i = 0; i < height; i++)
                {
                        for(int j = 0; j < width; j++)
                        {
                                if(_screen[i][j]!='x')
                                {
                                        setRedFont();
                                }
                                else
                                {
                                        setWhiteFont();
                                }
                                cout << _screen[i][j];
                        }
                        cout << "\n";
                }
        }


private:
        void hideCursor()
        {
#ifdef _WIN32
                CONSOLE_CURSOR_INFO cursorInfo; 
                GetConsoleCursorInfo(_hConsole, &cursorInfo);
                cursorInfo.dwSize = 1;
                cursorInfo.bVisible = FALSE;
                SetConsoleCursorInfo(_hConsole, &cursorInfo);
#else
                cout << "\33[?25l";//disable cursor
#endif


        }


        void saveCursorPosition()
        {
#ifdef _WIN32
                CONSOLE_SCREEN_BUFFER_INFO coninfo;
                GetConsoleScreenBufferInfo(_hConsole, &coninfo);
                _prevPos = coninfo.dwCursorPosition;
#else
                cout << "\33[s";
#endif
                
        }


        void restoreCursorPosition()
        {
#ifdef _WIN32
                SetConsoleCursorPosition(_hConsole, _prevPos);
#else
                cout << "\338";
#endif
        }


        void setRedFont()
        {
#ifdef _WIN32
                WORD attributes = FOREGROUND_RED;
                SetConsoleTextAttribute(_hConsole, attributes);
#else
                cout << "\33[31m";//red color
#endif
        }


        void setWhiteFont()
        {
#ifdef _WIN32
                WORD attributes = FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE;
                SetConsoleTextAttribute(_hConsole, attributes);
#else
                cout << "\33[37m";//white color
#endif
        }


        static constexpr int width{60};
        static constexpr int height{20};
        char _screen[height][width];
#ifdef _WIN32
        HANDLE _hConsole;
        COORD _prevPos;
#endif
};


struct Position
{
        float x;
        float y;
};


using Pixel = char;


class Engine
{
public:
        void setPosition(const Position& _position)
        {
            position = _position;
        }
        void update(chrono::milliseconds deltaTime)
        {


        }


        void render(Screen& screen)
        {
                screen.draw(pixel, position.x, position.y);
        }
private:
        Pixel pixel{1};
        Position position{0.0f, 0.0f};
};


const std::vector<Position> positions = { {0.0f, 0.0f}, {40.0f, 0.0f}, {40.0f, 10.0f} };
const std::vector<chrono::milliseconds> durations = { 2000ms, 3000ms, 1000ms };
const int repetitions = 5;


Position GetPos (int currentRoad, chrono::milliseconds currentTime)
{
    float progress = (static_cast<float>(currentTime.count()) / durations.at(currentRoad).count());

    if (progress > 1.0f) progress = 1.0f;

    return { ((positions.at((currentRoad + 1) % positions.size()).x - positions.at(currentRoad).x) * progress) + positions.at(currentRoad).x,       //pos2.x - pos1.x * progress + pos1.x
             ((positions.at((currentRoad + 1) % positions.size()).y - positions.at(currentRoad).y) * progress) + positions.at(currentRoad).y };     //pos2.y - pos1.y * progress + pos1.y
};


int main()
{
        Engine engine;
        Screen screen;

        int currentRepetition = 0;
        int currentRoute = 0;
       
        bool stop = false;

        auto startTime = chrono::high_resolution_clock::now();
        auto routeTime = chrono::high_resolution_clock::now();
        while(1)
        {
                auto now = chrono::high_resolution_clock::now();
                auto deltaTime = chrono::duration_cast<chrono::milliseconds>(now - startTime);
                auto deltaTimeRoute = chrono::duration_cast<chrono::milliseconds>(now - routeTime);
                if( deltaTime >=20ms)
                {
                        screen.clear();
                        //engine.update(deltaTime);
                        if(!stop)
                            engine.setPosition(GetPos(currentRoute, deltaTimeRoute));
                        engine.render(screen);
                        screen.flush();
                        startTime = now;
                        
                }
                
                if (deltaTimeRoute >= durations.at(currentRoute) )
                {
                    currentRoute++;
                    if (currentRoute >= positions.size())
                    {
                        currentRepetition++;
                        if (currentRepetition >= repetitions) stop = true;

                        currentRoute = 0;
                   }

                    routeTime = now;
                }
               
        }


        return 0;
}