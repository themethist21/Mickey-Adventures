#pragma once

#include "ShaderProgram.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <vector>
#include <SFML/Audio.hpp>
#include "TextRenderer.h"
#include "Sprite.h"

#define SCENE_WIDTH 352
#define SCENE_HEIGHT 198

struct Quad {
    GLuint VAO;
    GLuint VBO;
};

class MenuScene
{
public:
    void init(TextRenderer& tr1, TextRenderer& tr2);
    void update(int deltaTime);
    void render();
    ShaderProgram& getShaderProgram();
	
private:
	void initShaders();
	void initSquare(const glm::vec2 &esquina_ii, const glm::vec2& esquina_id, const glm::vec2& esquina_sd, const glm::vec2& esquina_si);
    void renderQuads();
    glm::mat4 projection;
    ShaderProgram texProgram;
    float currentTime;
    Texture menuBackGround, gameMenuButtonsSpritesheet;
    Sprite *gameMenuButton;

    std::vector<Quad> quads;
    std::vector<std::vector<float>> verticesList; // Almacena los vértices de cada quad
	TextRenderer *textRenderer, *textRenderer2;
    bool upKeyPrevState = false;
    bool downKeyPrevState = false;

    sf::SoundBuffer soundBuffer;
    sf::Sound soundEffect;

    glm::vec4 blanco = glm::vec4(1.f, 1.f, 1.f, 1.f);
    glm::vec4 amarillo = glm::vec4(.6, .6, 0.f, 1.f);

};

