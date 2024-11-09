#define STB_IMAGE_IMPLEMENTATION

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

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
#include <player.hpp>
#include <texture.hpp>
#include <sprite.hpp>
//#include <hud.hpp>

// Definitions
const float SCR_WIDTH = 1280.f;
const float SCR_HEIGHT = 720.f;

// Debugging
int success;
char infoLog[512];
bool freeCursor = false;
bool debugging = false;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
int fps = 0;

// Lighting
glm::vec3 lightPos(0.f, 6.0f, 0.f);

// Shaders
//std::vector<Shader> shaders;

int chunkX = 0, chunkZ = 0;

int image_load(const char image_path[])
{
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
    if (debugging)
    {
        printf("Loaded a image, path:%s, res:%ix%i, channels: %i\n", image_path, image_w, image_h, image_channels);
    }
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
    //glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
    glfwWindowHint(GLFW_REFRESH_RATE, 144);
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "_floating_ Project", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!freeCursor)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // GLFW_CURSOR_DISABLED
    }
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    if (debugging)
    {
        // Print OpenGL and GLSL version
        printf("Version GL: %s\n", glGetString(GL_VERSION));
        printf("Version GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    }
    
    /*GLuint texture = LoadTexture("img/brick.png");

    if (texture == 0) {
        std::cerr << "Texture loading failed!" << std::endl;
        return -1;
    }*/

    // Build and cowhileader("shaders/water/water.vs", "shaders/water/water.fs");
    Shader edges_shader("shaders/edges/edges.vs", "shaders/edges/edges.fs");

    //  Light VAO
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    unsigned int brick, grass, water;
    int image_w, image_h, image_channels;
    brick = image_load("img/stone.png");
    grass = image_load("img/dirt.png");
    water = image_load("img/water.png");

    unsigned int dirt = image_load("img/grass.png");
    ChunkManager chunkmanager;
    chunkmanager.UpdateLoadList(chunkX, chunkZ);
    chunkmanager.UpdateVisibilityList(chunkX, chunkZ);
    chunkmanager.UpdateRebuildList();
    chunkmanager.RebuildChunks();
    Player player;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    while (!glfwWindowShouldClose(window))
    {
        // fps counter
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        fps = 1 / deltaTime;

        // input
        processInput(window, chunkmanager, player);
        // render
        glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // draw
        Shader shader("shaders/1.vs", "shaders/1.fs");
        shader.use();
        shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader.setVec3("lightPos", cameraPos * glm::vec3(0.f, 10.f, 0.f));
        shader.setVec3("viewPos", cameraPos);

        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(fov), SCR_WIDTH / SCR_HEIGHT, 0.1f, 256.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f));
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        edges_shader.use();
        edges_shader.setMat4("projection", projection);
        edges_shader.setMat4("view", view);
        // chunk logic
        if (chunkX != static_cast<int>(std::floor(cameraPos.x / 16.0f)) || chunkZ != static_cast<int>(std::floor(cameraPos.z / 16.0f)))
        {
            chunkX = static_cast<int>(std::floor(cameraPos.x / 16.0f));
            chunkZ = static_cast<int>(std::floor(cameraPos.z / 16.0f));
            chunkmanager.UpdateLoadList(chunkX, chunkZ);
            chunkmanager.UpdateUnloadList(chunkX, chunkZ);
            chunkmanager.UpdateRebuildList();
            chunkmanager.UpdateVisibilityList(chunkX, chunkZ);
            chunkmanager.RebuildChunks();
        }
        chunkmanager.renderChunks(shader);
        DrawSprite(dirt, -0.5f, -0.5f);
        //chunkmanager.renderEdges(edges_shader);
        ImGui::Begin("ImGUI");
        ImGui::Text("Player XYZ : %f, %f, %f", cameraPos.x, cameraPos.y, cameraPos.z);
        ImGui::Text("Player direction : %f, %f, %f", direction.x, direction.y, direction.z);
        ImGui::Text("Chunk XY : %i, %i", chunkX, chunkZ);
        ImGui::Text("FPS : %i", fps);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // check and call events and swap the buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
