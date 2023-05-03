#include <iostream>
#include <vector>
#include <array>
#include <conio.h>

const int width = 100;
const int height = 28;

struct triangle;
bool isColliding(const triangle &triangle1, const triangle &triangle2);

struct vec2
{
    int x;
    int y;
};

struct triangle
{
    triangle(std::array<vec2, 3> arr) : points(arr) {}

    void setPosition(const vec2& pos, int n)
    {
        try
        {
            points.at(n) = pos;
        }
        catch (const std::out_of_range&) {}
    }

    std::array<vec2, 3>points;
};




class Screen
{

public:
    void print()
    {
        std::cout << "\033[?25l"; //hide coursor
        this->clear();
        this->drawFigures();
        std::cout << "\338"; //back to (0,0)

        if (isColliding(_figures.at(0), _figures.at(1)))
        {
            std::cout << "\033[31m";
        }
        for (size_t i = 0; i < height; i++)
        {
            for (size_t j = 0; j < width; j++)
            {
                std::cout << _screen[j][i];
            }
            std::cout << std::endl;
        }
        std::cout << "\033[0m";
        
    }

    void addFigure(const triangle& fig)
    {
        _figures.push_back(fig);
    }

    std::vector<triangle>& getFigures()
    {
        return _figures;
    }


private:

    void drawFigures()
    {
        for (auto& fig : _figures)
        {
            auto& corners = fig.points;

            if (corners.size() < 1) break;
            if (corners.size() == 1) { _screen[corners.at(0).x][corners.at(0).y] = '*'; break; } //point


            for (size_t i = 0; i < corners.size(); i++)
            {
                vec2 beginPos = corners.at(i);
                vec2 endPos = corners.at((i + 1) % corners.size());
                
                if (beginPos.x == endPos.x) //y = x
                {
                    if (beginPos.y > endPos.y) std::swap(beginPos, endPos);
                    int y = beginPos.y;
                    while (y <= endPos.y)
                    {
                        _screen[beginPos.x][y] = 'O';
                        y++;
                    }

                    continue;
                }

                float a = (static_cast<float>(corners.at(i).y - corners.at((i + 1) % corners.size()).y)       //y = ax + b
                    / (corners.at(i).x - corners.at((i + 1) % corners.size()).x));
               

                float b = static_cast<float>(corners.at(i).y - (a * static_cast<float>(corners.at(i).x)));


                if(beginPos.x > endPos.x) std::swap(beginPos, endPos);
                
                for (int x = beginPos.x; x <= endPos.x; x++)
                {
                    int y = round(a * static_cast<float>(x) + b);
                    markPixel(x, y);

                    if (x == endPos.x - 1)
                    {
                        while (y < endPos.y)
                        {

                            int x = round(static_cast<float>(y - b) / a);
                            markPixel(x, y);
                            y++;
                        }
                    }

                    else if (int y1 = round(a * static_cast<float>(x + 1) + b); abs(y1 - y) > 1) // | y(x+1) - y(x) |
                    {
                        if (y > y1)
                        {
                            while (y > endPos.y)
                            {

                                int x = round(static_cast<float>(y - b) / a);
                                markPixel(x, y);
                                y--;
                            }
                            
                        }
                        else
                        {
                            while (y < endPos.y)
                            {
                                int x = round(static_cast<float>(y - b) / a);
                                markPixel(x, y);
                                y++;
                            }
                        }
                    }
                   
                }
            }
        }
    }

    void clear()
    {
        for (size_t i = 0; i < width; i++)
        {
            for (size_t j = 0; j < height; j++)
            {
                _screen[i][j] = ' ';
            }
        }

        //draw frame
        for (size_t i = 0; i < width; i++) 
        {
            _screen[i][0] = 'x';
            _screen[i][height - 1] = 'x';
        }
        for (size_t i = 0; i < height; i++) 
        {
            _screen[0][i] = 'x';
            _screen[width - 1][i] = 'x';
        }

      
    }



    void markPixel(int x, int y)
    {
        if (y < height && x < width)
        {
             _screen[x][y] = 'O';
        }
    }

    char _screen[width][height]{ ' ' };
    std::vector<triangle> _figures;

};


class Controller
{
public:
    Controller(Screen& scr) : _screen(scr) {}

    void setControllingFigure(int n)
    {
        conrollingFig = n;
    }

    void waitForInput()
    {
        auto& ref = _screen.getFigures();

        if (ref.size() <= conrollingFig) return; //controlling fig out of scope
    

        if (_getch() == 224) {

            int addX = 0;
            int addY = 0;

            switch (_getch()) { 
            case 72: //up
                addY = -1;
                break;
            case 80: //down
                addY = 1;
                break;
            case 77: //right
                addX = 1;
                break;
            case 75: //left
                addX = -1;
                break;
            default:
                break;
            }

            triangle& movingFigure = ref.at(conrollingFig);
            bool movable = true;

            for (auto& corner : movingFigure.points)
            {
                if (addX == 1 && corner.x >= width - 2) movable = false;
                if (addX == -1 && corner.x <= 1) movable = false;
                if (addY == 1 && corner.y >= height - 2) movable = false;
                if (addY == -1 && corner.y <= 1) movable = false;
            }
            if(movable)
            {
                for (auto& corner : movingFigure.points)
                {
                    corner.x += addX;
                    corner.y += addY;
                }
            }
        }

    }

private:
    Screen& _screen;
    int conrollingFig=0;
};


float det2D(vec2 p1, vec2 p2, vec2 p3)
{
    return +p1.x * (p2.y - p3.y)
        + p2.x * (p3.y - p1.y)
        + p3.x * (p1.y - p2.y);
}

void CheckTriWinding(vec2& p1, vec2& p2, vec2& p3)
{
    float detTriangle = det2D(p1, p2, p3);
    if (detTriangle < 0.0)
    {
        vec2 temp = p3;
        p3 = p2;
        p2 = temp;
    }

}

int main()
{
    Screen screen;
    Controller controller(screen);

    screen.addFigure(triangle(std::array<vec2, 3>({ {5,5}, {10,5}, {5,10} })));
    screen.addFigure(triangle(std::array<vec2, 3>({ {40,7}, {30,17}, {50,17} })));

    controller.setControllingFigure(0);
    
    while (1)
    {
        screen.print();
        controller.waitForInput();
    }
    

}



bool isColliding(const triangle& triangle1, const triangle& triangle2)
{

    triangle tr1 = triangle1;
    triangle tr2 = triangle2;
    

    vec2 pos1[3] = { tr1.points.at(0), tr1.points.at(1), tr1.points.at(2) };
    vec2 pos2[3] = { tr2.points.at(0), tr2.points.at(1), tr2.points.at(2) };


    CheckTriWinding(pos1[0], pos1[1], pos1[2]);
    CheckTriWinding(pos2[0], pos2[1], pos2[2]);


    for (int i = 0; i < 3; i++)
    {
        int j = (i + 1) % 3;

        if ((det2D(pos1[i], pos1[j], pos2[0]) < 0.0f) &&
            (det2D(pos1[i], pos1[j], pos2[1]) < 0.0f) &&
            (det2D(pos1[i], pos1[j], pos2[2]) < 0.0f))
            return false;
    }

    for (int i = 0; i < 3; i++)
    {
        int j = (i + 1) % 3;

        if ((det2D(pos2[i], pos2[j], pos1[0]) < 0.0f) &&
            (det2D(pos2[i], pos2[j], pos1[1]) < 0.0f) &&
            (det2D(pos2[i], pos2[j], pos1[2]) < 0.0f))
            return false;
    }

    return true;
}