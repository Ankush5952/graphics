#include<graphics.h>
#include<conio.h>
#include "includes.cpp"

const int WIDTH = 800;
const int HEIGHT = 700;
const char* TITLE = "PROCEDURAL GENERATION";

const int MINM = 0, MAXM = 101;
const uint8_t SEA = 30%MAXM;
const uint8_t SAND = 40%MAXM;
const uint8_t DIRT = 55%MAXM;
const uint8_t GRASS = 80%MAXM;
const uint8_t SNOW = 100%MAXM;

const int CELLSIZE = 4;
const int GRIDX = WIDTH/CELLSIZE;
const int GRIDY = HEIGHT/CELLSIZE;

//using uint_8 instead of int to prevent overhead
vector<vector<uint8_t>> GRID; 
vector<vector<uint8_t>> NEWGRID;

//Random generator stuff - just keep it as is
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int> dist(MINM, MAXM-1);

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

//Return the average height value according to neighbours
int neighbouravg(vector<vector<uint8_t>>& grid, int i, int j)
{
    int neighboursum = 0, numneighbours = 0;
    for(int dx = -1; dx <= 1; dx++)
    {
        for(int dy = -1; dy <= 1; dy++)
        {
            if(dx == 0 && dy == 0) continue;
            int nx = i+dx, ny = j+dy;
            if(nx >= 0 && nx < GRIDX && ny >= 0 && ny < GRIDY) 
                neighboursum += grid[nx][ny], numneighbours ++;
        }
    }
    int avg = neighboursum/numneighbours;
    return avg;
}

//flood fill + procedural noise
void generateNoiseMap(vector<vector<uint8_t>>& grid, int it)
{
    //generate random buffer
    for(int i = 0; i < GRIDX; i++)
    {
        for(int j = 0; j < GRIDY; j++)
        {
            grid[i][j] = dist(gen); // minm to maxm(0-100)
        }
    }

    //smooth buffer
    for(int k = 0; k < it; k++)
    {
        for(int i = 0; i < GRIDX; i++)
        {
            for(int j = 0; j < GRIDY; j++)
            {
                NEWGRID[i][j] = neighbouravg(grid,i, j);
            }
        }
        grid.swap(NEWGRID);
    }
}
void generateOctaves()
{
    //OCTAVE NOISEMAPS;
    vector<vector<uint8_t>> big(GRIDX,vector<uint8_t>(GRIDY,0)); //large stuff
    vector<vector<uint8_t>> med(GRIDX,vector<uint8_t>(GRIDY,0)); //medium features
    vector<vector<uint8_t>> smol(GRIDX,vector<uint8_t>(GRIDY,0)); //finer detailing

    generateNoiseMap(big, 8); //huge changes
    generateNoiseMap(med, 4); //cave-like structure
    generateNoiseMap(smol, 1); //finer changes

    float wb = 0.6, wm = 0.3, ws = 0.1; //Weights of Octaves

    //Merge Noisemaps according to weights
    for(int i = 0; i < GRIDX; i++)
    {
        for(int j = 0; j < GRIDY; j++)
        {
            float val = wb*big[i][j] + wm*med[i][j] + ws*smol[i][j];
            val = (val / (wb + wm + ws));
            GRID[i][j] = (uint8_t)val;
        }
    }
}

void fillnoise()
{
    generateOctaves();
    //find minm and maxm values in GRID
    uint8_t minval = 255, maxval = 0;
    for(int i = 0; i < GRIDX; i++)
    {
        for(int j = 0; j < GRIDY; j++)
        {
            minval = min(minval, GRID[i][j]);
            maxval = max(maxval, GRID[i][j]);
        }
    }

    for(int i = 0; i < GRIDX; i++)
    {
        for(int j = 0; j < GRIDY; j++)
        {
            //adjust the grid
            GRID[i][j] = (uint8_t)( ( ( GRID[i][j] - minval )*100 )/(float)( maxval - minval ) );
            //set values
            uint8_t val = GRID[i][j];
            int color = BLACK;
            if(val < SEA) color = BLUE;
            else if(val < SAND) color = YELLOW;
            else if(val < DIRT) color = BROWN;
            else if(val < GRASS) color = GREEN;
            else if(val < SNOW) color = WHITE;
            
            //draw cell
            setfillstyle(SOLID_FILL, color);
            bar(i*CELLSIZE, j*CELLSIZE, (i+1)*CELLSIZE, (j+1)*CELLSIZE);
        }
    }
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
                    runfunc(fillnoise);
                    break;
                
                default:
                    break;
            }
        }
    }
    closegraph();
}