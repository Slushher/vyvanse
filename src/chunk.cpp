#include <chunk.hpp>
#include <cube.h>
#include <perlin.hpp>

Chunk::Chunk(int x, int z)
{
    const siv::PerlinNoise::seed_type seed = 1234u;
    const siv::PerlinNoise perlin{seed};
    double perlin_multiplier = 0.04;
    this->chunkX = x;
    this->chunkZ = z;
    this->chunkVisible = true;
    m_pBlocks = new Block **[CHUNK_SIZE];
    for (int i = 0; i < CHUNK_SIZE; i++)
    {
        m_pBlocks[i] = new Block *[CHUNK_HEIGHT];
        for (int j = 0; j < CHUNK_HEIGHT; j++) // used to be j
        {
            m_pBlocks[i][j] = new Block[CHUNK_SIZE];
            for (int k = 0; k < CHUNK_SIZE; k++) // used to be k
            {
                const double noise = perlin.octave2D_01((chunkX * 16 + i) * perlin_multiplier, (chunkZ * 16 + k) * perlin_multiplier, 2);
                if (noise >= static_cast<double>(j) / CHUNK_HEIGHT)
                {
                    if (j < CHUNK_HEIGHT / 2)
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
    for (int i = 0; i < CHUNK_SIZE; i++)
    {
        for (int j = 12; j >= 0; j--)
        {
            for (int k = 0; k < CHUNK_SIZE; k++){
                if(!m_pBlocks[i][j][k].getBlockType()){
                    m_pBlocks[i][j][k].setBlockType(Water);
                }
            }
        }
    }
    m_pTextures.push_back({1, "brick"});
    m_pTextures.push_back({2, "grass"});
    m_pTextures.push_back({3, "water"});
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

bool Chunk::isBlockExposedToAir(int x, int y, int z, Direction face, std::vector<Chunk *> chunkNeighbors)
{
    int nx = x + face.x;
    int ny = y + face.y;
    int nz = z + face.z;

    if (nx >= 0 && nx < CHUNK_SIZE &&
        ny >= 0 && ny < CHUNK_HEIGHT &&
        nz >= 0 && nz < CHUNK_SIZE)
    {
        if (m_pBlocks[nx][ny][nz].getBlockType() == Water && m_pBlocks[x][y][z].getBlockType() != Water)
        {
            return true;
        }
        else if (m_pBlocks[nx][ny][nz].getBlockType() == Air)
        {
            return true;
        }
    }
    for (Chunk *neighbor : chunkNeighbors)
    {
        int neighborX = x + face.x;
        int neighborY = y + face.y;
        int neighborZ = z + face.z;

        if (neighborX >= 0 && neighborX < CHUNK_SIZE &&
            neighborY >= 0 && neighborY < CHUNK_HEIGHT &&
            neighborZ >= 0 && neighborZ < CHUNK_SIZE)
        {
            if (neighbor->m_pBlocks[neighborX][neighborY][neighborZ].getBlockType() == Water && m_pBlocks[x][y][z].getBlockType() != Water)
            {
                return true;
            }
            else if (neighbor->m_pBlocks[neighborX][neighborY][neighborZ].getBlockType() == Air)
            {
                return true;
            }
        }
    }
    return false;
}

void Chunk::Update()
{
    /*for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < CHUNK_HEIGHT; y++)
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
    }*/
}
void Chunk::Render(Shader &shader)
{
    for (int i = 0; i < 2; i++) // i < meshes.size()
    {
        shader.use();
        glm::mat4 model = glm::mat4(1.0f);

        // Calculate translation for each cube based on its position in the chunk
        int x = i % CHUNK_SIZE;
        int y = (i / CHUNK_HEIGHT) % CHUNK_HEIGHT;
        int z = (i / (CHUNK_SIZE * CHUNK_SIZE)) % CHUNK_SIZE;

        glm::vec3 blockTranslate = glm::vec3(x + chunkX * CHUNK_SIZE, y, z + chunkZ * CHUNK_SIZE);
        model = glm::translate(model, blockTranslate);

        shader.setMat4("model", model);
        shader.setInt("texture1", i);
        shader.setFloat("transparency", 1.0f);
        meshes[i].Draw(shader);
        //glDisable(GL_BLEND);
    }
}

void Chunk::RenderTransparent(Shader &shader)
{
    for (int i = 2; i < meshes.size(); i++)
    {
        shader.use();
        glm::mat4 model = glm::mat4(1.0f);

        // Calculate translation for each cube based on its position in the chunk
        int x = i % CHUNK_SIZE;
        int y = (i / CHUNK_HEIGHT) % CHUNK_HEIGHT;
        int z = (i / (CHUNK_SIZE * CHUNK_SIZE)) % CHUNK_SIZE;

        glm::vec3 blockTranslate = glm::vec3(x + chunkX * CHUNK_SIZE, y, z + chunkZ * CHUNK_SIZE);
        model = glm::translate(model, blockTranslate);

        shader.setMat4("model", model);
        shader.setInt("texture1", i);
        shader.setFloat("transparency", 0.3f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        meshes[i].Draw(shader);
        glDisable(GL_BLEND);
    }
}

void Chunk::RenderEdges(Shader &shader)
{
    /*shader.use();
    glm::mat4 model = glm::mat4(1.0f);

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    glm::vec3 blockTranslate = glm::vec3(x + chunkX * CHUNK_SIZE, y, z + chunkZ * CHUNK_SIZE);
    model = glm::translate(model, blockTranslate);

    shader.setMat4("model", model);
    shader.setVec3("lineColor", glm::vec3(1.0f, 0.0f, 0.0f)); // Red color

    // Set the line width to 3 pixels
    glLineWidth(6.0f);

    // Create a VAO and VBO for the line
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    // Vertices for the line
    float lineVertices[] = {
        chunkX * CHUNK_SIZE + x, 0.0f, chunkZ * CHUNK_SIZE + z,
        chunkX * CHUNK_SIZE + x, CHUNK_HEIGHT, chunkZ * CHUNK_SIZE + z};

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Draw the line
    glDrawArrays(GL_LINES, 0, 2);

    // Clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // Reset line width
    glLineWidth(1.0f);
    // Reset color to white
    shader.setVec3("lineColor", glm::vec3(1.0f, 1.0f, 1.0f));

    // Reset other OpenGL states
    glBindVertexArray(0);*/
}

void Chunk::createMesh(std::vector<Chunk *> chunkNeighbors)
{
    meshes.clear();
    std::vector<Vertex> p_blockMesh_vertices;
    std::vector<unsigned int> cubeIndices;
    for (int block_type = Brick; block_type <= Water; block_type++)
    {
        p_blockMesh_vertices.clear();
        cubeIndices.clear();
        // Calculate the starting position of the chunk in the world coordinates
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            for (int y = 0; y < CHUNK_HEIGHT; y++)
            {
                for (int z = 0; z < CHUNK_SIZE; z++)
                {
                    if (m_pBlocks[x][y][z].getBlockType() == block_type)
                    {
                        // Calculate the translation for the current block within the chunk
                        glm::vec3 blockTranslate = glm::vec3(x, y, z);
                        int iterator = 0;
                        for (const Direction &face : ALL_DIRECTIONS)
                        {
                            if (isBlockExposedToAir(x, y, z, face, chunkNeighbors))
                            {
                                // Apply translation to vertices
                                for (size_t i = 0; i < 6; ++i)
                                {
                                    Vertex vertex;
                                    size_t baseIndex = (iterator * 6 + i) * 8;
                                    vertex.Position = glm::vec3(cubeVertices[baseIndex], cubeVertices[baseIndex + 1], cubeVertices[baseIndex + 2]) + blockTranslate;
                                    vertex.Normal = glm::vec3(cubeVertices[baseIndex + 3], cubeVertices[baseIndex + 4], cubeVertices[baseIndex + 5]);
                                    vertex.TexCoords = glm::vec2(cubeVertices[baseIndex + 6], cubeVertices[baseIndex + 7]);
                                    p_blockMesh_vertices.push_back(vertex);
                                }
                                // Adjust indices for the current block
                                for (size_t i = 0; i < 6; ++i)
                                {
                                    cubeIndices.push_back(i + p_blockMesh_vertices.size() - 6);
                                }
                            }
                            iterator++;
                        }
                    }
                }
            }
        }
        Mesh mesh(p_blockMesh_vertices, cubeIndices, m_pTextures);
        meshes.push_back(mesh);
    }
    // printf("Meshes built %i\n", meshes.size());
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