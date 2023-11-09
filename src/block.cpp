#include <block.hpp>

bool Block::isVisible()
{
    return this->m_visible;
}
void Block::setVisible(bool visible)
{
    this->m_visible=visible;
}
void Block::setBlockType(BlockType block_type){
    this->block_type = block_type;
}
BlockType Block::getBlockType(){
    return this->block_type;
}