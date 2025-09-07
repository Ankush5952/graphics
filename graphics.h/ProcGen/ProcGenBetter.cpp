#include<graphics.h>
#include<conio.h>
#include "includes.cpp"
//TODO : IMPLEMENT STRUCTS FOR SCALABLE GROUND VARIETY AND MAKE FILLNOISE COMPATIBLE WITH IT.
struct Terrain
{
    int color;
    uint8_t threshold;
};

const vector<Terrain> normal_terrain = {
    {BLUE, 30},//sea
    {YELLOW, 40},//sand
    {BROWN, 50},//dirt
    {GREEN, 80},//grass
    {WHITE, 100},//snow
};
const vector<Terrain> dessert = {
    {BLUE, 15},//sea
    {COLOR(241,219,167), 45},//sand
    {COLOR(238,180,111), 80},//dark sand
    {COLOR(153,104,83), 100},//reddish hue
};
const vector<Terrain> cold_ocean = {
    {COLOR(94,179,216), 20},
    {COLOR(168,214,236),30},
    {COLOR(51,106,143), 55},
    {COLOR(19,71,112), 80},
    {COLOR(4,30,65),100}
};
const vector<Terrain> mountain = {
    {COLOR(19,30,50), 15},
    {COLOR(82,90,97), 30},
    {COLOR(93,102,114), 60},
    {COLOR(236,227,216),75},
    {COLOR(97,102,115), 90},
    {COLOR(141,138,138), 100}
};
const vector<Terrain> forest = {
    {COLOR(93,138,75), 20},
    {COLOR(50,86,37), 40},
    {COLOR(24,57,25), 65},
    {COLOR(11,35,4), 80},
    {COLOR(7,22,9), 95}
};

const int WIDTH = 1360;
const int HEIGHT = 765;
const char* TITLE = "PROCEDURAL GENERATION";

const int MINM = 0, MAXM = 101;

const int CELLSIZE = 4;
const int GRIDX = WIDTH/CELLSIZE;
const int GRIDY = HEIGHT/CELLSIZE;

//using uint_8 instead of int to prevent overhead
vector<vector<uint8_t>> GRID; 
vector<vector<uint8_t>> NEWGRID;

//OCTAVE NOISEMAPS;
vector<vector<uint8_t>> BIG(GRIDX,vector<uint8_t>(GRIDY,0)); //large stuff
vector<vector<uint8_t>> MED(GRIDX,vector<uint8_t>(GRIDY,0)); //medium features
vector<vector<uint8_t>> SMOL(GRIDX,vector<uint8_t>(GRIDY,0)); //finer detailing

//Random generator stuff - just keep it as is
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int> dist(MINM, MAXM-1);

//Input Queue prevention
bool busy = false;

//Color Storage
int current_color = BLACK;
#define ENTER '\r'
#define SPACE ' '

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
                NEWGRID[i][j] = avg;
            }
        }
        grid.swap(NEWGRID);
    }
}

void generateOctaves()
{
    //generate noisemaps
    generateNoiseMap(BIG, 8); //huge changes
    generateNoiseMap(MED, 4); //cave-like structure
    generateNoiseMap(SMOL, 1); //finer changes

    float wb = 0.6, wm = 0.3, ws = 0.1; //Weights of Octaves

    //Merge Noisemaps according to weights
    uint8_t minval = 255, maxval = 0;
    for(int i = 0; i < GRIDX; i++)
    {
        for(int j = 0; j < GRIDY; j++)
        {
            float val = wb*BIG[i][j] + wm*MED[i][j] + ws*SMOL[i][j];
            val = (val / (wb + wm + ws));
            GRID[i][j] = (uint8_t)val;

            minval = min(minval, GRID[i][j]);
            maxval = max(maxval, GRID[i][j]);
        }
    }

    //Convert New Grid values back to 0-100 range
    //adjust the grid
    float scale = 100/(maxval - minval);
    if(maxval == minval)
    {
        for(int i = 0; i < GRIDX; i++)
    {
        for(int j = 0; j < GRIDY; j++)
        {
            GRID[i][j] = i+j; //error
        }
    }
    }
    for(int i = 0; i < GRIDX; i++)
    {
        for(int j = 0; j < GRIDY; j++)
        {
            GRID[i][j] = (uint8_t)( ( GRID[i][j] - minval ) * scale);
        }
    }
}

array<int,101> getcolorLUP(vector<Terrain> terrains)
{
    array<int,101> colorLUP = {BLACK};
    for(int v = 0; v < 101; v++)
    {
        for(auto& i : terrains)
        {
            if(v < i.threshold) { colorLUP[v] = i.color; break; }
        }
    }
    //error
    return colorLUP;
}

void fillnoise(vector<Terrain> terrains)
{
    generateOctaves();

    array<int,101> colors = getcolorLUP(terrains);

    for(int i = 0; i < GRIDX; i++)
    {
        for(int j = 0; j < GRIDY; j++)
        {
            uint8_t val = GRID[i][j];
            int color = colors[val];

            if(color != current_color) setfillstyle(SOLID_FILL, color);
            current_color = color;
            bar(i*CELLSIZE, j*CELLSIZE, (i+1)*CELLSIZE, (j+1)*CELLSIZE);
        }
    }
}

template<typename Func, typename... Args>
void runfunc( Func func, Args... args )
{
    busy = true;
    auto start = chrono::high_resolution_clock::now();
    func(args...);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end-start).count();
    while(kbhit()) getch();
    busy = false;
    char time[32];
    sprintf(time, "%d ms",duration);
    outtextxy(0, 0, time);
}

int main()
{
    GRID.assign(GRIDX, vector<uint8_t>(GRIDY,0));
    NEWGRID.assign(GRIDX, vector<uint8_t>(GRIDY,0));
    BIG.assign(GRIDX, vector<uint8_t>(GRIDY,0));
    MED.assign(GRIDX, vector<uint8_t>(GRIDY,0));
    SMOL.assign(GRIDX, vector<uint8_t>(GRIDY,0));

    initwindow(WIDTH, HEIGHT, TITLE);
    while (true)
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
                case '1':
                    runfunc(fillnoise, normal_terrain);
                    break;
                case '2':
                    runfunc(fillnoise, cold_ocean);
                    break;
                case '3':
                    runfunc(fillnoise, dessert);
                    break;
                case '4':
                    runfunc(fillnoise, mountain);
                    break;
                case '5':
                    runfunc(fillnoise, forest);
                    break;
                default:
                    break;
            }
        }
    }

    getch();
    closegraph();
}