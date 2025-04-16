#include "SpiderEnemy.h"
#include "Player.h"
#include <iostream>

#define SPIDER_WIDTH 24
#define SPIDER_HEIGHT 16

#define SPEED 1
#define FALL_SPEED 1
#define STOP_TIME 1.f

//spawn
#define SPAWN_BORDER 128
#define BORDER_SIZE 16
#define SPAWN_SIZE 128
#define DESPAWN_BORDER 384

#define DEATH_TIME 1.f
#define DEATH_JUMP_HEIGHT 96
#define DEATH_JUMP_STEP 4


enum SpiderAnimations
{
	MOVE, ATTACK, DIE, NANIMS
};

void SpiderEnemy::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{
	spritesheet.loadFromFile("images/Spider.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(SPIDER_WIDTH, SPIDER_HEIGHT), glm::vec2(float(SPIDER_WIDTH) / 144.f, float(SPIDER_HEIGHT) / SPIDER_HEIGHT), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(NANIMS);

	sprite->setAnimationSpeed(MOVE, 4);
	sprite->addKeyframe(MOVE, glm::vec2(0.f, 0.f));
	sprite->addKeyframe(MOVE, glm::vec2(24.f / 144.f, 0.f));
	sprite->addKeyframe(MOVE, glm::vec2(48.f / 144.f, 0.f));

	sprite->setAnimationSpeed(DIE, 0);
	sprite->addKeyframe(DIE, glm::vec2(120.f / 144.f, 0.f));

	sprite->setAnimationSpeed(ATTACK, 3);
	sprite->addKeyframe(ATTACK, glm::vec2(72.f / 144.f, 0.f));
	sprite->addKeyframe(ATTACK, glm::vec2(96.f / 144.f, 0.f));

	sprite->changeAnimation(MOVE);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));

	facingLeft = false;

	showHitbox = false;
	prevF1KeyState = false;
}

void SpiderEnemy::update(int deltaTime) {
	//Timers
	deathTimer -= deltaTime / 1000.f;
	stopTimer -= deltaTime / 1000.f;

	//Collisions

	//Sprite
	sprite->update(deltaTime);

	//Movement

	switch (enemyState) {
	case SpiderEnemyStates::SPAWN: updateSpawn(deltaTime); break;
	case SpiderEnemyStates::MOVE: updateMove(deltaTime); break;
	case SpiderEnemyStates::ATTACK: updateAttack(deltaTime); break;
	case SpiderEnemyStates::DIE: updateDie(deltaTime); break;
	}

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));

}

void SpiderEnemy::updateMove(int deltaTime)
{
	if (sprite->animation() != MOVE) {
		sprite->changeAnimation(MOVE);
	}
	Object* obj = nullptr;
		if (facingLeft) {
			translatePosition(glm::ivec2(-SPEED, 0));
			if (map->collisionMoveLeft(posEnemy, glm::ivec2(SPIDER_WIDTH, SPIDER_HEIGHT), &collisions[0], &posEnemy.x, obj))
			{
				facingLeft = false;
				sprite->setScale(glm::vec2(1.f, 1.f));
			}
		}
		else {
			translatePosition(glm::ivec2(SPEED, 0));
			if (map->collisionMoveRight(posEnemy, glm::ivec2(SPIDER_WIDTH, SPIDER_HEIGHT), &collisions[0], &posEnemy.x, obj))
			{
				facingLeft = true;
				sprite->setScale(glm::vec2(-1.f, 1.f));
			}
		}
	obj = nullptr;

	glm::vec2 playerPos = player->getPosition();
	if (posEnemy.x < (playerPos.x + 24) && (playerPos.x + 8) < (posEnemy.x + SPIDER_WIDTH)) {
		enemyState = SpiderEnemyStates::ATTACK;
		startY = posEnemy.y;
	}

	glm::vec2 distance = glm::vec2(abs(playerPos.x - posEnemy.x), abs(playerPos.y - posEnemy.y));
	glm::vec2 mapSize = map->getSize();
	if (distance.x >= DESPAWN_BORDER || posEnemy.y >= mapSize.y - 48) {
		enemyState = SpiderEnemyStates::SPAWN;
		posEnemy = initPosEnemy;
		if (facingLeft) sprite->setScale(glm::vec2(-1.f, 1.f));
		else sprite->setScale(glm::vec2(1.f, 1.f));
	}
}

void SpiderEnemy::updateAttack(int deltaTime)
{
	if (sprite->animation() != ATTACK) {
		bGoingDown = true;
		sprite->changeAnimation(ATTACK);
	}

	if (bGoingDown) {
		translatePosition(glm::ivec2(0, FALL_SPEED));
		Object* obj = nullptr;
		if (map->collisionMoveDown(posEnemy, glm::ivec2(SPIDER_WIDTH, SPIDER_HEIGHT), SPIDER_HEIGHT, &posEnemy.y, &collisions[0], obj)) {
			stopTimer = STOP_TIME;
			bGoingDown = false;
		}
		obj = nullptr;
	}
	else {
		if (stopTimer < 0) translatePosition(glm::ivec2(0, -FALL_SPEED));
		if (posEnemy.y < startY) {
			posEnemy.y = startY;
			enemyState = SpiderEnemyStates::MOVE;
		}
	}
}

void SpiderEnemy::updateDie(int deltaTime)
{
	if (sprite->animation() != DIE) {
		sprite->changeAnimation(DIE);
		bJumping = true;
		jumpAngle = 60;
		startY = posEnemy.y + int(DEATH_JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.0f));
	}

	translatePosition(glm::ivec2(1, 0));
	if (bJumping)
	{
		jumpAngle += DEATH_JUMP_STEP;
		if (jumpAngle == 180)
		{
			bJumping = false;
			posEnemy.y = startY;
		}
		else
		{
			posEnemy.y = int(startY - DEATH_JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));
		}
	}
	else
	{
		translatePosition(glm::ivec2(0, DEATH_JUMP_STEP));
	}

	if (deathTimer < 0) {
		reset();
	}
}

void SpiderEnemy::updateSpawn(int deltaTime)
{
	glm::vec2 playerPos = player->getPosition();
	glm::vec2 distance = glm::vec2(abs(playerPos.x - posEnemy.x), abs(playerPos.y - posEnemy.y));
	if (distance.x <= SPAWN_BORDER - BORDER_SIZE && distance.y <= SPAWN_SIZE) playerInRange = true;
	else if ((distance.x <= SPAWN_BORDER) && (distance.y <= SPAWN_SIZE) && !playerInRange) {
		enemyState = SpiderEnemyStates::MOVE;
		posEnemy = initPosEnemy;
		if (facingLeft) sprite->setScale(glm::vec2(-1.f, 1.f));
		else sprite->setScale(glm::vec2(1.f, 1.f));
		playerInRange = true;
	}
	else if (distance.x > SPAWN_BORDER) playerInRange = false;
}

void SpiderEnemy::render()
{
	if (!isSpawn()) sprite->render();
}

void SpiderEnemy::reset()
{
	enemyState = SpiderEnemyStates::SPAWN;
	posEnemy = initPosEnemy;
	if (facingLeft) sprite->setScale(glm::vec2(-1.f, 1.f));
	else sprite->setScale(glm::vec2(1.f, 1.f));
	sprite->changeAnimation(MOVE);
}

void SpiderEnemy::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}

void SpiderEnemy::setPlayer(Player * playerPtr)
{
	player = playerPtr;
}

void SpiderEnemy::setPosition(const glm::vec2& pos)
{
	posEnemy = pos;
	initPosEnemy = posEnemy;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
}


void SpiderEnemy::setDefeat()
{
	deathTimer = DEATH_TIME;
	enemyState = SpiderEnemyStates::DIE;
}

glm::ivec2 SpiderEnemy::getSize()
{
	return glm::ivec2(SPIDER_WIDTH, SPIDER_HEIGHT);
}

glm::ivec2 SpiderEnemy::getPosition()
{
	return posEnemy;
}

bool SpiderEnemy::isSpawn()
{
	return (enemyState == SpiderEnemyStates::SPAWN);
}

bool SpiderEnemy::isAttacking()
{
	return (enemyState == SpiderEnemyStates::MOVE || enemyState == SpiderEnemyStates::ATTACK);
}

void SpiderEnemy::translatePosition(const glm::vec2& t) {
	posEnemy += t;
}