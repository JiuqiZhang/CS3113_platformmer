#include "Win.h"
#define Win_WIDTH 28
#define Win_HEIGHT 8

#define Win_ENEMYCOUNT 3
unsigned int Win_data[] =
{
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3
};
void Win::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("blue.png");
    state.map = new Map(Win_WIDTH, Win_HEIGHT, Win_data, mapTextureID, 1.0f, 8, 8);
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

    //player set up
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(3, -4, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.97f, 0);//gravity
    state.player->speed = 1.0f;
    state.player->textureID = Util::LoadTexture("juese.png");

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
    state.player->width = 1.0f;
    state.player->jumpPower = 6.0f;
    state.player->restart = true;




    /*

    */




    //enemy setup
    state.enemy = new Entity[Win_ENEMYCOUNT];
    GLuint enemyTextureID = Util::LoadTexture("bad.png");


    state.enemy[0].textureID = enemyTextureID;
    state.enemy[0].entityType = ENEMI;
    state.enemy[0].position = glm::vec3(1, -2.25f, 0);

    state.enemy[0].speed = 0.97;
    state.enemy[0].height = 0.9f;
    state.enemy[0].width = 1.0f;

    state.enemy[0].aiType = PATROLER;
    state.enemy[0].movement = glm::vec3(-1, 0, 0);
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
    for (int i = 0; i < Win_ENEMYCOUNT; i++) {
        state.enemy[i].isActive = false;
        //state.enemy[i].Update(0,NULL, NULL, 0, 0);
    }
    // Move over all of the player and enemy code from initialization.
}
void Win::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemy, Win_ENEMYCOUNT, state.map);
    if (state.player->position.x >= 20) {
        state.nextScene = 6;
    }
}
void Win::Render(ShaderProgram* program) {
    GLuint fontTextureID = Util::LoadTexture("font2.png");
    Util::DrawText(program, fontTextureID, "You Win, winner", 0.4f, 0.1f, glm::vec3(2, -5, 0));
    Util::DrawText(program, fontTextureID, "Hit 'R' to restart", 0.4f, 0.1f, glm::vec3(2, -4, 0));
    if (state.player->lives == 3) {
        Util::DrawText(program, fontTextureID, "alive with still 3 chances", 0.8f, 0.1f, glm::vec3(2, -6, 0));
    }
    else if (state.player->lives == 2) {
        Util::DrawText(program, fontTextureID, "only spent 1 life", 0.8f, 0.1f, glm::vec3(2, -6, 0));
    }
    else if (state.player->lives == 1) {
        Util::DrawText(program, fontTextureID, "spent 2 lives", 0.8f, 0.1f, glm::vec3(2, -6, 0));
    }
    state.map->Render(program);
    state.player->Render(program);
}
