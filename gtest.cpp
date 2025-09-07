#include<graphics.h>
#include "../DSA/includes.cpp"

const int WIDTH = 800;
const int HEIGHT = 700;
const char* TITLE = "MY FIRST WINDOW";

struct vec2
{
    int x; int y;

    vec2() : x(0),y(0){};
    vec2(int xcoord,int ycoord) : x(xcoord),y(ycoord){};
    vec2(const vec2& pos) : x(pos.x), y(pos.y){};    
};
struct pnt
{
    int x; int y;
    #define O pnt(0,0);
    pnt() : x(WIDTH/2), y(HEIGHT/2){};
    pnt(int posx, int posy) : x(posx + WIDTH/2), y(HEIGHT/2 - posy){};
    pnt(const pnt& p) : x(p.x), y(p.y){};
    pnt(const vec2& v) : x(v.x + WIDTH/2), y(HEIGHT/2 - v.y){};
};

void plot(const pnt& p1, const pnt& p2)
{
    line(p1.x, p1.y, p2.x, p2.y);
}
void plot(vector<pnt>& pnts)
{
    pnt prev = pnts[0];
    pnt curr = pnts[0];
    for(auto i : pnts)
    {
        setcolor(RED);
        curr = i;
        plot(prev, curr);
        prev = curr;
    }
}

vector<pnt> spiralOrder(vector<vector<int>>& matrix, int scale, int offset) 
{
    vector<pnt> ans;
    int r = matrix.size();
    int c = matrix[0].size();
    int top = 0, right = c-1, left = 0, bottom = r-1;

    while(top <= bottom && right >= left)
    {
        for(int j = left, i = top; j <= right; j++)
        {
            ans.push_back(pnt(i*scale + offset,j*scale + offset));
        }
        for(int i = top+1, j = right; i <= bottom; i++)
        {
            ans.push_back(pnt(i*scale + offset,j*scale + offset));
        }
        for(int j = right-1, i = bottom; j >= left && top < bottom; j--)
        {
            ans.push_back(pnt(i*scale + offset,j*scale + offset));
        }
        for(int i = bottom - 1, j = left; i > top && left < right; i--)
        {
            ans.push_back(pnt(i*scale + offset,j*scale + offset));
        }
        top++; right --; left++; bottom--;
    }
    return ans;
}

main()
{
    initwindow(WIDTH, HEIGHT, TITLE);

    #pragma region AXES
    vec2 X = {0,HEIGHT/2};
    vec2 XX = {WIDTH,HEIGHT/2};
    vec2 Y = {WIDTH/2,0};
    vec2 YY = {WIDTH/2, HEIGHT};
    line(X.x,X.y, XX.x,XX.y);
    line(Y.x,Y.y, YY.x,YY.y);
    setcolor(BLUE);
    outtextxy(2,-2, const_cast<char*>("O"));
    #pragma endregion

    // pnt a = {100,-100};
    // plot(O,a);

    vector<pnt> pnts = {
        {0,10}, {10,50}, {20,100},{30, 120}, {40, 150}, {50, 200},
        {60, 250}, {70, 290}, {80, 300}, {90, 320}, {100, 349}
    };

    vector<vector<int>> vec2d{
        {1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1},
    };
    int scale = 7;
    int offset = -125;
    vector<pnt> spiral = spiralOrder(vec2d,30, offset);

    plot(spiral);
    
    getch();
    closegraph();
}