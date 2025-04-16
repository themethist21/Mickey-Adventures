#include "TreeEnemy.h"
#include "Player.h"
#include "Game.h"
#include <iostream>

#define TREE_WIDTH 24
#define TREE_HEIGHT 32

#define SPEED 1
#define FALL_STEP 4

//spawn
#define SPAWN_BORDER 128
#define BORDER_SIZE 16
#define SPAWN_SIZE 128
#define DESPAWN_BORDER 384

#define DEATH_TIME 1.f
#define DEATH_JUMP_HEIGHT 96
#define DEATH_JUMP_STEP 4


enum TreeAnimations
{
	MOVE, DIE, NANIMS
};

void TreeEnemy::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{
	spritesheet.loadFromFile("images/Tree.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(TREE_WIDTH, TREE_HEIGHT), glm::vec2(float(TREE_WIDTH) / 96.f, float(TREE_HEIGHT)/TREE_HEIGHT), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(NANIMS);

	sprite->setAnimationSpeed(MOVE, 5);
	sprite->addKeyframe(MOVE, glm::vec2(0.f, 0.f));
	sprite->addKeyframe(MOVE, glm::vec2(24.f / 96.f, 0.f));
	sprite->addKeyframe(MOVE, glm::vec2(48.f / 96.f, 0.f));

	// Animacion MOVE
	sprite->setAnimationSpeed(DIE, 0);
	sprite->addKeyframe(DIE, glm::vec2(72.f / 96.f, 0.f));

	sprite->changeAnimation(MOVE);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posTree.x), float(tileMapDispl.y + posTree.y)));

	initFacingLeft = facingLeft = false;

	showHitbox = false;
	prevF1KeyState = false;

	// Cargar el sonido
	if (!dieBuffer.loadFromFile("sounds/SFX/Ouch/Wav/Ouch__009.wav")) {
		std::cerr << "Error al cargar Select.wav" << std::endl;
	}

	// Crear una instancia de sf::Sound y asignarle el buffer
	dieEffect.setBuffer(dieBuffer);
	dieEffect.setVolume(Game::SOUND_EFFECTS_VOLUME + 20);
}

void TreeEnemy::update(int deltaTime) {
	//Timers
	deathTimer -= deltaTime / 1000.f;

	//Collisions

	//Sprite
	sprite->update(deltaTime);

	//Movement

	switch (enemyState) {
	case EnemyStates::SPAWN: updateSpawn(deltaTime); break;
	case EnemyStates::ATTACK: updateAttack(deltaTime); break;
	case EnemyStates::DIE: updateDie(deltaTime); break;
	}

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posTree.x), float(tileMapDispl.y + posTree.y)));

}

void TreeEnemy::updateAttack(int deltaTime)
{
	Object* obj = nullptr;
	translatePosition(glm::ivec2(0, FALL_STEP));
	if (map->collisionMoveDown(posTree, glm::ivec2(TREE_WIDTH, TREE_HEIGHT), TREE_HEIGHT, &posTree.y, &collisions[0], obj)) {
		if (facingLeft) {
			translatePosition(glm::ivec2(-SPEED, 0));
			if (map->collisionMoveLeft(posTree, glm::ivec2(TREE_WIDTH, TREE_HEIGHT), &collisions[0], &posTree.x, obj))
			{
				facingLeft = false;
				sprite->setScale(glm::vec2(1.f, 1.f));
			}
		}
		else {
			translatePosition(glm::ivec2(SPEED, 0));
			if (map->collisionMoveRight(posTree, glm::ivec2(TREE_WIDTH, TREE_HEIGHT), &collisions[0], &posTree.x, obj))
			{
				facingLeft = true;
				sprite->setScale(glm::vec2(-1.f, 1.f));
			}
		}
	}
	obj = nullptr;

	glm::vec2 playerPos = player->getPosition();
	glm::vec2 distance = glm::vec2(abs(playerPos.x - posTree.x), abs(playerPos.y - posTree.y));
	glm::vec2 mapSize = map->getSize();
	if (distance.x >= DESPAWN_BORDER || posTree.y >= mapSize.y - 48) {
		enemyState = EnemyStates::SPAWN;
		posTree = initPosTree;
		facingLeft = initFacingLeft;
		if (facingLeft) sprite->setScale(glm::vec2(-1.f, 1.f));
		else sprite->setScale(glm::vec2(1.f, 1.f));
	}
}

void TreeEnemy::updateDie(int deltaTime)
{
	if (sprite->animation() != DIE) {
		sprite->changeAnimation(DIE);
		dieEffect.play();
		bJumping = true;
		jumpAngle = 60;
		startY = posTree.y + int(DEATH_JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.0f));
	}

	translatePosition(glm::ivec2(1, 0));
	if (bJumping)
	{
		jumpAngle += DEATH_JUMP_STEP;
		if (jumpAngle == 180)
		{
			bJumping = false;
			posTree.y = startY;
		}
		else
		{
			posTree.y = int(startY - DEATH_JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));
		}
	}
	else
	{
		translatePosition(glm::ivec2(0, FALL_STEP));
	}

	if (deathTimer < 0) {
		reset();
	}
}

void TreeEnemy::updateSpawn(int deltaTime)
{
	glm::vec2 playerPos = player->getPosition();
	glm::vec2 distance = glm::vec2(abs(playerPos.x - posTree.x), abs(playerPos.y - posTree.y));
	if (distance.x <= SPAWN_BORDER - BORDER_SIZE && distance.y <= SPAWN_SIZE) playerInRange = true;
	else if ((distance.x <= SPAWN_BORDER) && (distance.y <= SPAWN_SIZE) && !playerInRange) {
		enemyState = EnemyStates::ATTACK;
		posTree = initPosTree;
		facingLeft = initFacingLeft;
		if (facingLeft) sprite->setScale(glm::vec2(-1.f, 1.f));
		else sprite->setScale(glm::vec2(1.f, 1.f));
		playerInRange = true;
	}
	else if (distance.x > SPAWN_BORDER) playerInRange = false;
}

void TreeEnemy::render()
{
	if (!isSpawn()) sprite->render();
}

void TreeEnemy::reset()
{
	enemyState = EnemyStates::SPAWN;
	posTree = initPosTree;
	facingLeft = initFacingLeft;
	if (facingLeft) sprite->setScale(glm::vec2(-1.f, 1.f));
	else sprite->setScale(glm::vec2(1.f, 1.f));
	sprite->changeAnimation(MOVE);
}

void TreeEnemy::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}

void TreeEnemy::setPlayer(Player * playerPtr)
{
	player = playerPtr;
}

void TreeEnemy::setPosition(const glm::vec2& pos)
{
	posTree = pos;
	initPosTree = posTree;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posTree.x), float(tileMapDispl.y + posTree.y)));
}

void TreeEnemy::setFacingLeft(bool faceLeft)
{
	initFacingLeft = facingLeft = faceLeft;
	if (faceLeft) sprite->setScale(glm::vec2(-1.f, 1.f));
}

void TreeEnemy::setDefeat()
{
	deathTimer = DEATH_TIME;
	enemyState = EnemyStates::DIE;
}

glm::ivec2 TreeEnemy::getSize()
{
	return glm::ivec2(TREE_WIDTH, TREE_HEIGHT);
}

glm::ivec2 TreeEnemy::getPosition()
{
	return posTree;
}

bool TreeEnemy::isSpawn()
{
	return (enemyState == EnemyStates::SPAWN);
}

bool TreeEnemy::isAttacking() 
{
	return (enemyState == EnemyStates::ATTACK);
}

void TreeEnemy::translatePosition(const glm::vec2& t) {
	posTree += t;
}