#include <player.hpp>
#include <chunk.hpp>
#include <chunkmanager.hpp>
#include <camera.hpp>
#include <block.hpp>

void Player::destroyBlock(ChunkManager &chunkmanager){
    rayOrigin = cameraPos;
    rayDirection = cameraFront;
    for(float t = 0.f; t < MAX_RAY_DISTANCE; t+= RAY_STEP)
    {
        glm::vec3 currentPoint = rayOrigin + t * rayDirection;
        int chunkX = static_cast<int>(std::floor(currentPoint.x / CHUNK_SIZE));
        int chunkZ = static_cast<int>(std::floor(currentPoint.z / CHUNK_SIZE));
        int blockX = static_cast<int>(std::floor(currentPoint.x)) % CHUNK_SIZE;
        int blockY = static_cast<int>(std::floor(currentPoint.y)) % CHUNK_HEIGHT;
        int blockZ = static_cast<int>(std::floor(currentPoint.z)) % CHUNK_SIZE;
        Chunk* chunk = chunkmanager.getChunk(chunkX, chunkZ);
        if(chunk != nullptr && chunk->getBlockType(blockX, blockY, blockZ) != Air){
            chunk->setBlockType(blockX, blockY, blockZ, Air);
            chunk->setNeedsRebuild(true);
            break;
        }
    }
}

void Player::placeBlock(){

}