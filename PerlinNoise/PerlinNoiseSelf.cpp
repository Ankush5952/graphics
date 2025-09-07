#include "PerlinNoiseSelf.h"
#include<random>
#include<algorithm>
#include<numeric>

//fade function, input : [0,1]
static double fade(double f) { return f*f*f*(f*(6*f - 15) + 10); }

//Linear Interpolation
static double lerp(double a, double b, double t) { return a + t*(b-a); }

//Gradient Function
static double grad(int hash, double x, double y)
{
    int h = hash & 7; // convert lowest 3 bits to 8 directions
    double u = h<4 ? x : y;
    double v = h<4 ?y : x;
    return ((h&1) ? -u : u) + ((h&2) ? -2.0*v : 2.0*v);
}

PerlinNoiseSelf::PerlinNoiseSelf(unsigned int seed)
{
    perm.resize(512);
        vector<int> p(256);
        iota(p.begin(), p.end(), 0);

        mt19937 gen(seed);
        shuffle(p.begin(), p.end(), gen);

        for(int i = 0; i < 256; i++)
        {
            perm[i] = p[i]; perm[i + 256] = p[i];
        }
}

double PerlinNoiseSelf::noise(double x, double y)
{
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;

    double xf = x - floor(x), yf = y - floor(y); // [0,1]

    double u = fade(xf);
    double v = fade(yf);

    int aa = perm[perm[X]   + Y];
    int ab = perm[perm[X]   + Y  + 1];
    int ba = perm[perm[X+1] + Y];
    int bb = perm[perm[X+1] + Y  + 1];

    double res = lerp( 
        lerp(
            grad(aa, xf, yf),
            grad(ba, xf-1, yf), u
        ), 
        lerp(
            grad(ab, xf, yf-1),
            grad(bb, xf-1, yf-1), u
        ),
        v 
    );

    return (res + 1.0)/2.0;
}

double PerlinNoiseSelf::fractalNoise(double x, double y, int octaves, double frequency, double persistence)
{
    double total = 0;
    double amplitude = 1; //decrease exponentially from 1 to 1/(per^octave)
    double maxValue = 0; //normalization

    for(int i = 0; i < octaves; i++)
    {
        total += noise(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;

        amplitude *= persistence; // reduce amp : per = [0,1]
        frequency *= 2; //increase the freq : inc the zoom
    }
    return total / maxValue ;
}
