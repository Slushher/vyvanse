#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "block.hpp"

class Chunk
{
    Block ***m_pBlocks;

public:
    static const int CHUNK_SIZE = 16;
    Chunk();
    ~Chunk();
    bool isBlockSurrounded(int x, int y, int z);
    void Update();
    void Render();
    void createMesh();
};