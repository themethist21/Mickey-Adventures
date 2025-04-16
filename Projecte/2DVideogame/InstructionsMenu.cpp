#include "InstructionsMenu.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <iostream>
#include "Game.h"

void InstructionsMenu::init(TextRenderer& tr, ShaderProgram& shaderProgram) {
    // Guardamos referencias al TextRenderer y ShaderProgram
    textRenderer = &tr;
    texProgram = &shaderProgram;

    currentTime = 0.0f;
    // Cargar la textura de fondo
    if (!instructionsBackground.loadFromFile("images/inst.png", TEXTURE_PIXEL_FORMAT_RGBA)) {
        std::cerr << "Error al cargar la textura de fondo 'inst.png'" << std::endl;
        return;
    }

    // Inicializar el quad de fondo
    initSquare(glm::vec2(0.0f, SCENE_HEIGHT), glm::vec2(SCENE_WIDTH, SCENE_HEIGHT), glm::vec2(SCENE_WIDTH, 0.0f), glm::vec2(0.0f, 0.0f));

}

void InstructionsMenu::update(int deltaTime) {
    // Aquí puedes actualizar cualquier animación o temporizador que desees en el menú
    currentTime += deltaTime;
}

void InstructionsMenu::render() {
    glClearColor(0.f, 0.f, 0.f, 1.0f);

    texProgram->use();

    // Configurar la matriz modelview y otros uniforms
    glm::mat4 modelview = glm::mat4(1.0f);
    texProgram->setUniformMatrix4f("modelview", modelview);
    texProgram->setUniform2f("texCoordDispl", 0.0f, 0.0f);
    texProgram->setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
    texProgram->setUniformMatrix4f("projection", glm::ortho(0.0f, float(SCENE_WIDTH), float(SCENE_HEIGHT), 0.0f));

    // Dibujar el fondo del menú de instrucciones
    glEnable(GL_TEXTURE_2D);
    instructionsBackground.use();
    glBindVertexArray(quads[0].VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisable(GL_TEXTURE_2D);


    // Renderizar el texto de instrucciones
    textRenderer->renderText("INSTRUCTIONS", 0.3f * SCENE_WIDTH, 0.25f * SCENE_HEIGHT, 1.2f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    textRenderer->renderText("Movement: WASD keys ", 0.2f * SCENE_WIDTH, 0.4f * SCENE_HEIGHT, 0.8f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    textRenderer->renderText("Press S while falling to break objects", 0.2f * SCENE_WIDTH, 0.5f * SCENE_HEIGHT, 0.8f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    textRenderer->renderText("Press SPACE to grab objects", 0.2f * SCENE_WIDTH, 0.6f * SCENE_HEIGHT, 0.8f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    textRenderer->renderText("Objective: Reach the end of the level", 0.2f * SCENE_WIDTH, 0.7f * SCENE_HEIGHT, 0.8f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    textRenderer->renderText("Press i to go back", 0.3f * SCENE_WIDTH, 0.85f * SCENE_HEIGHT, 0.8f, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

    glBindVertexArray(0);
}

void InstructionsMenu::initSquare(const glm::vec2& esquina_ii, const glm::vec2& esquina_id, const glm::vec2& esquina_sd, const glm::vec2& esquina_si) {
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
