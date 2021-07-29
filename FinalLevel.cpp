#include "FinalLevel.h"
#define FLevel_WIDTH 14
#define FLevel_HEIGHT 8

#define FLevel_ENEMYCOUNT 2
unsigned int FLevel_data[] =
{
 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 33,
 33, 3, 0, 0, 0, 2, 17, 24, 25, 26, 33, 0, 49, 33,
 33, 33, 3, 0, 1, 33, 33, 33, 33, 33, 33, 33, 33, 33,
 33, 33, 33, 0, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33
};

void FinalLevel::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("box.png");
    state.map = new Map(FLevel_WIDTH, FLevel_HEIGHT, FLevel_data, mapTextureID, 1.0f, 8, 8);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    //player set up
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1.2f, 6, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.97f, 0);//gravity
    state.player->speed = 1.5f;
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
    state.player->height = 0.9f;
    state.player->width = 0.8f;
    state.player->jumpPower = 6.0f;





    /*

    */




    //enemy setup
    state.enemy = new Entity[FLevel_ENEMYCOUNT];


    state.enemy[0].textureID = Util::LoadTexture("ball.png");
    state.enemy[0].entityType = ENEMI;
    state.enemy[0].position = glm::vec3(2.5, 1, 0);
    state.enemy[0].movement = glm::vec3(0);
    state.enemy[0].acceleration = glm::vec3(0, -9.97f, 0);

    state.enemy[0].aiType = JUMPER;
    state.enemy[0].aiState = JUMPING;
    state.enemy[0].height = 0.8f;
    state.enemy[0].width = 0.65f;

    state.enemy[1].textureID = Util::LoadTexture("ball.png");
    state.enemy[1].entityType = ENEMI;
    state.enemy[1].position = glm::vec3(11, 1, 0);
    state.enemy[1].movement = glm::vec3(0);
    state.enemy[1].acceleration = glm::vec3(0, -9.97f, 0);

    state.enemy[1].aiType = JUMPER;
    state.enemy[1].aiState = JUMPING;
    state.enemy[1].height = 0.8f;
    state.enemy[1].width = 0.8f;


    // Move over all of the player and enemy code from initialization.
}
void FinalLevel::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemy, FLevel_ENEMYCOUNT, state.map);
    for (int i = 0; i < FLevel_ENEMYCOUNT; i++) {
        state.enemy[i].Update(deltaTime, state.player, state.enemy, FLevel_ENEMYCOUNT, state.map);
    }

    //state.enemy->Update(deltaTime, state.player, state.enemy, LEVEL1_ENEMYCOUNT, state.map);

    if (state.player->position.x >= 11.5 && state.player->position.y >= -5 && state.player->position.y <= -4) {
        state.nextScene = 6;
    }
    if (state.player->position.y <= -7.0f) {
        state.player->lives -= 1;
        state.nextScene = 3;
    }
    else if (state.player->isActive == false) {
        state.nextScene = 3;
        state.player->lives -= 1;

    }
    state.player->ifHit(state.enemy, FLevel_ENEMYCOUNT);
    state.player->CheckCollisionsY(state.enemy, FLevel_ENEMYCOUNT);

    if (state.player->lives <= 0) {
        state.nextScene = 4;
    }

}
void FinalLevel::Render(ShaderProgram* program) {

    state.map->Render(program);
    state.player->Render(program);
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    GLuint fontTexture2ID = Util::LoadTexture("font2.png");
    Util::DrawText(program, fontTexture2ID, "Level 3", 0.597f, 0.0297f, glm::vec3(2, -2.5, 0));
    Util::DrawText(program, fontTexture2ID, "Don't get fooled by the camouflage", 0.597f, 0.0297f, glm::vec3(2, -2.5, 0));

    Util::DrawText(program, fontTexture2ID, "Find Z", 0.4f, 0.0297f, glm::vec3(2, -4.2, 0));

    for (int i = 0; i < FLevel_ENEMYCOUNT; i++) {
        state.enemy[i].Render(program);
    }
}