#include "Level2.h"
#define LEVEL2_WIDTH 14
#define LEVEL2_HEIGHT 8

#define LEVEL2_ENEMYCOUNT 3
unsigned int level2_data[] =
{
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3,
 3, 1, 1, 1, 1, 1, 1, 0, 1, 3, 3, 3, 3, 3,
 3, 2, 2, 2, 2, 2, 2, 0, 2, 3, 3, 3, 3, 3
};
void Level2::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("marble.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 8, 8);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    //player set up
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(5, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.97f, 0);//gravity
    state.player->speed = 2.0f;
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
    state.player->width = 0.65f;
    state.player->jumpPower = 6.0f;





    /*

    */




    //enemy setup
    state.enemy = new Entity[LEVEL2_ENEMYCOUNT];
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
    for (int i = 0; i < LEVEL2_ENEMYCOUNT; i++) {
        state.enemy[i].isActive = false;
        //state.enemy[i].Update(0,NULL, NULL, 0, 0);
    }
    // Move over all of the player and enemy code from initialization.
}
void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemy, LEVEL2_ENEMYCOUNT, state.map);
    if (state.player->position.x >= 12) {
        state.nextScene = 2;
    }
}
void Level2::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->Render(program);
}
