#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#include "Entity.h"

Mix_Music *music;
Mix_Chunk *bounce;
Mix_Chunk *hit;

#define PLATFORM_COUNT 25
#define ENEMY_COUNT 3
struct GameState {
    Entity *player;
    Entity *platforms;
    Entity *enemy;
};

GameState state;


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;



void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;

    std::vector<float> vertices;
    std::vector<float> texCoords;

    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;

        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            });

        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
            });

    } // end of for loop

    glm::mat4 fontModelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(fontModelMatrix, position);
    program->SetModelMatrix(fontModelMatrix);

    glUseProgram(program->programID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
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



void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Texture!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");


    // Start Audio
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    //play music
    music = Mix_LoadMUS("bgm.mp3");
    Mix_PlayMusic(music, -1);
    //jump effect
    bounce = Mix_LoadWAV("jump.wav");
    //hit effect 
    hit = Mix_LoadWAV("hit.wav");


    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    //player set up
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1, 5, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.97f, 0);//gravity
    state.player->speed = 0.97f;
    state.player->textureID = LoadTexture("juese.png");

    //animation of the player
    state.player->animFront = new int[4]{ 0, 1, 2, 3 };
    state.player->animLeft = new int[4]{ 12, 13, 14, 15 };
    state.player->animRight = new int[4]{ 8, 9, 10, 11 };
    state.player->animBack = new int[4]{ 4, 5, 6, 7 };
    state.player->animIndices = state.player->animFront;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animIndex = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    //state.player->height = 0.95f;
    state.player->width = 0.65f;
    state.player->jumpPower = 5.0f;

    



    /*
    
    */



    //platformsetup
    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("metal.png");
    for (int i = 0; i < 12; i++) {
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-5+i, -3.25f, 0);

    }
    state.platforms[12].textureID = platformTextureID;
    state.platforms[12].position = glm::vec3(1, 0.75, 0);
    state.platforms[13].textureID = platformTextureID;
    state.platforms[13].position = glm::vec3(3, -2.5f, 0);

    state.platforms[14].textureID = platformTextureID;
    state.platforms[14].position = glm::vec3(-1, -2.5f, 0);
    state.platforms[15].textureID = platformTextureID;
    state.platforms[15].position = glm::vec3(-2, -2.5f, 0);
    state.platforms[16].textureID = platformTextureID;
    state.platforms[16].position = glm::vec3(-3, -2.5f, 0);
    state.platforms[17].textureID = platformTextureID;
    state.platforms[17].position = glm::vec3(-3, -1.75f, 0);
    state.platforms[18].textureID = platformTextureID;
    state.platforms[18].position = glm::vec3(-4, -2.75f, 0);
    state.platforms[19].textureID = platformTextureID;
    state.platforms[19].position = glm::vec3(-5, -2.75f, 0);
    state.platforms[20].textureID = platformTextureID;
    state.platforms[20].position = glm::vec3(0, 0, 0);
    state.platforms[21].textureID = platformTextureID;
    state.platforms[21].position = glm::vec3(-1, 0, 0);
    state.platforms[22].textureID = platformTextureID;
    state.platforms[22].position = glm::vec3(1, 0, 0);
    state.platforms[23].textureID = platformTextureID;
    state.platforms[23].position = glm::vec3(2, 0, 0);
    state.platforms[24].textureID = platformTextureID;
    state.platforms[24].position = glm::vec3(-2, -0.75f, 0);
    //update platforms
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].entityType = PLATFORM;
    }
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Update(0, NULL, NULL, NULL, 0, 0);
    }
    //enemy setup
    state.enemy = new Entity[ENEMY_COUNT];
    GLuint enemyTextureID = LoadTexture("bad.png");


    state.enemy[0].textureID = enemyTextureID;
    state.enemy[0].entityType = ENEMI;
    state.enemy[0].position = glm::vec3(1, -2.25f, 0);

    state.enemy[0].speed = 0.6;
    state.enemy[0].height = 0.9f;
    state.enemy[0].width = 1.0f;

    state.enemy[0].aiType = PATROLER;
    state.enemy[0].movement = glm::vec3(-1,0,0);
    state.enemy[0].aiState = PATROLING;

    state.enemy[1].textureID = enemyTextureID;
    state.enemy[1].position = glm::vec3(2, 1.0f, 0);
    state.enemy[1].movement = glm::vec3(0);
    state.enemy[1].acceleration = glm::vec3(0, -9.97f, 0);
    state.enemy[1].entityType = ENEMI;
    state.enemy[1].aiType = SPINNER;
    state.enemy[1].aiState = SPINING;

    state.enemy[1].speed = 1.0f;
    state.enemy[1].height = 1.0f;
    state.enemy[1].width = 0.6f;



    state.enemy[2].textureID = enemyTextureID;
    state.enemy[2].position = glm::vec3(-4, -1.75f, 0);
    state.enemy[2].movement = glm::vec3(0);
    state.enemy[2].acceleration = glm::vec3(0, -9.97f, 0);
    state.enemy[2].entityType = ENEMI;
    state.enemy[2].aiType = JUMPER;
    state.enemy[2].aiState = JUMPING;
    state.enemy[2].speed = 1.0f;
    state.enemy[2].height = 0.8f;
    state.enemy[2].width = 0.8f;
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemy[i].Update(0, NULL, NULL, NULL, 0, 0);
    }
}

void ProcessInput() {
    state.player->movement = glm::vec3(0);
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                // Move the player left
                break;

            case SDLK_RIGHT:
                // Move the player right
                break;

            case SDLK_SPACE:
                if (state.player->collidedBottom) {
                    state.player->jump = true;
                    Mix_PlayChannel(-1, bounce, 0);
                }
                break;
            }
            break; // SDL_KEYDOWN
        }
    }
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_A]) {
        state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    //not at the same time
    else if (keys[SDL_SCANCODE_D]) {
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;

    }
    /*if (state.player->defeat == true) {
            state.enemy[0].isActive = false;
        }
        else { state.player->isActive == false; }*/


    /*if (state.player->CheckCollision(&state.enemy[0]) == true) {
        state.player->CheckCollisionsX(state.enemy, ENEMY_COUNT);
        state.player->CheckCollisionsY(state.enemy, ENEMY_COUNT);
        if (state.player->defeat == true) {
            state.enemy[0].isActive = false;
        }
        if (state.player->collidedRight == true or state.player->collidedLeft == true) {
            state.player->isActive = false;
        }
        

        
    }
    else if (state.player->CheckCollision(&state.enemy[1]) == true) {
        state.player->CheckCollisionsX(state.enemy, ENEMY_COUNT);
        state.player->CheckCollisionsY(state.enemy, ENEMY_COUNT);
        if (state.player->collidedRight == true or state.player->collidedLeft == true) {
            state.player->isActive = false;
        }
        if (state.player->defeat == true) {
            state.enemy[1].isActive = false;
        }

    }
    else if (state.player->CheckCollision(&state.enemy[2]) == true) {
        state.player->CheckCollisionsX(state.enemy, ENEMY_COUNT);
        state.player->CheckCollisionsY(state.enemy, ENEMY_COUNT);
        if (state.player->collidedRight == true or state.player->collidedLeft == true) {
            state.player->isActive = false;
        }
        if (state.player->defeat == true) {
            state.enemy[2].isActive = false;
        }
    }*/

    //cant move faster with two keys pressed
    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }

    state.player->ifHit(state.enemy, ENEMY_COUNT);

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
        state.player->Update(FIXED_TIMESTEP, state.player, state.platforms,state.enemy, PLATFORM_COUNT, ENEMY_COUNT);
        for (int i = 0; i < ENEMY_COUNT; i++) {
            state.enemy[i].Update(FIXED_TIMESTEP, state.player, state.platforms, state.enemy, PLATFORM_COUNT, ENEMY_COUNT);
        }

        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;
    //if (state.player->collidedBottom or state.player->collidedLeft or state.player->collidedRight or state.player->collidedTop) {
        
    //}

}



void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemy[i].Render(&program);
    }
    if (state.player->isActive == false) {
        DrawText(&program, LoadTexture("font1.png"), "YOU LOSE!", 0.697f, -0.397f, glm::vec3(-5, 0, 0));
    }
    else if(state.player->enemy_num == 0) {
        DrawText(&program, LoadTexture("font1.png"), "YOU WIN!", 0.697f, -0.397f, glm::vec3(-5, 0, 0));
    }
    else if(state.player->collidedBottom or state.player->collidedLeft or state.player->collidedRight) {
        DrawText(&program, LoadTexture("font2.png"), "Jump on them!", 0.597f, -0.307f, glm::vec3(-5, 0, 0));
    }
    else {
        DrawText(&program, LoadTexture("font2.png"), "DONT TOUCH ENEMY", 0.597f, -0.307f, glm::vec3(-5, 0, 0));

    }
    state.player->Render(&program);

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