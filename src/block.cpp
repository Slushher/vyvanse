#include <block.hpp>

Block::Block()
{
}
Block::~Block()
{
}
bool Block::isVisible()
{
    return this->m_visible;
}
void Block::setVisible(bool visible)
{
    this->m_visible=visible;
}