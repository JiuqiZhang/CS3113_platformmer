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
#include "Util.h"


#include "Map.h"
#include "Entity.h"
#include "Scene.h"
#include "Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Lost.h"
#include "Effects.h"
#include "FinalLevel.h"
#include "Win.h"
Mix_Music *music;
Mix_Chunk *bounce;
Mix_Chunk *hit;

Effects* effects;


#define ENEMY_COUNT 3



SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene *currentScene;
Scene *sceneList[7];


void SwitchToScene(Scene* scene, int lives) {
    
    currentScene = scene;
    currentScene->Initialize();

    currentScene->state.player->lives = lives;

}

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




void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Find JQZ~", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize the levels and start at the first one
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    sceneList[0] = new Menu();
    sceneList[4] = new Lost();
    sceneList[5] = new FinalLevel();
    sceneList[6] = new Win();
    SwitchToScene(sceneList[0], 3);

    effects = new Effects(projectionMatrix, viewMatrix);
    effects->Start(FADEIN, 0.297f);
}

void ProcessInput() {
    currentScene->state.player->movement = glm::vec3(0);
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
                if (currentScene->state.player->collidedBottom) {
                    currentScene->state.player->jump = true;
                    Mix_PlayChannel(-1, bounce, 0);

                }
                
                break;
            case SDLK_RETURN:
                if (currentScene->state.player->menu == true) {
                    SwitchToScene(sceneList[1], 3);
                }

            }
            break; // SDL_KEYDOWN
        }
    }
    
    if (currentScene->state.player->hit) {
        effects->Start(SHAKE, 4.297f);
        currentScene->state.player->CheckCollisionsY(currentScene->state.map);

    }
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_B]) {
        if (currentScene->state.player->bottom == true) {
            SwitchToScene(sceneList[1], currentScene->state.player->lives);
        }
    }
    if (keys[SDL_SCANCODE_R]) {
        if (currentScene->state.player->restart == true) {
        SwitchToScene(sceneList[0], 3);
    }
    }
    if (keys[SDL_SCANCODE_A]) {
        currentScene->state.player->movement.x = -1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animLeft;
    }
    //not at the same time
    else if (keys[SDL_SCANCODE_D]) {
        currentScene->state.player->movement.x = 1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animRight;

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
    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }

    currentScene->state.player->ifHit(currentScene->state.enemy, ENEMY_COUNT);

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;
bool lastCollidedBottom = false;
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
        // 
        currentScene->Update(FIXED_TIMESTEP);

        /*if (lastCollidedBottom == false && currentScene->state.player->collidedBottom) {
            if (currentScene->state.enemy->isActive == false && currentScene->state.enemy->shake == true) {
                effects->Start(SHAKE, 2.97f);
            
        }*/
        lastCollidedBottom = currentScene->state.player->collidedBottom;

        effects->Update(FIXED_TIMESTEP);
        //currentScene->state.player->Update(FIXED_TIMESTEP, state.player, state.enemy, ENEMY_COUNT, state.map);
        /*for (int i = 0; i < ENEMY_COUNT; i++) {
            state.enemy[i].Update(FIXED_TIMESTEP, state.player, state.platforms, state.enemy, PLATFORM_COUNT, ENEMY_COUNT);
        }
        */

        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;
    //if (state.player->collidedBottom or state.player->collidedLeft or state.player->collidedRight or state.player->collidedTop) {
        
    //}
    viewMatrix = glm::mat4(1.0f);
    if (currentScene->state.player->position.x > 5) {
        viewMatrix = glm::translate(viewMatrix,
            glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
    }
    else {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    }


    viewMatrix = glm::translate(viewMatrix, effects->viewOffset);
}



void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);
    currentScene->Render(&program);


    effects->Render();
    /*for (int i = 0; i < ENEMY_COUNT; i++) {
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
    */
    

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

        if (currentScene->state.nextScene >= 0) {
            int live = currentScene->state.player->lives;
            SwitchToScene(sceneList[currentScene->state.nextScene], live);
            effects->Start(FADEOUT, 0.297f);
            effects->Start(FADEIN, 0.297f);
        }
        Render();
    }

    Shutdown();
    return 0;
}