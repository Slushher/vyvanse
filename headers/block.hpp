#pragma once

#include <glm/glm.hpp>

enum BlockType
{
    Air = 0,
    Brick
};

class Block
{
    bool m_visible;
    BlockType block_type;
public:
    Block();
    ~Block();
    bool isVisible();
    void setVisible(bool visible);
};