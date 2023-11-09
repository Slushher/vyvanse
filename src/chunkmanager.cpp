#include <chunkmanager.hpp>

bool ChunkManager::isChunkOutOfRenderDistance(int chunkX, int chunkZ, int playerX, int playerZ)
{
    int distanceX = std::abs(chunkX - playerX);
    int distanceZ = std::abs(chunkZ - playerZ);
    return (distanceX > RENDER_DISTANCE || distanceZ > RENDER_DISTANCE);
}

void ChunkManager::UpdateLoadList(int playerX, int playerZ)
{
    for (int xOffset = -RENDER_DISTANCE; xOffset <= RENDER_DISTANCE; ++xOffset)
    {
        for (int zOffset = -RENDER_DISTANCE; zOffset <= RENDER_DISTANCE; ++zOffset)
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
        chunk->Update(); // Assuming Chunk::Update() updates the visibility flag or any other relevant data

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
    for (Chunk *chunk : chunkVector)
    {
        chunk->Render(shader);
    }
}

void ChunkManager::RebuildChunks()
{
    for (Chunk *chunk : chunksToRebuild)
    {
        chunk->createMesh();
        chunk->setNeedsRebuild(false);
    }

    chunksToRebuild.clear();
}
