#ifndef _SPIDER_ENEMY_INCLUDE
#define _SPIDER_ENEMY_INCLUDE


#include "Sprite.h"
#include "TileMap.h"
#include "Hitbox.h"
#include <vector>


class Player;
enum class SpiderEnemyStates
{
	ATTACK, MOVE, DIE, SPAWN, NSTATES
};

class SpiderEnemy
{
public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
	void update(int deltaTime);
	void updateMove(int deltaTime);
	void updateAttack(int deltaTime);
	void updateDie(int deltaTime);
	void updateSpawn(int deltaTime);
	void render();
	void reset();

	void setTileMap(TileMap* tileMap);
	void setPlayer(Player* playerPtr);
	void setPosition(const glm::vec2& pos);
	void setDefeat();

	glm::ivec2 getSize();
	glm::ivec2 getPosition();

	bool isSpawn();
	bool isAttacking();
private:

	void translatePosition(const glm::vec2& t);

	glm::ivec2 tileMapDispl, posEnemy, initPosEnemy;
	Texture spritesheet;
	Sprite* sprite;
	TileMap* map;
	Player* player;
	SpiderEnemyStates enemyState = SpiderEnemyStates::SPAWN;

	int jumpAngle, startY;

	bool facingLeft;
	bool playerInRange = false;
	bool bJumping = false;
	bool bGoingDown = false;

	float deathTimer;
	float stopTimer;

	//Hitbox
	bool showHitbox;
	bool prevF1KeyState;

	//Collisions
	bool collisions[1];
};

#endif // _SPIDER_ENEMY_INCLUDE