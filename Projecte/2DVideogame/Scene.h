#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "HUD.h"
#include "Sprite.h"



// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();
	~Scene();

	void reset();
	void init(TextRenderer& tr, string mapPath, const glm::ivec2 &playerPos);
	void update(int deltaTime);
	void render();
	void pause();

private:
	void initShaders();
	void updateCamera();
	int getCameraLevel(const glm::vec2 &pos);

private:
	TileMap* map;
	Player *player;
	HUD *hud;
	ShaderProgram texProgram, hitboxProgram;
	float currentTime;
	glm::mat4 projection;
	glm::vec2 camPosition;
	glm::ivec2 initPlayerPos;
	Sprite* gameMenu;
	Sprite* gameMenuButton;
	Texture gameMenuSpritesheet, gameMenuButtonsSpritesheet;
	string levelPath;

	bool pendingCamUpdate;
	bool gameOver = false;
	bool paused = false;
	int menuOption = 0;
	float buttonBufferTime = 0;

	int currentCamLevel;
};


#endif // _SCENE_INCLUDE

