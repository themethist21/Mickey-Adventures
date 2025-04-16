#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"
#include "Hitbox.h"
#include "HUD.h"
#include <vector>
#include <SFML/Audio.hpp>


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.

enum class PlayerStates
{
	S_RUN, S_CROUCH, S_SMASH, S_CARRY, S_DAMAGED, S_DEAD, S_GAMEOVER, NSTATES
};

class Player
{

public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, ShaderProgram& hitboxShaderProgram);
	void update(int deltaTime);
	void updateRun(int deltaTime);
	void updateCrouch(int deltaTime);
	void updateSmashing(int deltaTime);
	void updateCarry(int deltaTime);
	void updateDamaged(int deltaTime);
	void updateDead(int deltaTime);
	void checkCollisions();
	void render();
	void reset();

	void setTileMap(TileMap* tileMap);
	void setHud(HUD* playerHud);
	void setPosition(const glm::vec2& pos);
	void setCheckpoint(const glm::vec2 & pos);
	void translatePosition(const glm::vec2& t);
	void updateHitbox();
	glm::ivec2 getPosition();

	bool isGameOver();

private:
	bool bJumping;
	glm::ivec2 tileMapDispl, posPlayer, posHitbox, hitboxPadding;
	int jumpAngle, startY, hitboxWidth = 20, hitboxHeight = 40;
	float mov_acceleration_left, mov_acceleration_right;
	float horizontalSpeed;
	bool smashing;
	Texture spritesheet;
	Sprite* sprite;
	TileMap* map;
	Hitbox* hitbox;
	HUD* hud;

	//Player states
	PlayerStates playerState = PlayerStates::S_RUN;
	bool facingLeft;
	bool carryObj = false;
	bool godMode = false;
	bool showHitbox;
	bool prevF1KeyState;

	//Collisions
	bool collisions[2];
	Object* lastInteractableObj;
	Object* currentCarryObj;

	// Timers
	float throwCooldown = 0;
	float deathTimer = 0;
	float damageTOTimer = 0;
	float animBufferTimer = 0;
	float keyBufferTimer = 0;
	float jumpBufferTimer = 0;
	float gameTime = 0;

	// Vidas
	int lives, tries, score = 0;
	glm::vec2 checkpoint;

	sf::SoundBuffer jumpsoundBuffer, breakingsoundBuffer, healingBuffer, damagebuffer, coinbuffer, deathbuffer;
	sf::Sound jumpEffect, breakingEffect, healingEffect, damageeffect, coineffect, deatheffect;
};


#endif // _PLAYER_INCLUDE


