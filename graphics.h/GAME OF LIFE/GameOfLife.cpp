#include<graphics.h>
#include<conio.h>
#include"../includes.cpp"

const int WIDTH = 800;
const int HEIGHT = 800;
const char* TITLE = "GAME OF LIFE";

const int NOISE_ITERATIONS = 4;
vector<vector<int>> GRID;
vector<vector<int>> NEWGRID;

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int> dist(0, 100);

bool busy = false;

#define ENTER '\r'
#define SPACE ' '

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
    pnt(int posx, int posy, bool offset = true) : 
    x( (offset == true) ? posx + WIDTH/2: posx), y( (offset == true) ? HEIGHT/2 - posy : posy){};
    pnt(const pnt& p) : x(p.x), y(p.y){};
    bool operator==(const pnt& p2) const{return (x==p2.x && y == p2.y);}
};
void plot(const pnt& p1, const pnt& p2, int color = WHITE)
{
    setcolor(color);
    line(p1.x, p1.y, p2.x, p2.y);
}
void plot(pnt p, int color = WHITE)
{
    putpixel(p.x,p.y,color);
}
void plot(vector<pnt>& pnts,int color = WHITE)
{
    pnt prev = pnts[0];
    pnt curr = pnts[0];
    for(auto i : pnts)
    {
        setcolor(color);
        curr = i;
        plot(prev, curr);
        prev = curr;
    }
}

int checkneighbours(vec2 curr)
{
    vector<int> offset = {-1,0,1};
    int count = 0;
    for(auto i : offset)
    {
        for(auto j : offset)
        {
            if(i == 0 && j == 0) continue;
            int nx = curr.x + i; int ny = curr.y + j;
            if(nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) 
                if(GRID[curr.x + i][curr.y + j] == 1) count++;
        }
    }
    return count;
}

void fillnoise(int it)
{
    //generate buffer
    for(int i = 0; i < WIDTH; i++)
    {
        for(int j = 0; j < HEIGHT; j++)
        {
            int bit = dist(gen); //0 to 100
            if(bit <= 40) GRID[i][j] = 1; //40% alive
            else GRID[i][j] = 0;
        }
    }
    //fill buffer
    for(int i = 0; i < WIDTH; i++)
    {
        for(int j = 0; j < HEIGHT; j++)
        {
            if(GRID[i][j] == 1) putpixel(i,j,WHITE);
        }
    }
    //generate smooth buffer
    for(int k = 0; k < it; k++)
    {
        for(int i = 0; i < WIDTH; i++)
        {
            for(int j = 0; j < HEIGHT; j++)
            {
                int alive = checkneighbours(vec2(i,j));
                if((alive >= 4)) NEWGRID[i][j] = 1;
                else NEWGRID[i][j] = 0;
            }
        }
        GRID.swap(NEWGRID);
    }
    //fill smooth buffer
    for(int i = 0; i < WIDTH; i++)
    {
        for(int j = 0; j < HEIGHT; j++)
        {
            if(GRID[i][j] == 1) putpixel(i,j,WHITE);
            else putpixel(i,j, BLACK);
        }
    }
}

void gameOfLife()
{
    //generate new grid
    for(int i = 0; i < WIDTH; i++)
    {
        for(int j = 0; j < HEIGHT; j++)
        {
            int alive = checkneighbours(vec2(i,j));
            if(alive > 3 || alive < 1) NEWGRID[i][j] = 0;
            else if(alive == 3 && GRID[i][j] == 1) NEWGRID[i][j] = 1;
            else NEWGRID[i][j] = 1;
        }
    }
    GRID.swap(NEWGRID);
    //fill new buffer
    for(int i = 0; i < WIDTH; i++)
    {
        for(int j = 0; j < HEIGHT; j++)
        {
            if(GRID[i][j] == 1) putpixel(i,j,WHITE);
            else putpixel(i,j, BLACK);
        }
    }
    delay(30);
}

template<typename Func, typename... Args>
void runfunc( Func func, Args... args )
{
    busy = true;
    func(args...);
    while(kbhit()) getch();
    busy = false;
}

int main()
{
    initwindow(WIDTH, HEIGHT, TITLE);

    GRID.assign(WIDTH,vector<int>(HEIGHT,0));
    NEWGRID.assign(WIDTH,vector<int>(HEIGHT,0));

    while(true)
    {
        if(kbhit())
        {
            if(busy)
            {
                while(kbhit()) getch();
                continue;
            }

            char ch = getch();
            switch (ch)
            {
                case ENTER:
                    runfunc(fillnoise,1);
                    break;
                case SPACE:
                    runfunc(gameOfLife);
                    break;
                
                default:
                    break;
            }
        }
    }
    closegraph();
}