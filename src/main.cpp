#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <shader.h>
#include <camera.hpp>
#include <iostream>
#include <chunk.hpp>
#include <chunkmanager.hpp>

// Definitions
const float SCR_WIDTH = 1280.f;
const float SCR_HEIGHT = 720.f;

// Debugging
int success;
char infoLog[512];

glm::vec3 blockPositions[72];

// Transformatee
glm::vec4 vec(0.0f, 0.0f, 0.0f, 1.0f);
glm::mat4 trans = glm::mat4(1.0f);

// Lighting
glm::vec3 lightPos(0.f, 6.0f, 0.f);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

int chunkX = 0, chunkZ = 0;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "_floating_ Project", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetWindowPos(window, 360, 260);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    // Print OpenGL and GLSL version
    printf("Version GL: %s\n", glGetString(GL_VERSION));
    printf("Version GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Build and compile shader programs
    Shader shader("shaders/1.vs", "shaders/1.fs");
    /*for (int x = 0; x < 9; x++)
    {
        for (int z = 0; z < 9; z++)
        {
            genWorld(x, z);
            world[x][z].x=4-x;
            world[x][z].z=4-z;
        }
    }
    */
    // Vertex data
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Light VAO
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Image handling
    int image_w, image_h, image_channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("img/brick.png", &image_w, &image_h, &image_channels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_w, image_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "ERROR::TEXTURE::LOADING_FAILED\n";
    }
    stbi_image_free(data);
    shader.use();
    ChunkManager chunkmanager;
    chunkmanager.UpdateLoadList(chunkX, chunkZ);
    chunkmanager.UpdateVisibilityList(chunkX, chunkZ);
    chunkmanager.UpdateRebuildList();
    chunkmanager.RebuildChunks();
    //c1.Update();
    //c1.createMesh();
    
    /*
        std::vector<Vertex> p_blockMesh_vertices = Mesh::loadVertices(cubeVertices, 36);
        std::vector<Texture> m_pTextures;
        m_pTextures.push_back({1, "brick"});
        Mesh meshInstance(p_blockMesh_vertices, cubeIndices, m_pTextures);
    */

    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // render
        glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //shader.use();
        
        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // draw
        shader.use();
        shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("viewPos", cameraPos);
        
        const float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;

        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(fov), SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f));
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 blockTranslate = glm::vec3(0.f, 0.f, 1.f);
        //model = glm::translate(model, blockTranslate);
        //shader.setMat4("model", model);
        //c1.Render(shader);
        // chunk logic
        if(chunkX!=static_cast<int>(cameraPos.x)/16 || chunkZ!=static_cast<int>(cameraPos.z)/16){
            chunkX = static_cast<int>(cameraPos.x)/16;
            chunkZ = static_cast<int>(cameraPos.z)/16;
            std::cout<<"CHUNK CHANGED "<<chunkX<<" : "<<chunkZ<<"\n";
            chunkmanager.UpdateLoadList(chunkX, chunkZ);
            chunkmanager.UpdateUnloadList(chunkX, chunkZ);
            chunkmanager.UpdateRebuildList();
            chunkmanager.UpdateVisibilityList(chunkX, chunkZ);
            chunkmanager.RebuildChunks();
        }
        chunkmanager.renderChunks(shader);

        /*for (int i = 0; i < 81; i++)
        {
            for (int x = 0; x < chunk_x; x++)
            {
                for (int y = 0; y < chunk_y; y++)
                {
                    for (int z = 0; z < chunk_z; z++)
                    {
                        if (world[i/9][i%9].chunk[x][y][z] == 1)
                        {
                            glm::vec3 chunkTranslate = glm::vec3(world[i/9][i%9].x*16.f,0.f,world[i/9][i%9].z*16.f);
                            glm::vec3 blockTranslate = glm::vec3((0.f + 1.f * x), (-10.f + 1.f * y), (0.f + 1.f * z));
                            glm::mat4 model = glm::mat4(1.0f);
                            model = glm::translate(model, blockTranslate+chunkTranslate);
                            shader.setMat4("model", model);
                            glDrawArrays(GL_TRIANGLES, 0, 36);
                        }
                    }
                }
            }
        }
        /*if (chunkLast_x != (int)(cameraPos.x / 16) || chunkLast_z != (int)(cameraPos.z / 16))
        {
            chunkLast_x = (cameraPos.x / 16);
            chunkLast_z = (cameraPos.z / 16);
            std::cout << "Chunk changed X:" << chunkLast_x << " Z:" << chunkLast_z << " \n";
        }
        */

        // check and call events and swap the buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

