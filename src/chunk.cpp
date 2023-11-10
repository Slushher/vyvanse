#include <chunk.hpp>
#include <cube.h>
#include <perlin.hpp>

Chunk::Chunk(int x, int z)
{
    const siv::PerlinNoise::seed_type seed = 123456u;
    const siv::PerlinNoise perlin{seed};
    this->chunkX = x;
    this->chunkZ = z;
    this->chunkVisible = true;
    m_pBlocks = new Block **[CHUNK_SIZE];
    for (int i = 0; i < CHUNK_SIZE; i++)
    {
        m_pBlocks[i] = new Block *[CHUNK_SIZE];
        for (int j = 0; j < CHUNK_SIZE; j++) // used to be j
        {
            m_pBlocks[i][j] = new Block[CHUNK_SIZE];
            for (int k = 0; k < CHUNK_SIZE; k++) // used to be k
            {
                const double noise = perlin.octave2D_01((chunkX * 16 + i) * 0.05, (chunkZ * 16 + k) * 0.05, 2);
                if (noise >= static_cast<double>(j) / CHUNK_SIZE)
                {
                    if (j < CHUNK_SIZE/2)
                    {
                        m_pBlocks[i][j][k].setBlockType(Brick);
                    }
                    else
                    {
                        m_pBlocks[i][j][k].setBlockType(Grass);
                    }
                }
                else
                {
                    m_pBlocks[i][j][k].setBlockType(Air);
                }
            }
        }
    }
    m_pTextures.push_back({1, "Brick"});
    m_pTextures.push_back({2, "Grass"});
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

bool Chunk::isBlockSurrounded(int x, int y, int z)
{
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            for (int dz = -1; dz <= 1; ++dz)
            {
                int nx = x + dx;
                int ny = y + dy;
                int nz = z + dz;

                // Skip the center block
                if (dx == 0 && dy == 0 && dz == 0)
                {
                    continue;
                }

                // Check if the neighboring block is within chunk bounds
                if (nx >= 0 && nx < CHUNK_SIZE &&
                    ny >= 0 && ny < CHUNK_SIZE &&
                    nz >= 0 && nz < CHUNK_SIZE)
                {
                    // Check if the neighboring block is empty or not obstructing
                    if (m_pBlocks[nx][ny][nz].getBlockType() != Air)
                    {
                        // Neighboring block is not empty, target block is not surrounded
                        return false;
                    }
                }
                // If neighboring block is outside the chunk, consider it surrounded
            }
        }
    }
    // All surrounding blocks are either empty or within the chunk, target block is surrounded
    return true;
}

void Chunk::Update()
{
    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                if (!isBlockSurrounded(x, y, z))
                {
                    m_pBlocks[x][y][z].setVisible(false);
                }
                else
                {
                    m_pBlocks[x][y][z].setVisible(true);
                }
            }
        }
    }
}
void Chunk::Render(Shader &shader)
{
    for (int i = 0; i < meshes.size(); i++)
    {
        shader.use();
        glm::mat4 model = glm::mat4(1.0f);

        // Calculate translation for each cube based on its position in the chunk
        int x = i % CHUNK_SIZE;
        int y = (i / CHUNK_SIZE) % CHUNK_SIZE;
        int z = (i / (CHUNK_SIZE * CHUNK_SIZE)) % CHUNK_SIZE;

        glm::vec3 blockTranslate = glm::vec3(x + chunkX * CHUNK_SIZE, y, z + chunkZ * CHUNK_SIZE);
        model = glm::translate(model, blockTranslate);

        shader.setMat4("model", model);
        meshes[i].Draw(shader);
    }
}

void Chunk::createMesh()
{
    meshes.clear();
    for (int i = 1; i <= Grass; i++)
    {
        std::vector<Vertex> p_blockMesh_vertices;
        std::vector<unsigned int> cubeIndices;
        // Calculate the starting position of the chunk in the world coordinates
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            for (int y = 0; y < CHUNK_SIZE; y++)
            {
                for (int z = 0; z < CHUNK_SIZE; z++)
                {
                    if (m_pBlocks[x][y][z].getBlockType() == i)
                    {
                        //std::cout<<"Block type : "<<m_pBlocks[x][y][z].getBlockType()<<"\n";
                        // Calculate the translation for the current block within the chunk
                        glm::vec3 blockTranslate = glm::vec3(x, y, z);

                        // Apply translation to vertices
                        for (size_t i = 0; i < 36; ++i)
                        {
                            Vertex vertex;
                            vertex.Position = glm::vec3(cubeVertices[i * 8], cubeVertices[i * 8 + 1], cubeVertices[i * 8 + 2]) + blockTranslate;
                            vertex.Normal = glm::vec3(cubeVertices[i * 8 + 3], cubeVertices[i * 8 + 4], cubeVertices[i * 8 + 5]);
                            // vertex.TexCoords = glm::vec2(cubeVertices[i * 8 + 6], cubeVertices[i * 8 + 7]);
                            //std::cout << m_pBlocks[x][y][z].getBlockType() << "\n";
                            vertex.TexCoords = glm::vec2(cubeVertices[i * 8 + 6], cubeVertices[i * 8 + 7]);
                            p_blockMesh_vertices.push_back(vertex);
                        }

                        // Adjust indices for the current block
                        for (size_t i = 0; i < 36; ++i)
                        {
                            cubeIndices.push_back(i + p_blockMesh_vertices.size() - 36);
                        }
                    }
                }
            }
        }

        Mesh mesh(p_blockMesh_vertices, cubeIndices, m_pTextures);
        meshes.push_back(mesh);
    }
    bool rebuild = true;
}

int Chunk::getChunkX()
{
    return this->chunkX;
}
int Chunk::getChunkZ()
{
    return this->chunkZ;
}

bool Chunk::needsRebuild()
{
    return this->rebuild;
}

void Chunk::setNeedsRebuild(bool rebuild)
{
    this->rebuild = rebuild;
}

/*Chunk* Chunk::getNeighboringChunk(int x, int y, int z, int offsetX, int offsetY, int offsetZ)
{
    // Calculate the coordinates of the neighboring chunk
    int chunkX = x / CHUNK_SIZE + offsetX;
    int chunkY = y / CHUNK_SIZE + offsetY;
    int chunkZ = z / CHUNK_SIZE + offsetZ;

    // Iterate through the chunk vector and find the neighboring chunk
    for (Chunk* chunk : chunkVector)
    {
        // Check if the chunk matches the calculated coordinates
        if (chunk->getX() == chunkX && chunk->getY() == chunkY && chunk->getZ() == chunkZ)
        {
            // Return a pointer to the neighboring chunk
            return chunk;
        }
    }

    // Neighboring chunk does not exist
    return nullptr;
}*/