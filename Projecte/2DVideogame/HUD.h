#ifndef _HUD_INCLUDE
#define _HUD_INCLUDE

#include "Sprite.h"
#include <vector>
#include "TextRenderer.h"

class HUD
{
public:
	void init(ShaderProgram& shaderProgram, const glm::vec2 &pos);
	void render();

	void setTextRenderer(TextRenderer& tr);
	void setPos(const glm::vec2 &pos);
	void setLife(int l);
	void setTries(int t);
	void setTime(int t);
	void setScore(int s);

private:
	glm::ivec2 posHUD;
	Texture spritesheet, hudSpritesheet;
	Sprite* sprite;
	Sprite* lifeSprite;
	Sprite* triesSprite;
	TextRenderer *textRenderer;

	int time, score;
};

#endif //_HUD_iNCLUDE