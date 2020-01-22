#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <array>

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
ShaderProgram unTextured;

glm::mat4 viewMatrix, modelMatrix, projectionMatrix, translateMatrix, rotatingMatrix;

//GLuint playerTextureID;
GLuint fireNationShipTextureID;
GLuint aangTextureID;

void RenderTriangles(const float vertices[], int vertCount) {

    modelMatrix = glm::mat4(1.0f);
    unTextured.SetModelMatrix(modelMatrix);


    glVertexAttribPointer(unTextured.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(unTextured.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, vertCount);
    glDisableVertexAttribArray(unTextured.positionAttribute);
}

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}

void RenderTextures() {
    
    program.SetModelMatrix(rotatingMatrix);

    float aangVertices[] = { -0.75, -0.75, 0.75, -0.75, 0.75, 0.75, -0.75, -0.75, 0.75, 0.75, -0.75, 0.75};
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, aangTextureID);

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, aangVertices);
    glEnableVertexAttribArray(program.positionAttribute);

    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    program.SetModelMatrix(translateMatrix);

    float shipVertices[] = { -5.0, -1.0, 0.0, -1.0, 0.0, 1.0, -5.0, -1.0, 0.0, 1.0, -5.0, 1.0};

    glBindTexture(GL_TEXTURE_2D, fireNationShipTextureID);

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, shipVertices);
    glEnableVertexAttribArray(program.positionAttribute);

    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("TwoDScene", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    translateMatrix = glm::mat4(1.0f);
    rotatingMatrix = glm::mat4(1.0f);

    unTextured.Load("shaders/vertex.glsl", "shaders/fragment.glsl");

    unTextured.SetProjectionMatrix(projectionMatrix);
    unTextured.SetViewMatrix(viewMatrix);
    unTextured.SetColor(0.0f, 0.0f, 1.0f, 1.0f);

    glUseProgram(unTextured.programID);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    fireNationShipTextureID = LoadTexture("FireNation.png");
    aangTextureID = LoadTexture("Aang.png");
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    glUseProgram(program.programID);
    

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0;
float rotate_z = 0;
float player_x = 0;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    player_x += 1.0f * deltaTime;
    
    rotate_z += 45.0 * deltaTime;

    translateMatrix = glm::mat4(1.0f);
    translateMatrix = glm::translate(translateMatrix, glm::vec3(player_x, 0.0f, 0.0f)); 


    rotatingMatrix = glm::mat4(1.0f);
    rotatingMatrix = glm::translate(rotatingMatrix, glm::vec3(-3.5, 3.0, 0.0)); //just so that aang isn't behind the firenation ship. We wouldn't want that
    rotatingMatrix = glm::rotate(rotatingMatrix, glm::radians(rotate_z), glm::vec3(0.0f, 0.0f, 1.0f));
    
}

//not going keep on instantiate float arrays in a render() looped call from processinout
float fakeSea[] = {
-5.0f, -1.0f, 
-5.0f, -3.75f, 
5.0f, -1.0f,
5.0f, -1.0f,
-5.0f, -3.75f,
5.0f, -3.75f};

float testTriangle[] = { 0.5f, -0.5f, 0.0f, 0.5f, -0.5f, -0.5f }; //only for testing
//float vertices[] = { -2.0f, -2.0f, 2.0f, -2.0f, 2.0f, 0.0f, -2.0f, -2.0f, 2.0f, 0.0f, -2.0f, 0.0f };
void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    RenderTriangles(fakeSea, 6);
    RenderTextures();

    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
