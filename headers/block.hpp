#pragma once

#include <glm/glm.hpp>

enum BlockType
{
    Air = 0,
    Brick,
    Grass,
    Water
};

class Block
{
    bool m_visible;
    BlockType block_type;
public:
    Block() : m_visible(true), block_type(Air) {}
    ~Block() = default;
    bool isVisible();
    void setVisible(bool visible);
    void setBlockType(BlockType block_type);
    BlockType getBlockType();
};