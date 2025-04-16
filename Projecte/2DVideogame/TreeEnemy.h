#ifndef _TREE_ENEMY_INCLUDE
#define _TREE_ENEMY_INCLUDE


#include "Sprite.h"
#include "TileMap.h"
#include "Hitbox.h"
#include <vector>
#include <SFML/Audio.hpp>




class Player;
enum class EnemyStates
{
	ATTACK, DIE, SPAWN, NSTATES
};

class TreeEnemy
{
public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
	void update(int deltaTime);
	void updateAttack(int deltaTime);
	void updateDie(int deltaTime);
	void updateSpawn(int deltaTime);
	void render();
	void reset();

	void setTileMap(TileMap* tileMap);
	void setPlayer(Player* playerPtr);
	void setPosition(const glm::vec2& pos);
	void setFacingLeft(bool faceLeft);
	void setDefeat();

	glm::ivec2 getSize();
	glm::ivec2 getPosition();
	
	bool isSpawn();
	bool isAttacking();
private:

	void translatePosition(const glm::vec2& t);

	glm::ivec2 tileMapDispl, posTree, initPosTree;
	Texture spritesheet;
	Sprite* sprite;
	TileMap* map;
	Player* player;
	EnemyStates enemyState = EnemyStates::SPAWN;

	int jumpAngle, startY;

	bool facingLeft, initFacingLeft;
	bool playerInRange = false;
	bool bJumping = false;

	float deathTimer;

	//Hitbox
	bool showHitbox;
	bool prevF1KeyState;

	//Collisions
	bool collisions[1];

	sf::SoundBuffer dieBuffer;
	sf::Sound dieEffect;
};

#endif // _TREE_ENEMY_INCLUD