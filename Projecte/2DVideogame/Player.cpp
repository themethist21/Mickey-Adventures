#include <cmath>
#include <iostream>
#include <GL/glew.h>  
#include "Player.h"
#include "Game.h"
#include "Object.h"
#include <algorithm>


#define GAME_TIME 505.f
#define ENEMY_DEFEAT_SCORE 500

#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 64
#define FALL_STEP 4
#define JUMP_BUFFER_TIME 0.3f


#define PLAYER_WIDTH 32
#define PLAYER_HEIGHT 40

//Hitbox
#define HITBOX_PADDING_X 8
#define HITBOX_PADDING_Y 8
#define HITBOX_SIZE_X 16
#define HITBOX_SIZE_Y 32

#define HITBOX_PADDING_SMALL_Y 20
#define HITBOX_SIZE_SMALL_Y 20

//Trowing
#define THROW_COOLDOWN 0.2f
#define THROW_VELOCITY 6
#define DROP_VELOCITY 0
#define THROW_BUFFER_TIME 0.1f

//Run
#define ACCELERATION 0.4f
#define MAX_ACC 2.f
#define MAX_ANIM_SPEED 10.f
#define INIT_SPEED 1.f

//Smash
#define SMASH_ANGLE 0
#define SMASH_SPEED 2.f

//Lives
#define INIT_LIVES 3
#define INIT_TRIES 3
#define DEATH_TIMER 2.f
#define DAMAGE_TIME_OUT 1.6f
#define DAMAGE_FLICKER_TIME 0.1f
#define DAMAGE_KNOCKBACK_ANGLE 40
#define DAMAGE_KNOCKBACK_SPEED 1

#define KEY_BUFFER_TIME 0.2f;

enum PlayerAnimations
{
	STAND, MOVE, CROUCH, JUMP, FALL, SMASH, HOLD_STAND, HOLD_MOVE, HOLD_JUMP, HOLD_FALL, THROW, DAMAGE, DIE, NANIMS
};

enum PlayerCollisions
{
	OBJH, OBJD, ENEMY, NCOLLISIONS
};

void Player::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, ShaderProgram& hitboxShaderProgram)
{
	bJumping = false;
	spritesheet.loadFromFile("images/MM.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(PLAYER_WIDTH, PLAYER_HEIGHT), glm::vec2(float(PLAYER_WIDTH) / 488.f, float(PLAYER_HEIGHT) / 488.f), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(NANIMS); // Solo STAND y MOVE

	// Animacin STAND
	sprite->setAnimationSpeed(STAND, 2);
	float ini = 2.f / 488.f;
	sprite->addKeyframe(STAND, glm::vec2(0.f  + ini, 0.f));
	sprite->addKeyframe(STAND, glm::vec2(32.f / 488.f + ini, 0.f));

	// Animacion MOVE
	sprite->setAnimationSpeed(MOVE, 6);
	sprite->addKeyframe(MOVE, glm::vec2(64.f / 488.f + ini, 0.f));
	sprite->addKeyframe(MOVE, glm::vec2(96.f / 488.f + ini, 0.f));
	sprite->addKeyframe(MOVE, glm::vec2(128.f / 488.f + ini, 0.f));
	sprite->addKeyframe(MOVE, glm::vec2(160.f / 488.f + ini, 0.f));
	sprite->addKeyframe(MOVE, glm::vec2(192.f / 488.f + ini, 0.f));
	sprite->addKeyframe(MOVE, glm::vec2(224.f / 488.f + ini, 0.f));

	// Animacion CROUCH
	sprite->setAnimationSpeed(CROUCH, 2);
	sprite->addKeyframe(CROUCH, glm::vec2(0.f + ini, 48.f / 488.f));
	sprite->addKeyframe(CROUCH, glm::vec2(33.f / 488.f + ini, 48.f / 488.f));

	//Jump animation
	sprite->setAnimationSpeed(JUMP, 6);
	sprite->addKeyframe(JUMP, glm::vec2(64.f / 488.f + ini, 48.f / 488.f));

	sprite->setAnimationSpeed(FALL, 6);
	sprite->addKeyframe(FALL, glm::vec2(96.f / 488.f + ini, 48.f / 488.f));

	//Caure de cul
	sprite->setAnimationSpeed(SMASH, 6);
	sprite->addKeyframe(SMASH, glm::vec2(160.f / 488.f + ini, 48.f / 488.f));
	sprite->addKeyframe(SMASH, glm::vec2(192.f / 488.f + ini, 48.f / 488.f));

	//Damage
	sprite->setAnimationSpeed(DAMAGE, 0);
	sprite->addKeyframe(DAMAGE, glm::vec2(160.f / 488.f + ini, 320.f / 488.f));

	//Damage
	sprite->setAnimationSpeed(DIE, 1);
	sprite->addKeyframe(DIE, glm::vec2(256.f / 488.f + ini, 320.f / 488.f));
	sprite->addKeyframe(DIE, glm::vec2(224.f / 488.f + ini, 320.f / 488.f));

	//HOLD
	sprite->setAnimationSpeed(HOLD_STAND, 1);
	sprite->addKeyframe(HOLD_STAND, glm::vec2(128.f / 488.f + ini, 144.f / 488.f));
	sprite->addKeyframe(HOLD_STAND, glm::vec2(160.f / 488.f + ini, 144.f / 488.f));

	sprite->setAnimationSpeed(HOLD_MOVE, 6);
	sprite->addKeyframe(HOLD_MOVE, glm::vec2(0.f / 488.f + ini, 192.f / 488.f));
	sprite->addKeyframe(HOLD_MOVE, glm::vec2(32.f / 488.f + ini, 192.f / 488.f));
	sprite->addKeyframe(HOLD_MOVE, glm::vec2(64.f / 488.f + ini, 192.f / 488.f));
	sprite->addKeyframe(HOLD_MOVE, glm::vec2(96.f / 488.f + ini, 192.f / 488.f));
	sprite->addKeyframe(HOLD_MOVE, glm::vec2(128.f / 488.f + ini, 192.f / 488.f));
	sprite->addKeyframe(HOLD_MOVE, glm::vec2(160.f / 488.f + ini, 192.f / 488.f));

	sprite->setAnimationSpeed(HOLD_JUMP, 6);
	sprite->addKeyframe(HOLD_JUMP, glm::vec2(256.f / 488.f + ini, 192.f / 488.f));

	sprite->setAnimationSpeed(HOLD_FALL, 6);
	sprite->addKeyframe(HOLD_FALL, glm::vec2(288.f / 488.f + ini, 192.f / 488.f));

	//Throw
	sprite->setAnimationSpeed(THROW, 6);
	sprite->addKeyframe(THROW, glm::vec2(192.f / 488.f + ini, 144.f / 488.f));
	
	sprite->changeAnimation(STAND);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));

	showHitbox = false;
	prevF1KeyState = false;
	mov_acceleration_left = -1;
	mov_acceleration_right = 1;
	smashing = false;

	// Inicializar la hitbox
	hitboxPadding = glm::ivec2(HITBOX_PADDING_X, HITBOX_PADDING_Y);
	hitboxWidth = HITBOX_SIZE_X;
	hitboxHeight = HITBOX_SIZE_Y;
	updateHitbox();

	// Crear la hitbox
	hitbox = new Hitbox(glm::vec2(posHitbox.x, posHitbox.y), glm::vec2(hitboxWidth, hitboxHeight), &hitboxShaderProgram);
	
	lives = 3;
	tries = 3;

	gameTime = GAME_TIME;

	// Cargar el sonido
	if (!jumpsoundBuffer.loadFromFile("sounds/Jump.wav")) {
		std::cerr << "Error al cargar Jump.wav" << std::endl;
	}

	// Crear una instancia de sf::Sound y asignarle el buffer
	jumpEffect.setBuffer(jumpsoundBuffer);
	jumpEffect.setVolume(Game::SOUND_EFFECTS_VOLUME - 10);

	// Cargar el sonido
	if (!breakingsoundBuffer.loadFromFile("sounds/SFX/Ouch/Wav/Ouch__006.wav")) {
		std::cerr << "Error al cargar Ouch__006.wav" << std::endl;
	}
	// Crear una instancia de sf::Sound y asignarle el buffer
	breakingEffect.setBuffer(breakingsoundBuffer);
	breakingEffect.setVolume(Game::SOUND_EFFECTS_VOLUME + 30);

	// Cargar el sonido
	if (!healingBuffer.loadFromFile("sounds/SFX/Powerup/Wav/Powerup__006.wav")) {
		std::cerr << "Error al cargar Powerup__006.wav" << std::endl;
	}
	// Crear una instancia de sf::Sound y asignarle el buffer
	healingEffect.setBuffer(healingBuffer);
	healingEffect.setVolume(Game::SOUND_EFFECTS_VOLUME + 30);

	// Cargar el sonido
	if (!damagebuffer.loadFromFile("sounds/SFX/Ouch/Wav/Ouch__004.wav")) {
		std::cerr << "Error al cargar Ouch__004.wav" << std::endl;
	}
	// Crear una instancia de sf::Sound y asignarle el buffer
	damageeffect.setBuffer(damagebuffer);
	damageeffect.setVolume(Game::SOUND_EFFECTS_VOLUME + 10);

	// Cargar el sonido
	if (!coinbuffer.loadFromFile("sounds/Pickup_Coin.wav")) {
		std::cerr << "Error al cargar Pickup_Coin.wav" << std::endl;
	}
	// Crear una instancia de sf::Sound y asignarle el buffer
	coineffect.setBuffer(coinbuffer);
	coineffect.setVolume(Game::SOUND_EFFECTS_VOLUME + 10);

	// Cargar el sonido
	if (!deathbuffer.loadFromFile("sounds/death.wav")) {
		std::cerr << "Error al cargar death.wav" << std::endl;
	}
	// Crear una instancia de sf::Sound y asignarle el buffer
	deatheffect.setBuffer(deathbuffer);
	deatheffect.setVolume(Game::SOUND_EFFECTS_VOLUME + 10);

}

void Player::update(int deltaTime) {
	//Timers
	throwCooldown -= deltaTime / 1000.f;
	deathTimer -= deltaTime / 1000.f;
	damageTOTimer -= deltaTime / 1000.f;
	animBufferTimer -= deltaTime / 1000.f;
	keyBufferTimer -= deltaTime / 1000.f;
	jumpBufferTimer -= deltaTime / 1000.f;
	if (!isGameOver()) gameTime -= deltaTime / 1000.f;

	//Collisions
	for (int i = 0; i < NCOLLISIONS; ++i) collisions[i] = false;
	lastInteractableObj = nullptr;
	
	sprite->update(deltaTime);

	switch (playerState) {
	case PlayerStates::S_RUN: updateRun(deltaTime); break;
	case PlayerStates::S_CROUCH: updateCrouch(deltaTime); break;
	case PlayerStates::S_SMASH: updateSmashing(deltaTime); break;
	case PlayerStates::S_CARRY: updateCarry(deltaTime); break;
	case PlayerStates::S_DAMAGED: updateDamaged(deltaTime); break;
	case PlayerStates::S_DEAD: updateDead(deltaTime); break;
	}

	checkCollisions();

	if (Game::instance().getKey(GLFW_KEY_H) && keyBufferTimer < 0) {
		lives = INIT_LIVES;
		keyBufferTimer = KEY_BUFFER_TIME;
	}
	if (Game::instance().getKey(GLFW_KEY_G) && keyBufferTimer < 0) {
		godMode = !godMode;
		keyBufferTimer = KEY_BUFFER_TIME;
	}

	bool currentF1KeyState = Game::instance().getKey(GLFW_KEY_F1);
	if (currentF1KeyState && !prevF1KeyState) {
		showHitbox = !showHitbox; // Alternar la visualización de la hitbox
	}
	prevF1KeyState = currentF1KeyState;

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	updateHitbox();

	if (gameTime > 0) hud->setTime(int(gameTime));
	else {
		playerState = PlayerStates::S_DEAD;
		deathTimer = DEATH_TIMER;
		tries = 0;
	}
}

void Player::updateRun(int deltaTime)
{
	hitboxPadding.y = HITBOX_PADDING_Y, hitboxHeight = HITBOX_SIZE_Y;
		if (Game::instance().getKey(GLFW_KEY_A))
		{
			facingLeft = true;
			sprite->setScale(glm::vec2(-1.f, 1.f)); // Voltear horizontalmente
			mov_acceleration_right = 1;
			if (sprite->animation() != MOVE && !bJumping && animBufferTimer < 0)
			{
				updateHitbox();
				sprite->changeAnimation(MOVE);
			}
			translatePosition(glm::ivec2(-INIT_SPEED + max(mov_acceleration_left, -MAX_ACC), 0));	//aceleracion
			sprite->setAnimationSpeed(MOVE, min(6 + (-mov_acceleration_left), MAX_ANIM_SPEED)); //velocidad de animacion
			mov_acceleration_left -= ACCELERATION;

			if (map->collisionMoveLeft(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight), &collisions[OBJH], &posPlayer.x, lastInteractableObj))
			{
				mov_acceleration_left = -1;
				mov_acceleration_right = 1;
				updateHitbox();
				if (animBufferTimer < 0) sprite->changeAnimation(STAND);
			}
		}
		else if (Game::instance().getKey(GLFW_KEY_D))
		{
			facingLeft = false;
			sprite->setScale(glm::vec2(1.f, 1.f)); // Escala normal
			mov_acceleration_left = -1;
			if (sprite->animation() != MOVE && !bJumping && animBufferTimer < 0)
			{
				updateHitbox();
				sprite->changeAnimation(MOVE);
			}
			translatePosition(glm::ivec2(INIT_SPEED + min(mov_acceleration_right, MAX_ACC), 0));
			sprite->setAnimationSpeed(MOVE, min(6 + mov_acceleration_right, MAX_ANIM_SPEED));
			mov_acceleration_right += ACCELERATION;

			if (map->collisionMoveRight(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight), &collisions[OBJH], &posPlayer.x, lastInteractableObj))
			{
				mov_acceleration_left = -1;
				mov_acceleration_right = 1;
				updateHitbox();
				if (animBufferTimer < 0) sprite->changeAnimation(STAND);
			}
		}
		else if (Game::instance().getKey(GLFW_KEY_S) && !bJumping)
		{
			playerState = PlayerStates::S_CROUCH;
			if (sprite->animation() != CROUCH && animBufferTimer < 0) {
				mov_acceleration_left = -1;
				mov_acceleration_right = 1;
				updateHitbox();
				sprite->changeAnimation(CROUCH);

			}
		}
		else
		{
			if (sprite->animation() != STAND && animBufferTimer < 0) {
				mov_acceleration_left = -1;
				mov_acceleration_right = 1;
				updateHitbox();
				sprite->changeAnimation(STAND);
			}
		}
	

	if (bJumping)
	{
		jumpAngle += JUMP_ANGLE_STEP;
		//mov_acceleration_left = -1;
		//mov_acceleration_right = 1;
		if (Game::instance().getKey(GLFW_KEY_S))
		{
			playerState = PlayerStates::S_SMASH;
			if (sprite->animation() != SMASH) {
				updateHitbox();
				sprite->changeAnimation(SMASH);
				bJumping = false;
			}
		}
		else {
			if (jumpAngle == 180)
			{
				bJumping = false;
				posPlayer.y = startY;
				updateHitbox();
			}
			else
			{
				posPlayer.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));
				if (map->collisionMoveUp(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight), PLAYER_HEIGHT, &posPlayer.y)) {
					bJumping = false;
				}
				else {
					updateHitbox();
					if (jumpAngle > 90)
					{ // Falling
						if (sprite->animation() != FALL && animBufferTimer < 0) {
							updateHitbox();
							sprite->changeAnimation(FALL);
						}
						Object* obj = nullptr;
						bJumping = !map->collisionMoveDown(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight), PLAYER_HEIGHT, &posPlayer.y, &collisions[OBJD], obj);
						updateHitbox();
					}
					else
					{
						if (sprite->animation() != JUMP && animBufferTimer < 0) {
							updateHitbox();
							sprite->changeAnimation(JUMP);
						}

					}
				}
			}
		}
	}
	else
	{
		translatePosition(glm::ivec2(0, FALL_STEP));
		Object* obj = nullptr;
		bool condition = map->collisionMoveDown(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight), PLAYER_HEIGHT, &posPlayer.y, &collisions[OBJD], obj);
		updateHitbox();
		if (condition)
		{
			if (Game::instance().getKey(GLFW_KEY_W) && jumpBufferTimer < 0 && playerState == PlayerStates::S_RUN)
			{
				//mov_acceleration_left = -1;
				//mov_acceleration_right = 1;
				jumpEffect.play();
				jumpBufferTimer = JUMP_BUFFER_TIME;
				bJumping = true;
				jumpAngle = 0;
				startY = posPlayer.y;
			}
			else if (Game::instance().getKey(GLFW_KEY_SPACE) && playerState == PlayerStates::S_RUN) {
				mov_acceleration_left = -1;
				mov_acceleration_right = 1;
				if (lastInteractableObj != nullptr && collisions[OBJH] && lastInteractableObj->canBeMoved(posPlayer.y + PLAYER_HEIGHT)) {
					currentCarryObj = lastInteractableObj;
					currentCarryObj->setPos(glm::vec2(posPlayer.x + hitboxPadding.x, posPlayer.y + hitboxPadding.y - currentCarryObj->getSize() / 2.f));
					currentCarryObj->setHeld();
					throwCooldown = float(THROW_COOLDOWN);
					playerState = PlayerStates::S_CARRY;
				}
			}
		}
		else { //si estas cayendo
			if (Game::instance().getKey(GLFW_KEY_S))
			{
				playerState = PlayerStates::S_SMASH;
				if (sprite->animation() != SMASH) {
					updateHitbox();
					sprite->changeAnimation(SMASH);
				}
			}
			if (sprite->animation() != FALL && animBufferTimer < 0) {
				updateHitbox();
				sprite->changeAnimation(FALL);
			}
		}
	}
}

void Player::updateCrouch(int deltaTime)
{
	hitboxPadding.y = HITBOX_PADDING_SMALL_Y, hitboxHeight = HITBOX_SIZE_SMALL_Y;
	if (!Game::instance().getKey(GLFW_KEY_S))
	{
		playerState = PlayerStates::S_RUN;
	}
	else {
		if (sprite->animation() != CROUCH) {
			mov_acceleration_left = -1;
			mov_acceleration_right = 1;
			updateHitbox();
			sprite->changeAnimation(CROUCH);

		}
	}
}

void Player::updateSmashing(int deltaTime) {

	hitboxPadding.y = HITBOX_PADDING_SMALL_Y, hitboxHeight = HITBOX_SIZE_SMALL_Y;

	if (Game::instance().getKey(GLFW_KEY_A)) {
		facingLeft = true;
		sprite->setScale(glm::vec2(-1.f, 1.f)); // Voltear horizontalmente
		translatePosition(glm::ivec2(-SMASH_SPEED, 0));
		if (map->collisionMoveLeft(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight), &collisions[OBJH], &posPlayer.x, lastInteractableObj))
		{
			mov_acceleration_left = -1;
			mov_acceleration_right = 1;
			
			updateHitbox();
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_D)) {
		facingLeft = false;
		sprite->setScale(glm::vec2(1.f, 1.f)); // Escala normal
		translatePosition(glm::ivec2(SMASH_SPEED, 0));
		if (map->collisionMoveRight(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight), &collisions[OBJH], &posPlayer.x, lastInteractableObj))
		{
			mov_acceleration_left = -1;
			mov_acceleration_right = 1;
			updateHitbox();
		}
	}

	if (bJumping)
	{
		jumpAngle += JUMP_ANGLE_STEP;
		//mov_acceleration_left = -1;
		//mov_acceleration_right = 1;

		if (jumpAngle == 180)
		{
			bJumping = false;
			posPlayer.y = startY;
			updateHitbox();
		}
		else
		{
			posPlayer.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));
			updateHitbox();
			if (map->collisionMoveUp(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight), PLAYER_HEIGHT, &posPlayer.y)) {
				bJumping = false;
			}
			else {
				if (jumpAngle > 90)
				{ // Falling

					if (map->collisionMoveDown(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight), PLAYER_HEIGHT, &posPlayer.y, &collisions[OBJD], lastInteractableObj)) {
						if (collisions[OBJD]) {
							//mov_acceleration_left = -1;
							//mov_acceleration_right = 1;
							bJumping = true;
							jumpAngle = SMASH_ANGLE;
							startY = posPlayer.y + int(96 * sin(3.14159f * jumpAngle / 180.0f));
							lastInteractableObj->setDestroy();
							breakingEffect.play();
						}
						else {
							bJumping = false;
							playerState = PlayerStates::S_RUN;
						}
					}
					else if (map->collisionEnemyDamaging(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight))) {
						bJumping = true;
						jumpAngle = SMASH_ANGLE;
						startY = posPlayer.y + int(96 * sin(3.14159f * jumpAngle / 180.0f));
						score += ENEMY_DEFEAT_SCORE;
						hud->setScore(score);
						breakingEffect.play();
					}
					updateHitbox();
				}
			}
		}
	}
	else
	{
		translatePosition(glm::ivec2(0, FALL_STEP));
		bool condition = map->collisionMoveDown(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight), PLAYER_HEIGHT, &posPlayer.y, &collisions[OBJD], lastInteractableObj);
		updateHitbox();
		if (condition)
		{
			if (collisions[OBJD]) {
				//mov_acceleration_left = -1;
				//mov_acceleration_right = 1;
				bJumping = true;
				jumpAngle = SMASH_ANGLE;
				startY = posPlayer.y + int(JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.0f));;
				lastInteractableObj->setDestroy();
				breakingEffect.play();
			}
			else playerState = PlayerStates::S_RUN;
		}
		else if (map->collisionEnemyDamaging(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight))) {
			bJumping = true;
			jumpAngle = SMASH_ANGLE;
			startY = posPlayer.y + int(96 * sin(3.14159f * jumpAngle / 180.0f));
			score += ENEMY_DEFEAT_SCORE;
			hud->setScore(score);
			breakingEffect.play();
		}
	}
}

void Player::updateCarry(int deltaTime)
{
	hitboxPadding.y = HITBOX_PADDING_Y, hitboxHeight = HITBOX_SIZE_Y;
		if (Game::instance().getKey(GLFW_KEY_A))
		{
			facingLeft = true;
			sprite->setScale(glm::vec2(-1.f, 1.f)); // Voltear horizontalmente
			mov_acceleration_right = 1;
			if (sprite->animation() != HOLD_MOVE && !bJumping)
			{
				
				updateHitbox();
				//mov_acceleration_left = -1;
				sprite->changeAnimation(HOLD_MOVE);
			}
			translatePosition(glm::ivec2(-INIT_SPEED + max(mov_acceleration_left, -MAX_ACC), 0));	//aceleracion
			sprite->setAnimationSpeed(HOLD_MOVE, min(6 + (-mov_acceleration_left), MAX_ANIM_SPEED)); //velocidad de animacion
			mov_acceleration_left -= ACCELERATION;

			if (map->collisionMoveLeft(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight), &collisions[OBJH], &posPlayer.x, lastInteractableObj))
			{
				mov_acceleration_left = -1;
				mov_acceleration_right = 1;
				updateHitbox();
				sprite->changeAnimation(HOLD_STAND);
			}
		}
		else if (Game::instance().getKey(GLFW_KEY_D))
		{
			facingLeft = false;
			sprite->setScale(glm::vec2(1.f, 1.f)); // Escala normal
			mov_acceleration_left = -1;
			if (sprite->animation() != HOLD_MOVE && !bJumping)
			{
				updateHitbox();
				//mov_acceleration_right = 1;
				sprite->changeAnimation(HOLD_MOVE);
			}
			translatePosition(glm::ivec2(INIT_SPEED + min(mov_acceleration_right, MAX_ACC), 0));
			sprite->setAnimationSpeed(HOLD_MOVE, min(6 + mov_acceleration_right, MAX_ANIM_SPEED));
			mov_acceleration_right += ACCELERATION;

			if (map->collisionMoveRight(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight), &collisions[OBJH], &posPlayer.x, lastInteractableObj))
			{
				mov_acceleration_left = -1;
				mov_acceleration_right = 1;
				updateHitbox();
				sprite->changeAnimation(HOLD_STAND);
			}
		}
		else
		{
			if (sprite->animation() != HOLD_STAND) {
				mov_acceleration_left = -1;
				mov_acceleration_right = 1;
				updateHitbox();
				sprite->changeAnimation(HOLD_STAND);
			}
		}
	bool grounded = false;
	if (bJumping)
	{
		jumpAngle += JUMP_ANGLE_STEP;
		//mov_acceleration_left = -1;
		//mov_acceleration_right = 1;

			if (jumpAngle == 180)
			{
				bJumping = false;
				posPlayer.y = startY;
				updateHitbox();
			}
			else
			{
				posPlayer.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));
				updateHitbox();
				if (map->collisionMoveUp(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight), PLAYER_HEIGHT, &posPlayer.y)) {
					bJumping = false;
				}
				else {
					if (jumpAngle > 90)
					{ // Falling
						if (sprite->animation() != HOLD_FALL) {
							updateHitbox();
							sprite->changeAnimation(HOLD_FALL);
						}

						grounded = map->collisionMoveDown(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight), PLAYER_HEIGHT, &posPlayer.y, &collisions[OBJD], lastInteractableObj);
						bJumping = !grounded;
						updateHitbox();
					}
					else
					{
						if (sprite->animation() != HOLD_JUMP) {
							updateHitbox();
							sprite->changeAnimation(HOLD_JUMP);
						}

					}
				}
			}
	}
	else
	{
		translatePosition(glm::ivec2(0, FALL_STEP));
		grounded = map->collisionMoveDown(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight), PLAYER_HEIGHT, &posPlayer.y, &collisions[OBJD], lastInteractableObj);
		updateHitbox();
		if (grounded)
		{
			if (Game::instance().getKey(GLFW_KEY_W) && jumpBufferTimer < 0)
			{
				//mov_acceleration_left = -1;
				//mov_acceleration_right = 1;
				jumpEffect.play();
				jumpBufferTimer = JUMP_BUFFER_TIME;
				bJumping = true;
				jumpAngle = 0;
				startY = posPlayer.y;
			}
		}
		else {
			if (sprite->animation() != HOLD_FALL && animBufferTimer < 0) {
				updateHitbox();
				sprite->changeAnimation(HOLD_FALL);
			}
		}
	}

		if (Game::instance().getKey(GLFW_KEY_SPACE) && throwCooldown < 0) {
			//mov_acceleration_left = -1;
			//mov_acceleration_right = 1;
			currentCarryObj->setMoving();
			if (mov_acceleration_left < -1 || mov_acceleration_right > 1) {
				if (facingLeft) currentCarryObj->throwObject(glm::vec2(float(-THROW_VELOCITY), 0.f));
				else currentCarryObj->throwObject(glm::vec2(float(THROW_VELOCITY), 0.f));
			}
			else {
				if (facingLeft) {
					currentCarryObj->setPos(glm::vec2(posPlayer.x + HITBOX_PADDING_X - HITBOX_SIZE_X, posPlayer.y));
					currentCarryObj->throwObject(glm::vec2(float(-DROP_VELOCITY), 0.f));
				}
				else {
					currentCarryObj->setPos(glm::vec2(posPlayer.x + HITBOX_PADDING_X + HITBOX_SIZE_X, posPlayer.y));
					currentCarryObj->throwObject(glm::vec2(float(DROP_VELOCITY), 0.f));
				}
			}
			
			currentCarryObj = nullptr;
			playerState = PlayerStates::S_RUN;

			if (sprite->animation() != THROW) {
				sprite->changeAnimation(THROW);
				animBufferTimer = THROW_BUFFER_TIME;
			}
		}
		else {
			if (facingLeft) currentCarryObj->setPos(glm::vec2(posPlayer.x + hitboxPadding.x - 4, posPlayer.y + hitboxPadding.y - currentCarryObj->getSize() / 2.f));
			else currentCarryObj->setPos(glm::vec2(posPlayer.x + hitboxPadding.x + 4, posPlayer.y + hitboxPadding.y - currentCarryObj->getSize() / 2.f));
		}
}

void Player::updateDamaged(int deltaTime)
{
	if (!bJumping) {
		mov_acceleration_left = -1;
		mov_acceleration_right = 1;
		bJumping = true;
		jumpAngle = DAMAGE_KNOCKBACK_ANGLE;
		startY = posPlayer.y + int(JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.0f));
		sprite->changeAnimation(DAMAGE);
		damageeffect.play();
	}

	if (jumpAngle < 90) {
		if (facingLeft) {
			translatePosition(glm::ivec2(+SMASH_SPEED, 0));
			map->collisionMoveRight(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight), &collisions[OBJH], &posPlayer.x, lastInteractableObj);
		}
		else {
			translatePosition(glm::ivec2(-SMASH_SPEED, 0));
			map->collisionMoveLeft(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight), &collisions[OBJH], &posPlayer.x, lastInteractableObj);
		}

		jumpAngle += JUMP_ANGLE_STEP;
		posPlayer.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));
		updateHitbox();
	}
	else {
		bJumping = false;
		if (lives <= 0) {
			playerState = PlayerStates::S_DEAD;
			deathTimer = DEATH_TIMER;
		}
		else {
			playerState = PlayerStates::S_RUN;
		}
	}
}

void Player::updateDead(int deltaTime) { 
	if (sprite->animation() != DIE) {
		deatheffect.play();
		sprite->changeAnimation(DIE);
		mov_acceleration_left = -1;
		mov_acceleration_right = 1;
		bJumping = true;
		jumpAngle = 0;
		startY = posPlayer.y ;
	}

	if (bJumping)
	{
		jumpAngle += JUMP_ANGLE_STEP;
			if (jumpAngle == 180)
			{
				bJumping = false;
				posPlayer.y = startY;
			}
			else
			{
				posPlayer.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));
			}
	}
	else
	{
		translatePosition(glm::ivec2(0, FALL_STEP));
	}

	if (deathTimer < 0) {
		--tries;
		if (tries > 0) {
			hud->setTries(tries);
			posPlayer = checkpoint;
			bJumping = false;
			facingLeft = false;
			mov_acceleration_left = -1;
			mov_acceleration_right = 1;
			playerState = PlayerStates::S_RUN;
			lives = INIT_LIVES;
			hud->setLife(lives);
			map->resetEnemies();
		}
		else if (tries == 0) {
			hud->setTries(tries);
			playerState = PlayerStates::S_GAMEOVER;
		}
	}
}

void Player::checkCollisions()
{

	if (!godMode && map->collisionEnemy(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight)) && damageTOTimer < 0 && playerState != PlayerStates::S_DEAD) {
		--lives;
		playerState = PlayerStates::S_DAMAGED;
		damageTOTimer = DAMAGE_TIME_OUT;
		bJumping = false;

		if (currentCarryObj != nullptr) {
			currentCarryObj->setMoving();
			if (facingLeft) {
				currentCarryObj->setPos(glm::vec2(posPlayer.x + HITBOX_PADDING_X - HITBOX_SIZE_X, posPlayer.y));
				currentCarryObj->throwObject(glm::vec2(float(-DROP_VELOCITY), 0.f));
			}
			else {
				currentCarryObj->setPos(glm::vec2(posPlayer.x + HITBOX_PADDING_X + HITBOX_SIZE_X, posPlayer.y));
				currentCarryObj->throwObject(glm::vec2(float(DROP_VELOCITY), 0.f));
			}
			currentCarryObj = nullptr;
		}
	}
	bool win = false;
	int livesbefore = lives;
	int scorebefore = score;
	map->collisionItems(posHitbox, glm::ivec2(hitboxWidth, hitboxHeight), &lives, &score, &win);
	if (lives >= 0) hud->setLife(lives);
	if (livesbefore < lives) {
		healingEffect.play();
	}
	if (scorebefore < score) {
		coineffect.play();
	}
	hud->setScore(score);
	if (win == true) {
		Game::instance().beatCurrentStage();
	}

	glm::vec2 mapSize = map->getSize();
	if (posPlayer.y >= mapSize.y - 48 && playerState != PlayerStates::S_DEAD) {
		playerState = PlayerStates::S_DEAD;
		deathTimer = DEATH_TIMER;
		if (currentCarryObj != nullptr) {
			currentCarryObj->setMoving();
			if (facingLeft) {
				currentCarryObj->setPos(glm::vec2(posPlayer.x + HITBOX_PADDING_X - HITBOX_SIZE_X, posPlayer.y));
				currentCarryObj->throwObject(glm::vec2(float(-DROP_VELOCITY), 0.f));
			}
			else {
				currentCarryObj->setPos(glm::vec2(posPlayer.x + HITBOX_PADDING_X + HITBOX_SIZE_X, posPlayer.y));
				currentCarryObj->throwObject(glm::vec2(float(DROP_VELOCITY), 0.f));
			}
			currentCarryObj = nullptr;
		}
	}
}

void Player::render()
{
	if ((damageTOTimer >= 0) && playerState != PlayerStates::S_DEAD) {
		if (int(damageTOTimer/DAMAGE_FLICKER_TIME)%2 == 0) {
			sprite->render();
		}
	}
	else sprite->render();
	if (showHitbox && hitbox != nullptr) {
		hitbox->render();
	}
}

void Player::reset()
{
	sprite->changeAnimation(STAND);

	showHitbox = false;
	prevF1KeyState = false;
	mov_acceleration_left = -1;
	mov_acceleration_right = 1;
	smashing = false;

	// Inicializar la hitbox
	hitboxPadding = glm::ivec2(HITBOX_PADDING_X, HITBOX_PADDING_Y);
	hitboxWidth = HITBOX_SIZE_X;
	hitboxHeight = HITBOX_SIZE_Y;
	updateHitbox();


	lives = 3;
	tries = 3;
	hud->setLife(lives);
	hud->setTries(tries);

	gameTime = GAME_TIME;
	score = 0;
	hud->setScore(score);
	playerState = PlayerStates::S_RUN;
}

void Player::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}

void Player::setHud(HUD * playerHud)
{
	hud = playerHud;
	hud->setLife(lives);
	hud->setTries(tries);
}

void Player::setPosition(const glm::vec2& pos)
{
	posPlayer = pos;
	posHitbox = glm::ivec2( float(posPlayer.x + hitboxPadding.x),float(posPlayer.y + hitboxPadding.y));
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::setCheckpoint(const glm::vec2 &pos) {
	checkpoint = pos;
}

void Player::updateHitbox() {
	posHitbox = glm::ivec2(posPlayer.x + hitboxPadding.x, posPlayer.y + hitboxPadding.y);
	if (hitbox != nullptr) {
		hitbox->setPosition(glm::vec2(float(tileMapDispl.x + posHitbox.x), float(tileMapDispl.y + posHitbox.y)));
		hitbox->setSize(glm::vec2(hitboxWidth, hitboxHeight));
	}
}

void Player::translatePosition(const glm::vec2& t) {
	posPlayer += t;
	posHitbox += t;
}


glm::ivec2 Player::getPosition() {
	return posPlayer + tileMapDispl;
}

bool Player::isGameOver()
{
	return (tries == 0);
}
