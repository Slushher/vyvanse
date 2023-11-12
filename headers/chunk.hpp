#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "block.hpp"
#include <mesh.hpp>

class Chunk
{
    struct Direction
    {
        int x, y, z;
    };
    const Direction FRONT = {0, 0, -1};
    const Direction BACK = {0, 0, 1};
    const Direction LEFT = {-1, 0, 0};
    const Direction RIGHT = {1, 0, 0};
    const Direction TOP = {0, 1, 0};
    const Direction BOTTOM = {0, -1, 0};
    Block ***m_pBlocks;
    std::vector<Texture> m_pTextures;
    std::vector<Mesh> meshes;

public:
    static const int CHUNK_SIZE = 16;
    static const int CHUNK_HEIGHT = 32;
    Chunk(int x, int z);
    ~Chunk();
    bool isBlockSurrounded(int x, int y, int z);
    void Update();
    void Render(Shader &shader);
    void RenderTransparent(Shader &shader);
    void RenderEdges(Shader &shader);
    void createMesh();
    int chunkX, chunkZ;
    bool rebuild = true;
    bool chunkVisible = false;
    int getChunkX();
    int getChunkZ();
    bool needsRebuild();
    void setNeedsRebuild(bool rebuild);
};