#include <chunkmanager.hpp>

bool ChunkManager::isChunkOutOfRenderDistance(int chunkX, int chunkZ, int playerX, int playerZ)
{
    int distanceX = std::abs(chunkX - playerX);
    int distanceZ = std::abs(chunkZ - playerZ);
    return (distanceX > RENDER_DISTANCE || distanceZ > RENDER_DISTANCE);
}

void ChunkManager::UpdateLoadList(int playerX, int playerZ)
{
    for (int xOffset = -RENDER_DISTANCE - PRELOAD_DISTANCE; xOffset <= RENDER_DISTANCE + PRELOAD_DISTANCE; ++xOffset)
    {
        for (int zOffset = -RENDER_DISTANCE - PRELOAD_DISTANCE; zOffset <= RENDER_DISTANCE + PRELOAD_DISTANCE; ++zOffset)
        {
            int chunkX = playerX + xOffset;
            int chunkZ = playerZ + zOffset;

            // Check if the chunk is not already loaded
            auto it = std::find_if(chunkVector.begin(), chunkVector.end(), [chunkX, chunkZ](Chunk *chunk)
                                   {
                if (chunk != nullptr) {
                    // Check if the chunk's coordinates match the current iteration
                    int chunkCoordX = chunk->getChunkX();
                    int chunkCoordZ = chunk->getChunkZ();
                    return (chunkCoordX == chunkX && chunkCoordZ == chunkZ);
                }
                return false; });

            // Load the chunk if it is not already loaded
            if (it == chunkVector.end())
            {
                Chunk *newChunk = new Chunk(chunkX, chunkZ);
                chunkVector.push_back(newChunk);
            }
        }
    }
}

void ChunkManager::UpdateUnloadList(int playerX, int playerZ)
{
    auto it = std::remove_if(chunkVector.begin(), chunkVector.end(), [this, playerX, playerZ](Chunk *chunk)
                             {
        if (chunk != nullptr) {
            int chunkX = chunk->getChunkX();
            int chunkZ = chunk->getChunkZ();
            return isChunkOutOfRenderDistance(chunkX, chunkZ, playerX, playerZ);
        }
        return false; });

    chunkVector.erase(it, chunkVector.end());
}

void ChunkManager::UpdateVisibilityList(int playerX, int playerZ)
{
    visibleChunks.clear(); // Clear the vector before populating it again

    for (Chunk *chunk : chunkVector)
    {
        // Update the chunk
        // chunk->Update(); // Assuming Chunk::Update() updates the visibility flag or any other relevant data
        int chunkX = chunk->getChunkX();
        int chunkZ = chunk->getChunkZ();

        // Check if the chunk is within the render distance and visible to the player
        if (!isChunkOutOfRenderDistance(chunkX, chunkZ, playerX, playerZ) && chunk->chunkVisible)
        {
            visibleChunks.push_back(chunk);
        }
    }
}

void ChunkManager::renderChunks(Shader &shader)
{
    for (Chunk *chunk : visibleChunks)
    {
        chunk->Render(shader);
    }
    for (Chunk *chunk : visibleChunks)
    {
        chunk->RenderTransparent(shader);
    }
}

void ChunkManager::renderEdges(Shader &shader)
{
    for (Chunk *chunk : chunkVector)
    {
        chunk->RenderEdges(shader);
    }
}

void ChunkManager::UpdateRebuildList()
{
    chunksToRebuild.clear();

    for (Chunk *chunk : chunkVector)
    {
        if (chunk->needsRebuild())
        {
            chunksToRebuild.push_back(chunk);
        }
    }
}

void ChunkManager::RebuildChunks()
{
    for (Chunk *chunk : chunksToRebuild)
    {
        std::vector<Chunk *> chunkNeighbors;
        for (const chunkDirection &ch : CHUNK_DIRECTIONS)
        {
            const int neighborX = chunk->getChunkX() + ch.x;
            const int neighborZ = chunk->getChunkZ() + ch.z;
            if (getChunk(neighborX, neighborZ))
            {

                auto neighborIt = std::find_if(chunkVector.begin(), chunkVector.end(),
                                               [neighborX, neighborZ](Chunk *neighbor)
                                               {
                                                   return neighbor->getChunkX() == neighborX && neighbor->getChunkZ() == neighborZ;
                                               });

                if (neighborIt != chunkVector.end())
                {
                    chunkNeighbors.push_back(*neighborIt);
                }
            }
        }
        chunk->createMesh(chunkNeighbors);
        chunk->setNeedsRebuild(false);
    }

    chunksToRebuild.clear();
}

chunkDirection ChunkManager::getChunkDirection(const chunkDirection &face)
{
    for (const auto &dir : CHUNK_DIRECTIONS)
    {
        if (dir.x == face.x && dir.z == face.z)
        {
            return dir;
        }
    }
    // If no match is found, return a default direction (you may want to handle this differently)
    return {0, 0};
}

bool ChunkManager::isWithinBounds(int x, int z)
{
    return true;
    // return x >= 0 && x < RENDER_DISTANCE && z >= 0 && z < Chunk::CHUNK_SIZE;
}

Chunk *ChunkManager::getChunk(int x, int z)
{
    for (Chunk *chunk : chunkVector)
    {
        if (chunk->getChunkX() == x && chunk->getChunkZ() == z)
        {
            return chunk;
        }
    }

    // If no matching chunk is found
    return nullptr;
}