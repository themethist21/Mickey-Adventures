#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Game.h"
#include <iostream>

#define FONT_SIZE 14
#define FONT_SIZE2 16

#define LEVEL1_INIT_PLAYER_X_TILES 8
#define LEVEL1_INIT_PLAYER_Y_TILES 10
#define LEVEL2_INIT_PLAYER_X_TILES 8
#define LEVEL2_INIT_PLAYER_Y_TILES 82

float Game::SOUND_EFFECTS_VOLUME = 60.0f;
float Game::SOUND_MUSIC_VOLUME = 20.0f;

void Game::init()
{
    bPlay = true;
    glClearColor(0.4f, 0.7f, 7.0f, 1.0f);

    currentState = GameState::MENU;
    lastState = GameState::MENU;

    textRenderer = new TextRenderer("fonts/8bitBold.ttf", SCENE_WIDTH, SCENE_HEIGHT, FONT_SIZE);
    textRenderer2 = new TextRenderer("fonts/8bitBold.ttf", SCENE_WIDTH, SCENE_HEIGHT, FONT_SIZE2);
	//puedo agregar más fuentes si es necesario

    /* glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

    menuScene.init(*textRenderer, *textRenderer2);
    scene.init(*textRenderer, "levels/levelPractice.txt", glm::ivec2(LEVEL1_INIT_PLAYER_X_TILES, LEVEL1_INIT_PLAYER_Y_TILES)); //LEVEL1
	scene2.init(*textRenderer, "levels/levelFull.txt", glm::ivec2(LEVEL2_INIT_PLAYER_X_TILES, LEVEL2_INIT_PLAYER_Y_TILES)); //LEVEL2
	instructionsScene.init(*textRenderer, menuScene.getShaderProgram());

    backgroundMusic.setVolume(SOUND_MUSIC_VOLUME);
    // Cargar y reproducir la música del menú
    if (!backgroundMusic.openFromFile("sounds/derelict.ogg")) {  // Cambia el nombre del archivo si es necesario
        std::cerr << "Error al cargar la música del menú" << std::endl;
    }
    backgroundMusic.setLoop(true);  // Configura la música para que se repita en bucle
    backgroundMusic.play();         // Inicia la reproducción de la música

    // Cargar el sonido
    if (!victorybuffer.loadFromFile("sounds/Victory.wav")) {
        std::cerr << "Error al cargar Victory.wav" << std::endl;
    }
    // Crear una instancia de sf::Sound y asignarle el buffer
    victoryeffect.setBuffer(victorybuffer);
    victoryeffect.setVolume(Game::SOUND_EFFECTS_VOLUME + 20);
}

bool Game::update(int deltaTime)
{
    // Detecta si el estado cambió
    if (currentState != lastState) {
        // Detener la música actual si cambia de escena
        backgroundMusic.stop();

        // Cargar la música de acuerdo con la escena actual
        if (currentState == GameState::MENU) {
            if (!backgroundMusic.openFromFile("sounds/derelict.ogg")) {
                std::cerr << "Error al cargar la música del menú" << std::endl;
            }
        }
        else if (currentState == GameState::LEVEL1) {
            if (!backgroundMusic.openFromFile("sounds/lvl2music.ogg")) {
                std::cerr << "Error al cargar la música de Level 1" << std::endl;
            }
        }
        else if (currentState == GameState::LEVEL2) {
            if (!backgroundMusic.openFromFile("sounds/lvl1music.ogg")) {
                std::cerr << "Error al cargar la música de Level 2" << std::endl;
            }
        }
        else if (currentState == GameState::INSTRUCTIONS) {
            if (!backgroundMusic.openFromFile("sounds/instrmusic.wav")) {
                std::cerr << "Error al cargar la música de instrucciones" << std::endl;
            }
        }

        // Reproducir la nueva música
        backgroundMusic.setLoop(true); // Para que la música se repita en bucle
        backgroundMusic.play();

        // Actualizar el estado anterior
        lastState = currentState;
    }

    // Actualizar la escena actual
    if (currentState == GameState::MENU)
        menuScene.update(deltaTime);

    else if (currentState == GameState::LEVEL1)
        scene.update(deltaTime);
    else if (currentState == GameState::LEVEL2)
        scene2.update(deltaTime);
    else if (currentState == GameState::INSTRUCTIONS)
        instructionsScene.update(deltaTime);

    return bPlay;
}


void Game::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (currentState == GameState::MENU)
        menuScene.render();

    else if (currentState == GameState::LEVEL1)
        scene.render();

    else if (currentState == GameState::LEVEL2)
        scene2.render();

    else if (currentState == GameState::INSTRUCTIONS)
		instructionsScene.render();

}


void Game::keyPressed(int key)
{
    keys[key] = true;


    if (currentState == GameState::MENU)
    {
        // Maneja la navegación en el menú
        /*if (key == GLFW_KEY_ENTER)
        {
            beforeState = currentState;  // Guarda el estado actual antes de cambiar
            currentState = GameState::LEVEL1;
        }*/
        // Agrega más controles para el menú si es necesario
        if (key == GLFW_KEY_I)
        {
            beforeState = currentState;  // Guarda el estado actual antes de cambiar
            currentState = GameState::INSTRUCTIONS;
        }

		// Verifica si se presionó ESC para salir del juego
		if (key == GLFW_KEY_ESCAPE)
			bPlay = false;
    }
    else if (currentState == GameState::LEVEL2)
    {
        if (key == GLFW_KEY_I)
        {
            beforeState = currentState;  // Guarda el estado actual antes de cambiar
            currentState = GameState::INSTRUCTIONS;
        }

		// Verifica si se presionó ESC para pausar el juego
		if (key == GLFW_KEY_ESCAPE)
			scene2.pause();
	}
    else if (currentState == GameState::LEVEL1) 
    {
        if (key == GLFW_KEY_I)
        {
            beforeState = currentState;  // Guarda el estado actual antes de cambiar
            currentState = GameState::INSTRUCTIONS;
        }

		// Verifica si se presionó ESC para pausar el juego
		if (key == GLFW_KEY_ESCAPE)
			scene.pause();
    }

    else if (currentState == GameState::INSTRUCTIONS)
    {
        if (key == GLFW_KEY_I)
        {
            // Regresa al estado anterior guardado en `beforeState`
            currentState = beforeState;
        }
        // Maneja otras entradas durante el menú de instrucciones

		// Verifica si se presionó ESC para salir del juego
		if (key == GLFW_KEY_ESCAPE)
			bPlay = false;
    }
    else if (currentState == GameState::GAMEOVER)
    {
        // Maneja la entrada durante la pantalla de Game Over

		// Verifica si se presionó ESC para salir del juego
		if (key == GLFW_KEY_ESCAPE)
			bPlay = false;
    }
}


void Game::keyReleased(int key)
{
	keys[key] = false;
}

void Game::mouseMove(int x, int y)
{
}

void Game::mousePress(int button)
{
}

void Game::mouseRelease(int button)
{
}

bool Game::getKey(int key) const
{
	return keys[key];
}

void Game::setMenu()
{
	currentState = GameState::MENU;
	keys[GLFW_KEY_ENTER] = false;
}

void Game::beatCurrentStage()
{
    if (currentState == GameState::LEVEL1) {
        backgroundMusic.pause();
        victoryeffect.play();
        while (victoryeffect.getStatus() == sf::Sound::Playing) {
            sf::sleep(sf::milliseconds(100)); // Pausar brevemente para reducir la carga de la CPU
        }
        scene.reset();
        backgroundMusic.play();
    }
		

    else if (currentState == GameState::LEVEL2) {
		backgroundMusic.pause();
        victoryeffect.play();
        while (victoryeffect.getStatus() == sf::Sound::Playing) {
            sf::sleep(sf::milliseconds(100)); // Pausar brevemente para reducir la carga de la CPU
        }
        scene2.reset();
        backgroundMusic.play();
    }
		
    currentState = GameState::MENU;
}

void Game::setState(GameState state)
{
	currentState = state;
}



