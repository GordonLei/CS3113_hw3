#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <string>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

enum EntityType {PLAYER, PLATFORM, GOAL, LETTER};

class Entity {
public:
    
    EntityType entityType;
    bool isStatic;
    bool isActive;
    
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    
    float speed;
    float width;
    float height;
    std::string description;
    
    GLuint textureID;
    
    
    Entity();
    
    bool CheckCollision(Entity other);
    
    void Update(float deltaTime, Entity* objects, int objectCount);
    void Render(ShaderProgram *program);
    
    void CheckCollisionsX(Entity *objects, int objectCount);
    void CheckCollisionsY(Entity *objects, int objectCount);
    
    void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position);
    
    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool collidedRight;
    bool reachedGoal;
};



