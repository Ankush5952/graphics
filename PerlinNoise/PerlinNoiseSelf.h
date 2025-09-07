#ifndef PERLINENOISESELF
#define PERLINENOISESELF
#pragma once
#include<vector>
using namespace std;

class PerlinNoiseSelf
{
private:
    vector<int> perm; //pseudo random number vector - 2x size for wrapping handling
public:
    PerlinNoiseSelf(unsigned int seed = 0);
    double noise(double x, double y);
    double fractalNoise(double x, double y, int octaves, double frequency, double persistence);

    ~PerlinNoiseSelf() = default;
};
#endif
