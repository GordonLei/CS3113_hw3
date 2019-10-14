#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool acceptsInput = true;


ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

#define PLATFORM_COUNT 70
//#define LETTER_FAIL_COUNT 14 //15
//#define LETTER_SUCCESS_COUNT 18

struct GameState {
    Entity player;
    Entity platforms[PLATFORM_COUNT];
    Entity message_fail;
    Entity message_success;
};

GameState state;

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

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Physics!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    state.player.entityType = PLAYER;
    state.player.isStatic = false;
    //state.player.reachedGoal = false;
    state.player.width = 0.75f;
    state.player.position = glm::vec3(0, 7.0, 0);
    state.player.acceleration = glm::vec3(0, -.0981f, 0);
    state.player.velocity = glm::vec3(0, 0, 0);
    state.player.textureID = LoadTexture("me.png");
    
    GLuint tileTextureID = LoadTexture("tile.png");
    GLuint goalTextureID = LoadTexture("goal.jpg");
    
    GLuint fontTextureID = LoadTexture("font2.png");
    //GLuint fontTextureID = LoadTexture("tile.png");
    
    
    std::string message_fail = "Mission Failed";
    std::string message_success = "Mission Successful";
    
    state.message_fail.entityType = LETTER;
    state.message_fail.textureID = fontTextureID;
    state.message_fail.description = message_fail;
    state.message_fail.isActive = false;
    state.message_fail.position = glm::vec3(-5.5,0,0);
    
    state.message_success.entityType = LETTER;
    state.message_success.textureID =fontTextureID;
    state.message_success.description = message_success;
    state.message_success.isActive = false;
    state.message_success.position = glm::vec3(-7.5,0,0);
    
    //int curr_letter = 0;
    int curr_platform = 0;
    float curr_xvalue = -8.5f;
    float curr_yvalue = -7.00f;
    /*
    for(int counter = 0; counter < LETTER_FAIL_COUNT; ++counter){
        state.message_fail[curr_letter].description = "I";//message_fail[counter];
        state.message_fail[curr_letter].textureID = fontTextureID;
        state.message_fail[curr_letter].position = glm::vec3(curr_xvalue, 0.0f, 0);
        state.message_fail[curr_letter].isActive = false;
        ++curr_letter;
        curr_xvalue += 1.0f;
    }
    curr_xvalue = -8.5;
    curr_letter = 0;
    for(int counter = 0; counter < LETTER_SUCCESS_COUNT; ++counter){
        state.message_success[curr_letter].description = "ADADSADSADSADASDASDASDAS";//message_success[counter];
        state.message_success[curr_letter].textureID = fontTextureID;
        state.message_success[curr_letter].position = glm::vec3(curr_xvalue, 0.0f, 0);
        state.message_success[curr_letter].isActive = false;
        ++curr_letter;
        curr_xvalue += 1.0f;
    }
    */
    
    //make the bottom row
    //curr_xvalue = -8.5f;
    
    for(int counter = 0; counter < 18; ++counter){
        state.platforms[curr_platform].textureID = tileTextureID;
        state.platforms[curr_platform].position = glm::vec3(curr_xvalue, -7.0f, 0);
        ++curr_platform;
        curr_xvalue += 1.0f;
    }
    
    //make the lefthand column
    //curr_xvalue = -9.5f;
    
    for(int counter = 0; counter < 15; ++counter){
        state.platforms[curr_platform].textureID = tileTextureID;
        state.platforms[curr_platform].position = glm::vec3(-9.5f, curr_yvalue, 0);
        ++curr_platform;
        curr_yvalue += 1.0f;
    }
    
    //make the righthand column
    //curr_xvalue = 9.5f;
    curr_yvalue = -7.0f;
    
    for(int counter = 0; counter < 15; ++counter){
        state.platforms[curr_platform].textureID = tileTextureID;
        state.platforms[curr_platform].position = glm::vec3(9.5f, curr_yvalue, 0);
        ++curr_platform;
        curr_yvalue += 1.0f;
    }
    
    //should start at 48 now
    //make the floating row / column things in the middle
    curr_xvalue = -1.0f;
    for(int counter = 0; counter < 3; ++counter){
        state.platforms[curr_platform].textureID = tileTextureID;
        state.platforms[curr_platform].position = glm::vec3(curr_xvalue, 2.0f, 0);
        ++curr_platform;
        curr_xvalue += 1.0f;
    }
    
    curr_xvalue = -1.75f;
    for(int counter = 0; counter < 3; ++counter){
        state.platforms[curr_platform].textureID = tileTextureID;
        state.platforms[curr_platform].position = glm::vec3(curr_xvalue, -3.0f, 0);
        ++curr_platform;
        curr_xvalue -= 1.0f;
    }
    
    curr_xvalue = 1.75f;
    for(int counter = 0; counter < 3; ++counter){
        state.platforms[curr_platform].textureID = tileTextureID;
        state.platforms[curr_platform].position = glm::vec3(curr_xvalue, -3.0f, 0);
        ++curr_platform;
        curr_xvalue += 1.0f;
    }
    
    curr_yvalue = -5.50f;
    for(int counter = 0; counter < 5; ++counter){
        state.platforms[curr_platform].textureID = tileTextureID;
        state.platforms[curr_platform].position = glm::vec3(-6.5f, curr_yvalue, 0);
        ++curr_platform;
        curr_yvalue += 1.0f;
    }
    
    curr_yvalue = -5.50f;
    for(int counter = 0; counter < 5; ++counter){
        state.platforms[curr_platform].textureID = tileTextureID;
        state.platforms[curr_platform].position = glm::vec3(6.5f, curr_yvalue, 0);
        ++curr_platform;
        curr_yvalue += 1.0f;
    }
    
    //The goals
    state.platforms[curr_platform].textureID = goalTextureID;
    state.platforms[curr_platform].position = glm::vec3(-8, -6.0f, 0);
    //std :: cout << curr_platform << std :: endl;
    //std :: cout << state.platforms[0].entityType << std :: endl;
    //std :: cout << state.platforms[curr_platform].entityType << std :: endl;
    state.platforms[curr_platform].entityType = GOAL;
    //std :: cout << state.platforms[curr_platform].entityType << std :: endl;
    
    state.platforms[++curr_platform].textureID = goalTextureID;
    state.platforms[curr_platform].position = glm::vec3(8, -6.0f, 0);
    //std :: cout << curr_platform << std :: endl;
    //std :: cout << state.platforms[curr_platform].entityType << std :: endl;
    state.platforms[curr_platform].entityType = GOAL;
    //std :: cout << state.platforms[curr_platform].entityType << std :: endl;
    //state.platforms[curr_platform].entityType = GOAL;
    
    state.platforms[++curr_platform].textureID = goalTextureID;
    state.platforms[curr_platform].position = glm::vec3(0, -6.0f, 0);
    //std :: cout << curr_platform << std :: endl;
    //std :: cout << state.platforms[curr_platform].entityType << std :: endl;
    state.platforms[curr_platform].entityType = GOAL;
    //std :: cout << state.platforms[curr_platform].entityType << std :: endl;
    //state.platforms[curr_platform].entityType = GOAL;
    //std :: cout << state.platforms[0].entityType << std :: endl;

    
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-10.0f, 10.0f, -7.50f, 7.50f, -2.0f, 2.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    //program.SetColor(0.0f, 5.0f, 0.0f, 5.0f);
    
    glUseProgram(program.programID);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    //glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClearColor(0.0f,0.3f,0.4f,1.0f);
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            
        }
    }
    
    state.player.velocity.x = 0;
    
    // Check for pressed/held keys below
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_A])
    {
        state.player.acceleration.x = -30.0f;
    }
    else if (keys[SDL_SCANCODE_D])
    {
        state.player.acceleration.x = 30.0f;
    }
    
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.player.Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);
        
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    state.player.Render(&program);
    
    for (int i = 0; i < PLATFORM_COUNT; i++)
    {
        state.platforms[i].Render(&program);
    }
    
    //GLuint fontTextureID = LoadTexture("font1.png");
    
    /*
    for (int i = 0; i < LETTER_FAIL_COUNT; i++)
    {
        std::string x = "Mission Failed";
        //state.message_fail[i].DrawText(&program, fontTextureID, x, 5.0f, 1.0f, glm::vec3(0.0f,0.0f,0.0f));
        //state.message_fail[i].DrawText(&program, fontTextureID, "wow", 1.0f, 1.0f, glm::vec3(0.0f,0.0f,0.0f));
        state.message_fail[i].Render(&program);
    }
     */
    
    state.message_fail.Render(&program);
    state.message_success.Render(&program);
    
    /*
    for (int i = 0; i < LETTER_SUCCESS_COUNT; i++)
    {
        std::string x = "Mission Successful";
        //state.message_fail[i].DrawText(&program, fontTextureID, x, 5.0f, 1.0f, glm::vec3(0.0f,0.0f,0.0f));
        //state.message_fail[i].DrawText(&program, fontTextureID, "wow", 1.0f, 1.0f, glm::vec3(0.0f,0.0f,0.0f));
        state.message_success[i].Render(&program);
    }
    */
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        if (state.player.collidedTop || state.player.collidedLeft|| state.player.collidedRight || state.player.collidedBottom){
            state.player.velocity = glm::vec3(0, 0, 0);
            state.player.acceleration = glm::vec3(0, 0, 0);
            acceptsInput = false;
            if(state.player.reachedGoal){
                std :: cout << "WE DID IT";
                
                state.message_success.isActive = true;
                
            }
            
            else{
                std :: cout << "WE DID NOT DO IT";
                
                state.message_fail.isActive = true;
            
            }
            
        }
        if(acceptsInput){
            ProcessInput();
        }
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
