#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define SCREEN_X 0
#define SCREEN_Y 0

#define SCENE_WIDTH 352
#define SCENE_HEIGHT 198

#define GAME_MENU_WIDTH 128
#define GAME_MENU_HEIGHT 72
#define GAME_MENU_POS_X 112
#define GAME_MENU_POS_Y 60

#define GAME_MENU_BUTTON_WIDTH 8
#define GAME_MENU_BUTTON_HEIGHT 43
#define GAME_MENU_BUTTON_POS_X 218
#define GAME_MENU_BUTTON_POS_Y 72
#define BUTTON_BUFFER_TIME 0.2f

Scene::Scene()
{
	map = NULL;
	player = NULL;
	pendingCamUpdate = false;
}

Scene::~Scene()
{
	if(map != NULL)
		delete map;
	if(player != NULL)
		delete player;
}


void Scene::init(TextRenderer& tr, string mapPath, const glm::ivec2 &playerPos)
{
	levelPath = mapPath;
	initPlayerPos = playerPos;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	initShaders();
	player = new Player();

	map = TileMap::createTileMap(mapPath, glm::vec2(SCREEN_X, SCREEN_Y), texProgram, player);

	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, hitboxProgram);
	player->setPosition(glm::vec2(initPlayerPos.x * map->getTileSize(), initPlayerPos.y * map->getTileSize()));
	player->setCheckpoint(glm::vec2(initPlayerPos.x * map->getTileSize(), initPlayerPos.y * map->getTileSize()));
	player->setTileMap(map);
	camPosition = glm::vec2(0.0f, 96.0f);
	projection = glm::ortho(camPosition.x, float(SCENE_WIDTH) + camPosition.x, float(SCENE_HEIGHT) + camPosition.y, camPosition.y);
	currentTime = 0.0f;
	currentCamLevel = 0;

	hud = new HUD();
	hud->init(texProgram, glm::vec2(camPosition.x, camPosition.y + SCENE_HEIGHT - 24));
	player->setHud(hud);
	hud->setTextRenderer(tr);

	gameMenuSpritesheet.loadFromFile("images/MenuHud.png", TEXTURE_PIXEL_FORMAT_RGBA);
	gameMenu = Sprite::createSprite(glm::ivec2(GAME_MENU_WIDTH, GAME_MENU_HEIGHT), glm::vec2(1.f, 1.f), &gameMenuSpritesheet, &texProgram);
	gameMenu->setNumberAnimations(1);
	gameMenu->setAnimationSpeed(0, 0);
	gameMenu->addKeyframe(0, glm::vec2(0.f, 0.f));
	gameMenu->changeAnimation(0);
	gameMenu->setPosition(glm::vec2(camPosition.x + GAME_MENU_POS_X, camPosition.y + GAME_MENU_POS_Y));

	gameMenuButtonsSpritesheet.loadFromFile("images/MenuHudButtons.png", TEXTURE_PIXEL_FORMAT_RGBA);
	gameMenuButton = Sprite::createSprite(glm::ivec2(GAME_MENU_BUTTON_WIDTH, GAME_MENU_BUTTON_HEIGHT), glm::vec2(0.5f, 1.f), &gameMenuButtonsSpritesheet, &texProgram);
	gameMenuButton->setNumberAnimations(2);
	gameMenuButton->setAnimationSpeed(0, 0);
	gameMenuButton->addKeyframe(0, glm::vec2(0.f, 0.f));
	gameMenuButton->setAnimationSpeed(1, 0);
	gameMenuButton->addKeyframe(1, glm::vec2(0.5f, 0.f));
	gameMenuButton->changeAnimation(0);
	gameMenuButton->setPosition(glm::vec2(camPosition.x + GAME_MENU_BUTTON_POS_X, camPosition.y + GAME_MENU_BUTTON_POS_Y));
}

void Scene::reset()
{
	delete map;
	map = TileMap::createTileMap(levelPath, glm::vec2(SCREEN_X, SCREEN_Y), texProgram, player);
	player->setPosition(glm::vec2(initPlayerPos.x * map->getTileSize(), initPlayerPos.y * map->getTileSize()));
	player->setCheckpoint(glm::vec2(initPlayerPos.x * map->getTileSize(), initPlayerPos.y * map->getTileSize()));
	player->setTileMap(map);
	player->reset();
	camPosition = glm::vec2(0.0f, 96.0f);
	projection = glm::ortho(camPosition.x, float(SCENE_WIDTH) + camPosition.x, float(SCENE_HEIGHT) + camPosition.y, camPosition.y);

	hud->setPos(glm::vec2(camPosition.x, camPosition.y + SCENE_HEIGHT - 24));
	gameMenu->setPosition(glm::vec2(camPosition.x + GAME_MENU_POS_X, camPosition.y + GAME_MENU_POS_Y));
	gameMenuButton->setPosition(glm::vec2(camPosition.x + GAME_MENU_BUTTON_POS_X, camPosition.y + GAME_MENU_BUTTON_POS_Y));

	currentTime = 0.0f;
	currentCamLevel = 0;
	gameOver = false;
	paused = false;
}

void Scene::update(int deltaTime)
{
	buttonBufferTime -= deltaTime / 1000.f;
	gameOver = player->isGameOver();

	if (!paused && !gameOver) {
		currentTime += deltaTime;
		player->update(deltaTime);
		map->update(deltaTime);
	}
	updateCamera();
}

void Scene::render()
{
	glm::mat4 modelview;

    glClearColor(0.4f, 0.7f, 7.0f, 1.0f);

    texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

	hitboxProgram.use();
    hitboxProgram.setUniformMatrix4f("projection", projection);

    texProgram.use();
    map->render();

	player->render();

	hud->render();

	if (gameOver || paused) {
		gameMenu->render();
		gameMenuButton->render();

		if (buttonBufferTime < 0  &&(Game::instance().getKey(GLFW_KEY_UP) || Game::instance().getKey(GLFW_KEY_DOWN))) {
			buttonBufferTime = BUTTON_BUFFER_TIME;
			menuOption += 1;
			if (menuOption > 1) menuOption = 0;
			gameMenuButton->changeAnimation(menuOption);
		}
		if (Game::instance().getKey(GLFW_KEY_ENTER)) {
			if (menuOption == 0) reset();
			else if (menuOption == 1) {
				reset();
				Game::instance().setMenu();
			}
		}
	}
}

void Scene::pause()
{
	if (buttonBufferTime < 0 && !gameOver) {
		paused = !paused;
		buttonBufferTime = BUTTON_BUFFER_TIME;
	}
}

void Scene::initShaders()
{
    Shader vShader, fShader;

    // Shader para texturas (el existente)
    vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
    if (!vShader.isCompiled())
    {
        cout << "Vertex Shader Error" << endl;
        cout << "" << vShader.log() << endl << endl;
    }
    fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
    if (!fShader.isCompiled())
    {
        cout << "Fragment Shader Error" << endl;
        cout << "" << fShader.log() << endl << endl;
    }
    texProgram.init();
    texProgram.addShader(vShader);
    texProgram.addShader(fShader);
    texProgram.link();
    if (!texProgram.isLinked())
    {
        cout << "Shader Linking Error" << endl;
        cout << "" << texProgram.log() << endl << endl;
    }
    texProgram.bindFragmentOutput("outColor");
    vShader.free();
    fShader.free();

    // Shader para la hitbox (nuevo)
    Shader hitboxVShader, hitboxFShader;

    hitboxVShader.initFromFile(VERTEX_SHADER, "shaders/simple.vert");
    if (!hitboxVShader.isCompiled())
    {
        cout << "Hitbox Vertex Shader Error" << endl;
        cout << "" << hitboxVShader.log() << endl << endl;
    }
    hitboxFShader.initFromFile(FRAGMENT_SHADER, "shaders/simple.frag");
    if (!hitboxFShader.isCompiled())
    {
        cout << "Hitbox Fragment Shader Error" << endl;
        cout << "" << hitboxFShader.log() << endl << endl;
    }
    hitboxProgram.init();
    hitboxProgram.addShader(hitboxVShader);
    hitboxProgram.addShader(hitboxFShader);
    hitboxProgram.link();
    if (!hitboxProgram.isLinked())
    {
        cout << "Hitbox Shader Linking Error" << endl;
        cout << "" << hitboxProgram.log() << endl << endl;
    }
    hitboxProgram.bindFragmentOutput("outColor");
    hitboxVShader.free();
    hitboxFShader.free();
}


void Scene::updateCamera()
{
	glm::ivec2 pos = player->getPosition();
	glm::vec2 mapSize = map->getSize();

	int level = getCameraLevel(pos);

	if (level != currentCamLevel && (pos.y + 64) < mapSize.y) {
		camPosition.y = level * 224 + 96;
		currentCamLevel = level;
		hud->setPos(glm::vec2(camPosition.x, camPosition.y + SCENE_HEIGHT - 24));
		gameMenu->setPosition(glm::vec2(camPosition.x + GAME_MENU_POS_X, camPosition.y + GAME_MENU_POS_Y));
		gameMenuButton->setPosition(glm::vec2(camPosition.x + GAME_MENU_BUTTON_POS_X, camPosition.y + GAME_MENU_BUTTON_POS_Y));
	}

	if ((pos.x - camPosition.x) < (float(SCENE_WIDTH) / 3.f) && camPosition.x > 0) {
		camPosition.x = float(pos.x) - float(SCENE_WIDTH) / 3.f;
		if (camPosition.x < 0) camPosition.x = 0;
		hud->setPos(glm::vec2(camPosition.x, camPosition.y + SCENE_HEIGHT - 24));
		gameMenu->setPosition(glm::vec2(camPosition.x + GAME_MENU_POS_X, camPosition.y + GAME_MENU_POS_Y));
		gameMenuButton->setPosition(glm::vec2(camPosition.x + GAME_MENU_BUTTON_POS_X, camPosition.y + GAME_MENU_BUTTON_POS_Y));
	}
	if ((pos.x - camPosition.x) > 2*(float(SCENE_WIDTH) / 3.f) && camPosition.x < (mapSize.x - SCENE_WIDTH)) {
		camPosition.x = float(pos.x) - 2*(float(SCENE_WIDTH) / 3.f);
		if (camPosition.x > (mapSize.x - SCENE_WIDTH)) camPosition.x = mapSize.x - SCENE_WIDTH;
		hud->setPos(glm::vec2(camPosition.x, camPosition.y + SCENE_HEIGHT - 24));
		gameMenu->setPosition(glm::vec2(camPosition.x + GAME_MENU_POS_X, camPosition.y + GAME_MENU_POS_Y));
		gameMenuButton->setPosition(glm::vec2(camPosition.x + GAME_MENU_BUTTON_POS_X, camPosition.y + GAME_MENU_BUTTON_POS_Y));
	}
	projection = glm::ortho(camPosition.x, float(SCENE_WIDTH) + camPosition.x, float(SCENE_HEIGHT) + camPosition.y, camPosition.y);
}

int Scene::getCameraLevel(const glm::vec2 &pos)
{
	return (pos.y - 32) / 224;
}



