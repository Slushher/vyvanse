#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <block.hpp>
#include <chunkmanager.hpp>

class Player
{
    std::vector<BlockType> inventory;
    glm::vec3 rayOrigin;
    glm::vec3 rayDirection;
    static const int CHUNK_SIZE = 16;
    static const int CHUNK_HEIGHT = 32;
    float MAX_RAY_DISTANCE = 5.f;
    float RAY_STEP = 0.1;
public:
    void destroyBlock(ChunkManager &chunkmanager);
    void placeBlock();
};