#include<graphics.h>
#include<conio.h>
#include"../includes.cpp"

const int WIDTH = 800;
const int HEIGHT = 700;
const char* TITLE = "BETTER GAME OF LIFE";

const int CELLSIZE = 4;
const int GRIDX = WIDTH/CELLSIZE;
const int GRIDY = HEIGHT/CELLSIZE;

const int NOISE_ITERATIONS = 4;
//using uint_8 instead of int to prevent overhead
vector<vector<uint8_t>> GRID; 
vector<vector<uint8_t>> NEWGRID;
vector<vector<uint8_t>> GRIDCOPY;
//implementing a changed array to only update grids that experienced a change
vector<pair<int,int>> changed;

//Random generator stuff - just keep it as is
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int> dist(0, 100);

//Input Queue prevention
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

//returns num of alive neighbours
int checkneighbours(vec2 curr)
{
    const int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    const int dy[] = {-1, 1, 0, 1, -1, -1, 0, 1};
    int count = 0;

    for(int k = 0; k < 8; k++)
    {
        int nx = curr.x + dx[k];
        int ny = curr.y + dy[k];
        if(nx >= 0 && nx < GRIDX && ny >= 0 && ny < GRIDY)
            count += GRID[nx][ny];
    }
    return count;
}

//flood fill + procedural noise
void fillnoise(int it)
{
    //generate buffer
    for(int i = 0; i < GRIDX; i++)
    {
        for(int j = 0; j < GRIDY; j++)
        {
            int bit = dist(gen); //0 to 100
            //{10:DEATH Before GOL, 25: Quick Stability, 30:Slow stability, 40-45:Very slow stability, 50: Sparse Population, 60: Stable but very low population, 75: DEATH, 100: DEATH}
            if(bit <= 45) GRID[i][j] = 1; 
            else GRID[i][j] = 0;
        }
    }
    //fill buffer
    for(int i = 0; i < GRIDX; i++)
    {
        for(int j = 0; j < GRIDY; j++)
        {
            if(GRID[i][j] == 1)
            {
                setcolor(WHITE);
                bar(i*CELLSIZE, j*CELLSIZE, (i+1)*CELLSIZE, (j+1)*CELLSIZE);
            }
        }
    }
    //generate smooth buffer
    GRIDCOPY = GRID;
    for(int k = 0; k < it; k++)
    {
        //changed.clear();
        for(int i = 0; i < GRIDX; i++)
        {
            for(int j = 0; j < GRIDY; j++)
            {
                int alive = checkneighbours(vec2(i,j));
                NEWGRID[i][j] = (alive >= 4) ? 1 : 0;
                //if(GRID[i][j] != NEWGRID[i][j]) changed.push_back({i,j});
            }
        }
        GRID.swap(NEWGRID);
    }
    for(int i = 0; i < GRIDX; i++)
    {
        for(int j = 0; j < GRIDY; j++)
        {
            if(GRID[i][j] != GRIDCOPY[i][j]) changed.push_back({i,j});
        }
    }
    //fill smooth buffer
    for(auto i : changed)
    {
        if(GRID[i.first][i.second] == 1) setfillstyle(SOLID_FILL,WHITE);
        else setfillstyle(SOLID_FILL, BLACK);
        bar(i.first*CELLSIZE, i.second*CELLSIZE, (i.first+1)*CELLSIZE, (i.second+1)*CELLSIZE);
    }
    changed.clear();
}

void gameOfLife()
{
    //generate new grid
    for(int i = 0; i < GRIDX; i++)
    {
        for(int j = 0; j < GRIDY; j++)
        {
            int alive = checkneighbours(vec2(i,j));
            NEWGRID[i][j] = (alive == 3 || (GRID[i][j]==1 && alive==2)) ? 1 : 0;
            if(GRID[i][j] != NEWGRID[i][j]) changed.push_back({i,j});
        }
    }
    GRID.swap(NEWGRID);
    //fill new buffer
    for(auto i : changed)
    {
        if(GRID[i.first][i.second] == 1) setfillstyle(SOLID_FILL,WHITE);
        else setfillstyle(SOLID_FILL, BLACK);
        bar(i.first*CELLSIZE, i.second*CELLSIZE, (i.first+1)*CELLSIZE, (i.second+1)*CELLSIZE);
    }
    changed.clear();
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

    GRID.assign(GRIDX,vector<uint8_t>(GRIDY,0));
    NEWGRID.assign(GRIDX,vector<uint8_t>(GRIDY,0));
    GRIDCOPY = GRID;

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
                    runfunc(fillnoise,NOISE_ITERATIONS);
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