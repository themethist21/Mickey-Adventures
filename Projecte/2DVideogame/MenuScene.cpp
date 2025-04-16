#include "MenuScene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <iostream>
#include "Game.h"

#define GAME_MENU_BUTTON_WIDTH 8
#define GAME_MENU_BUTTON_HEIGHT 43
#define GAME_MENU_BUTTON_POS_X 230
#define GAME_MENU_BUTTON_POS_Y 140
#define BUTTON_BUFFER_TIME 0.2f


void MenuScene::init(TextRenderer &tr1, TextRenderer &tr2)
{
    // Inicializar la matriz de proyección
    projection = glm::ortho(0.0f, float(SCENE_WIDTH), float(SCENE_HEIGHT), 0.0f);
	menuBackGround.loadFromFile("images/menu3.png", TEXTURE_PIXEL_FORMAT_RGBA);
	currentTime = 0.0f;

	initShaders();
    glClearColor(0.4f, 0.7f, 7.0f, 1.0f);
	initSquare(glm::vec2(0.0f, SCENE_HEIGHT), glm::vec2(SCENE_WIDTH, SCENE_HEIGHT), glm::vec2(SCENE_WIDTH, 0.f), glm::vec2(0.f, 0.f)); //Fondo del menú

	textRenderer = &tr1;
	textRenderer2 = &tr2;

	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);

    // Inicializa shaders, texturas y otros recursos necesarios
    gameMenuButtonsSpritesheet.loadFromFile("images/MenuHudButtons.png", TEXTURE_PIXEL_FORMAT_RGBA);
    gameMenuButton = Sprite::createSprite(glm::ivec2(GAME_MENU_BUTTON_WIDTH, GAME_MENU_BUTTON_HEIGHT), glm::vec2(0.5f, 1.f), &gameMenuButtonsSpritesheet, &texProgram);
    gameMenuButton->setNumberAnimations(2);
    gameMenuButton->setAnimationSpeed(0, 0);
    gameMenuButton->addKeyframe(0, glm::vec2(0.f, 0.f));
    gameMenuButton->setAnimationSpeed(1, 0);
    gameMenuButton->addKeyframe(1, glm::vec2(0.5f, 0.f));
    gameMenuButton->changeAnimation(0);
    gameMenuButton->setPosition(glm::vec2(GAME_MENU_BUTTON_POS_X, GAME_MENU_BUTTON_POS_Y));

    // Cargar el sonido
    if (!soundBuffer.loadFromFile("sounds/Select.wav")) {
        std::cerr << "Error al cargar Select.wav" << std::endl;
    }

    // Crear una instancia de sf::Sound y asignarle el buffer
    soundEffect.setBuffer(soundBuffer);
    soundEffect.setVolume(Game::SOUND_EFFECTS_VOLUME);
}

void MenuScene::update(int deltaTime)
{
    currentTime += deltaTime;
    // Actualiza la lógica del menú, como animaciones o selección de opciones

    static int menuOption = 0;

    // Obtener el estado actual de las teclas
    bool upKeyCurrentState = Game::instance().getKey(GLFW_KEY_UP);
    bool downKeyCurrentState = Game::instance().getKey(GLFW_KEY_DOWN);

    if ((upKeyCurrentState && !upKeyPrevState) || downKeyCurrentState && !downKeyPrevState) {
        menuOption = (menuOption == 0) ? 1 : 0;
        gameMenuButton->changeAnimation(menuOption);
        soundEffect.play();
    }

	if (Game::instance().getKey(GLFW_KEY_ENTER)) {
		if (menuOption == 0)
		    Game::instance().setState(GameState::LEVEL1);
        else if (menuOption == 1)
			Game::instance().setState(GameState::LEVEL2);
	}
    // Actualizar los estados previos de las teclas
    upKeyPrevState = upKeyCurrentState;
    downKeyPrevState = downKeyCurrentState;
}

ShaderProgram& MenuScene::getShaderProgram()
{
	return texProgram;
}


void MenuScene::initSquare(const glm::vec2& esquina_ii, const glm::vec2& esquina_id, const glm::vec2& esquina_sd, const glm::vec2& esquina_si) {
    // Definir los vértices del cuadrado
    std::vector<float> vertices = {
        esquina_ii.x, esquina_ii.y ,   0.0f, 1.0f,   // Esquina inferior izquierda
        esquina_id.x ,esquina_id.y,    1.0f, 1.0f,   // Esquina inferior derecha
        esquina_sd.x, esquina_sd.y,    1.0f, 0.0f,   // Esquina superior derecha
        esquina_si.x, esquina_si.y,    0.0f, 0.0f    // Esquina superior izquierda
    };
    verticesList.push_back(vertices); // Almacenar vértices del quad

    // Crear y configurar VAO y VBO para el cuadrado
    Quad quad;
    glGenVertexArrays(1, &quad.VAO);
    glGenBuffers(1, &quad.VBO);

    glBindVertexArray(quad.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, quad.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Almacena el nuevo quad
    quads.push_back(quad);
}

void MenuScene::renderQuads() {
    for (const auto& quad : quads) {
        glBindVertexArray(quad.VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    glBindVertexArray(0);
}


void MenuScene::render()
{

    texProgram.use();


    glm::mat4 modelview = glm::mat4(1.0f);
    texProgram.setUniformMatrix4f("modelview", modelview);

    texProgram.setUniformMatrix4f("projection", projection);

    texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

    texProgram.setUniform4f("color", 1.0f, 1.f, 1.0f, 1.f);

    

    glEnable(GL_TEXTURE_2D);
    menuBackGround.use();
    glBindVertexArray(quads[0].VAO);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisable(GL_TEXTURE_2D);

    glBindVertexArray(0);

    textRenderer2->renderText("Practice Level", GAME_MENU_BUTTON_POS_X - 133, SCENE_HEIGHT - GAME_MENU_BUTTON_POS_Y - GAME_MENU_BUTTON_HEIGHT * 0.30, 1.f, blanco);
    textRenderer2->renderText("Full Level", GAME_MENU_BUTTON_POS_X - 93, SCENE_HEIGHT - GAME_MENU_BUTTON_POS_Y - GAME_MENU_BUTTON_HEIGHT * 0.9 , 1.f, blanco);

    int tot = 1200;
    int aux = int(currentTime) % tot;
    if (aux < (tot / 2)) {
        gameMenuButton->render();
    }

    textRenderer->renderText("Press i for", 0.01 * SCENE_WIDTH, 0.55 * SCENE_HEIGHT, 1.f, amarillo);
    textRenderer->renderText("instructions", 0.01 * SCENE_WIDTH, 0.50 * SCENE_HEIGHT, 1.f, amarillo);

    
}



void MenuScene::initShaders()
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

}
