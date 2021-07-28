#include "Level1.h"
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8

#define LEVEL1_ENEMYCOUNT 1
unsigned int level1_data[] =
{
 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 35, 0, 0, 0, 0, 0, 0, 0, 35, 35, 0, 0, 0, 0,
 35, 0, 0, 0, 0, 0, 0, 35, 35, 35, 35, 0, 0, 0,
 35, 0, 0, 0, 0, 0, 35, 35, 35, 35, 35, 35, 0, 0,
 35, 0, 0, 0, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35
};




void Level1::Initialize() {


    state.nextScene = -1;
	GLuint mapTextureID = Util::LoadTexture("wood.png");
	state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 8, 8);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

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
    state.enemy = new Entity[LEVEL1_ENEMYCOUNT];


    state.enemy[0].textureID = Util::LoadTexture("bad.png");
    state.enemy[0].entityType = ENEMI;
    state.enemy[0].position = glm::vec3(2.5f, 0, 0);

    state.enemy[0].speed = 0.97;
    state.enemy[0].height = 0.9f;
    state.enemy[0].width = 1.0f;

    state.enemy[0].aiType = PATROLER;
    state.enemy[0].movement = glm::vec3(-1, 0, 0);
    state.enemy[0].aiState = PATROLING;
    state.enemy[0].isActive = true;
	// Move over all of the player and enemy code from initialization.
}
void Level1::Update(float deltaTime) {
	state.player->Update(deltaTime, state.player, state.enemy, LEVEL1_ENEMYCOUNT, state.map);
    for (int i = 0; i < LEVEL1_ENEMYCOUNT; i++) {
        state.enemy[i].Update(deltaTime, state.player, state.enemy, LEVEL1_ENEMYCOUNT, state.map);
    }

    //state.enemy->Update(deltaTime, state.player, state.enemy, LEVEL1_ENEMYCOUNT, state.map);
    
    if (state.player->position.x >= 12) {
        state.nextScene = 1;
    }
}
void Level1::Render(ShaderProgram* program) {
	state.map->Render(program);
	state.player->Render(program);
    state.enemy->Render(program);
}
