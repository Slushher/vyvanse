#include <chunk.hpp>

Chunk::Chunk()
{
    m_pBlocks = new Block **[CHUNK_SIZE];
    for (int i = 0; i < CHUNK_SIZE; i++)
    {
        m_pBlocks[i] = new Block *[CHUNK_SIZE];
        for (int j = 0; j < CHUNK_SIZE; j++)
        {
            m_pBlocks[i][j] = new Block[CHUNK_SIZE];
        }
    }
}
Chunk::~Chunk()
{
    for (int i = 0; i < CHUNK_SIZE; ++i)
    {
        for (int j = 0; j < CHUNK_SIZE; ++j)
        {
            delete[] m_pBlocks[i][j];
        }
        delete[] m_pBlocks[i];
    }
    delete[] m_pBlocks;
}

bool Chunk::isBlockSurrounded(int x, int y, int z){
    if (x > 0 && m_pBlocks[x - 1][y][z].isVisible()) return false;
    if (x < CHUNK_SIZE - 1 && m_pBlocks[x + 1][y][z].isVisible()) return false;
    if (y > 0 && m_pBlocks[x][y - 1][z].isVisible()) return false;
    if (y < CHUNK_SIZE - 1 && m_pBlocks[x][y + 1][z].isVisible()) return false;
    if (z > 0 && m_pBlocks[x][y][z - 1].isVisible()) return false;
    if (z < CHUNK_SIZE - 1 && m_pBlocks[x][y][z + 1].isVisible()) return false;
    
    return true;
}

void Chunk::Update()
{
    for (size_t x = 0; x < CHUNK_SIZE; x++)
    {
        for (size_t y = 0; y < CHUNK_SIZE; y++)
        {
            for (size_t z = 0; z < CHUNK_SIZE; z++)
            {
                if(isBlockSurrounded(x,y,z)){
                    m_pBlocks[x][y][z].setVisible(false);
                }
                else{
                    m_pBlocks[x][y][z].setVisible(true);
                }
            }
        }
    }
}
void Chunk::Render() {}
void Chunk::createMesh()
{
    
}