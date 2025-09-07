#include<graphics.h>
#include<conio.h>
#include "includes.cpp"
#include "FastNoiseLite.h"

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
const vector<Terrain> world = {
    {BLUE,45},
    {LIGHTGREEN, 50},
    {GREEN, 80},
    {WHITE, 100}
};

const static int WIDTH = 1360;
const static int HEIGHT = 765;
const static char* TITLE = "PROCEDURAL GENERATION";

const static int OCTAVES = 8;
const static double FREQUENCY = 0.05;
static unsigned int SEED = 0;

const static int CELLSIZE = 4;
const static int GRIDX = WIDTH/CELLSIZE;
const static int GRIDY = HEIGHT/CELLSIZE;

//using uint_8 instead of int to prevent overhead
static vector<vector<uint8_t>> GRID; 
static vector<vector<uint8_t>> NEWGRID;

//Input Queue prevention
static bool busy = false;

//Color Storage
static int current_color = BLACK;

#define ENTER '\r'
#define SPACE ' '

void generateNoiseMap()
{
    FastNoiseLite noise;
    noise.SetSeed(SEED ? SEED : random_device{}());
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(OCTAVES);
    noise.SetFrequency(FREQUENCY);

    for(int i = 0; i < GRIDX; i++)
    {
        for(int j = 0; j < GRIDY; j++)
        {
            float val = noise.GetNoise((float)i, (float)j);
            val = (val + 1)/2;
            val *= 100;
            GRID[i][j] = (uint8_t)val;
        }
    }
}

array<int,101> getcolorLUP(vector<Terrain> terrains)
{
    array<int,101> colorLUP = {BLACK};
    for(int v = 0; v < 101; v++)
    {
        bool assigned = false;
        for(auto& i : terrains)
        {
            if(v < i.threshold) { colorLUP[v] = i.color; assigned = true; break; }
        }
        if(!assigned) colorLUP[v] = terrains.back().color; // ensure coverage
    }
    //error
    return colorLUP;
}

void fillnoise(vector<Terrain> terrains)
{
    generateNoiseMap();

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
                case '6':
                    runfunc(fillnoise, world);
                    break;
                default:
                    break;
            }
        }
    }

    getch();
    closegraph();
}