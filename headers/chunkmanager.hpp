#pragma once
#include <math.h>
#include <vector>
#include <chunk.hpp>
#include <algorithm>

const chunkDirection CHUNK_LEFT = {-1, 0};
const chunkDirection CHUNK_RIGHT = {1, 0};
const chunkDirection CHUNK_FORWARD = {0, 1};
const chunkDirection CHUNK_BACKWARD = {0, -1};

class ChunkManager{
    std::vector<Chunk*> chunkVector;
    std::vector<Chunk*> chunksToRebuild;
    std::vector<Chunk*> visibleChunks;
public:
    const std::vector<chunkDirection> CHUNK_DIRECTIONS = {CHUNK_LEFT, CHUNK_RIGHT, CHUNK_FORWARD, CHUNK_BACKWARD};
    const int RENDER_DISTANCE = 4;
    const int PRELOAD_DISTANCE = 1;
    void UpdateLoadList(int playerX, int playerZ);
    void UpdateRebuildList();
    void UpdateUnloadList(int playerX, int playerZ);
    void UpdateVisibilityList(int playerX, int playerZ);
    bool isChunkOutOfRenderDistance(int chunkX, int chunkY, int playerX, int playerY);
    bool isWithinBounds(int x, int z);
    void renderChunks(Shader &shader); // Shader &shader
    void renderEdges(Shader &shader);
    void RebuildChunks();
    Chunk *getChunk(int x, int z);
    chunkDirection getChunkDirection(const chunkDirection& face); // im really desperate
};