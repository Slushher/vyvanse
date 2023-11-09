#pragma once
#include <math.h>
#include <vector>
#include <chunk.hpp>
#include <algorithm>

class ChunkManager{
    std::vector<Chunk*> chunkVector;
    std::vector<Chunk*> chunksToRebuild;
    std::vector<Chunk*> visibleChunks;
public:
    const int RENDER_DISTANCE = 2;
    void UpdateLoadList(int playerX, int playerZ);
    void UpdateRebuildList();
    void UpdateUnloadList(int playerX, int playerZ);
    void UpdateVisibilityList(int playerX, int playerZ);
    bool isChunkOutOfRenderDistance(int chunkX, int chunkY, int playerX, int playerY);
    void renderChunks(Shader &shader);
    void RebuildChunks();
};