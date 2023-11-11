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
#include <thread>

// Definitions
const float SCR_WIDTH = 1280.f;
const float SCR_HEIGHT = 720.f;

// Debugging
int success;
char infoLog[512];
bool isDebugged = false;

// Lighting
glm::vec3 lightPos(0.f, 6.0f, 0.f);

int chunkX = 0, chunkZ = 0;

int image_load(const char image_path[]){
    unsigned int texture;
    int image_w, image_h, image_channels;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Image handling
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(image_path, &image_w, &image_h, &image_channels, 0);
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
    return texture;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "_floating_ Project", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if(!isDebugged){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //GLFW_CURSOR_DISABLED
    }
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
    Shader shader("shaders/2.vs", "shaders/2.fs");
    Shader water("shaders/water/water.vs", "shaders/water/water.fs");
    // Vertex data
    /*unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    */
    // Light VAO
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    unsigned int brick, grass;
    int image_w, image_h, image_channels;
    brick = image_load("img/brick.png");
    grass = image_load("img/grass.png");
    /*
    glGenTextures(1, &brick);
    glBindTexture(GL_TEXTURE_2D, brick);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Image handling
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
    

    glGenTextures(1, &grass);
    glBindTexture(GL_TEXTURE_2D, grass);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Image handling
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("img/grass.png", &image_w, &image_h, &image_channels, 0);
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
    */

    ChunkManager chunkmanager;
    chunkmanager.UpdateLoadList(chunkX, chunkZ);
    chunkmanager.UpdateVisibilityList(chunkX, chunkZ);
    chunkmanager.UpdateRebuildList();
    chunkmanager.RebuildChunks();

    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // render
        glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // draw
        shader.use();
        shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader.setVec3("lightPos", cameraPos * glm::vec3(0.f, 10.f, 0.f));
        shader.setVec3("viewPos", cameraPos);
        
        const float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;

        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(fov), SCR_WIDTH / SCR_HEIGHT, 0.1f, 256.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f));
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // chunk logic
        if(chunkX!=static_cast<int>(cameraPos.x)/16 || chunkZ!=static_cast<int>(cameraPos.z)/16){
            chunkX = static_cast<int>(cameraPos.x)/16;
            chunkZ = static_cast<int>(cameraPos.z)/16;
            chunkmanager.UpdateLoadList(chunkX, chunkZ);
            chunkmanager.UpdateUnloadList(chunkX, chunkZ);
            chunkmanager.UpdateRebuildList();
            chunkmanager.UpdateVisibilityList(chunkX, chunkZ);
            chunkmanager.RebuildChunks();
        }
        chunkmanager.renderChunks(shader);
        // check and call events and swap the buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

