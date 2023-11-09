#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "block.hpp"
#include <mesh.hpp>

class Chunk
{
    Block ***m_pBlocks;
    std::vector<Texture> m_pTextures;
    std::vector<glm::vec3> meshCoords;
    std::vector<Mesh> meshes;
public:
    static const int CHUNK_SIZE = 16;
    Chunk(int x, int z);
    ~Chunk();
    bool isBlockSurrounded(int x, int y, int z);
    void Update();
    void Render(Shader &shader);
    void createMesh();
    int chunkX, chunkZ;
    bool rebuild = true;
    bool chunkVisible = false;
    int getChunkX();
    int getChunkZ();
    bool needsRebuild();
    void setNeedsRebuild(bool rebuild);
};