#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "block.hpp"
#include <mesh.hpp>

struct Direction
{
    int x, y, z;
};

struct chunkDirection
{
    int x, z;
};

const Direction FRONT = {0, 0, -1};
const Direction BACK = {0, 0, 1};
const Direction LEFT = {-1, 0, 0};
const Direction RIGHT = {1, 0, 0};
const Direction TOP = {0, -1, 0};
const Direction BOTTOM = {0, 1, 0};

class Chunk
{
    Block ***m_pBlocks;
    std::vector<Texture> m_pTextures;
    std::vector<Mesh> meshes;
    const std::vector<Direction> ALL_DIRECTIONS = {FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM};
public:
    static const int CHUNK_SIZE = 16;
    static const int CHUNK_HEIGHT = 32;
    bool rebuild = true;
    bool chunkVisible = false;
    int chunkX, chunkZ;

    Chunk(int x, int z);
    ~Chunk();

    bool isBlockExposedToAir(int x, int y, int z, Direction face, std::vector<Chunk*> chunkNeighbors);
    void Update();
    void Render(Shader &shader);
    void RenderTransparent(Shader &shader);
    void RenderEdges(Shader &shader);
    void createMesh(std::vector<Chunk*> chunkNeighbors);
    int getChunkX();
    int getChunkZ();
    bool needsRebuild();
    void setNeedsRebuild(bool rebuild);
};