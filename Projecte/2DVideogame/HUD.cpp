#include "HUD.h"
#include <string>

#define HUD_WIDTH 352
#define HUD_HEIGHT 16

#define LIFE_WIDTH 42
#define LIFE_HEIGHT 12

#define SCENE_WIDTH 352
#define SCENE_HEIGHT 198

#define SCORE_POSITION_X 196
#define SCORE_POSITION_Y 12

#define TIME_POSITION_X 287
#define TIME_POSITION_Y 12

#define MAX_SCORE 999999

void HUD::init(ShaderProgram& shaderProgram, const glm::vec2 &pos)
{
	spritesheet.loadFromFile("images/PlayerHud.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(HUD_WIDTH, HUD_HEIGHT), glm::vec2(float(HUD_WIDTH) / 352.f, float(HUD_HEIGHT) / HUD_HEIGHT), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(1);

	sprite->setAnimationSpeed(0, 0);
	sprite->addKeyframe(0, glm::vec2(0.f, 0.f));

	sprite->changeAnimation(0);

	hudSpritesheet.loadFromFile("images/HudSprites.png", TEXTURE_PIXEL_FORMAT_RGBA);
	lifeSprite = Sprite::createSprite(glm::ivec2(LIFE_WIDTH, LIFE_HEIGHT), glm::vec2(float(LIFE_WIDTH) / 84.f, float(LIFE_HEIGHT) / 48.f), &hudSpritesheet, &shaderProgram);
	lifeSprite->setNumberAnimations(4);

	lifeSprite->setAnimationSpeed(0, 0);
	lifeSprite->addKeyframe(0, glm::vec2(0.f, 0.f));
	lifeSprite->setAnimationSpeed(1, 0);
	lifeSprite->addKeyframe(1, glm::vec2(0.f, 12.f / 48.f));
	lifeSprite->setAnimationSpeed(2, 0);
	lifeSprite->addKeyframe(2, glm::vec2(0.f, 24.f / 48.f));
	lifeSprite->setAnimationSpeed(3, 0);
	lifeSprite->addKeyframe(3, glm::vec2(0.f, 36.f / 48.f));

	lifeSprite->changeAnimation(3);

	lifeSprite->setPosition(glm::vec2(float(posHUD.x + 35), float(posHUD.y + 2)));

	triesSprite = Sprite::createSprite(glm::ivec2(LIFE_WIDTH, LIFE_HEIGHT), glm::vec2(float(LIFE_WIDTH) / 84.f, float(LIFE_HEIGHT) / 48.f), &hudSpritesheet, &shaderProgram);
	triesSprite->setNumberAnimations(4);

	triesSprite->setAnimationSpeed(0, 0);
	triesSprite->addKeyframe(0, glm::vec2(42.f / 84.f, 0.f));
	triesSprite->setAnimationSpeed(1, 0);
	triesSprite->addKeyframe(1, glm::vec2(42.f / 84.f, 12.f / 48.f));
	triesSprite->setAnimationSpeed(2, 0);
	triesSprite->addKeyframe(2, glm::vec2(42.f / 84.f, 24.f / 48.f));
	triesSprite->setAnimationSpeed(3, 0);
	triesSprite->addKeyframe(3, glm::vec2(42.f / 84.f, 36.f / 48.f));

	triesSprite->changeAnimation(3);

	triesSprite->setPosition(glm::vec2(float(posHUD.x + 116), float(posHUD.y + 2)));


	posHUD = pos;
	sprite->setPosition(glm::vec2(float(posHUD.x), float(posHUD.y)));
	lifeSprite->setPosition(glm::vec2(float(posHUD.x + 35), float(posHUD.y + 2)));
	triesSprite->setPosition(glm::vec2(float(posHUD.x + 116), float(posHUD.y + 2)));
}

void HUD::render()
{
	sprite->render();
	lifeSprite->render();
	triesSprite->render();
	textRenderer->renderText(std::to_string(score), SCORE_POSITION_X, SCORE_POSITION_Y, 1.f, glm::vec4(1.f, 1.f, 1.f, 1.f));
	textRenderer->renderText(std::to_string(time), TIME_POSITION_X, TIME_POSITION_Y, 1.f, glm::vec4(1.f, 1.f, 1.f, 1.f));
}

void HUD::setTextRenderer(TextRenderer & tr)
{
	textRenderer = &tr;
}

void HUD::setPos(const glm::vec2 & pos)
{
	posHUD = pos;
	sprite->setPosition(glm::vec2(float(posHUD.x), float(posHUD.y)));
	lifeSprite->setPosition(glm::vec2(float(posHUD.x + 35), float(posHUD.y + 2)));
	triesSprite->setPosition(glm::vec2(float(posHUD.x + 116), float(posHUD.y + 2)));
}

void HUD::setLife(int l)
{
	lifeSprite->changeAnimation(l);
}

void HUD::setTries(int t)
{
	triesSprite->changeAnimation(t);
}

void HUD::setTime(int t)
{
	time = t;
}

void HUD::setScore(int s)
{
	if (score < MAX_SCORE) score = s;
	else score = MAX_SCORE;
}
