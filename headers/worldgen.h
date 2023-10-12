#pragma once

#include <perlin.hpp>
#include <iostream>
#include <vector>

const int chunk_x = 16;
const int chunk_y = 10;
const int chunk_z = 16;

const int RENDER_DISTANCE = 4;

// Perlin noise persistence
const double multiplier = 0.3;

/*
    3D array of blocks containing block type data [int]
    int chunk_x,chunk_y,chunk_z : cartesian coordinates in 3 dimensions
    Block types as of now:
    0 - air
    1 - brick
*/
struct Chunk{
    int chunk[chunk_x][chunk_y][chunk_z];
    float x,z;
};

Chunk world[RENDER_DISTANCE*2+1][RENDER_DISTANCE*2+1];

void genChunk(int cord16_x, int cord16_z)
{
    const siv::PerlinNoise::seed_type seed = 123456u;
    const siv::PerlinNoise perlin{seed};
    for (int x = 0; x < chunk_x; x++)
    {
        for (int z = 0; z < chunk_z; z++)
        {
            const double noise = perlin.octave2D_01((x+cord16_x * multiplier), (z+cord16_z * multiplier), 1);
            for (int y = 0; y < chunk_y; y++)
            {
                if (noise > (double)(1.0/chunk_y)*y)
                {
                    world[cord16_x][cord16_z].chunk[x][y][z] = 1;
                }
                else
                {
                    world[cord16_x][cord16_z].chunk[x][y][z] = 0;
                }
            }
        }
    }
}

/*
    genWorld: current chunk that player is on
*/
void genWorld(int cord16_x, int cord16_z){
    genChunk(cord16_x, cord16_z);
}