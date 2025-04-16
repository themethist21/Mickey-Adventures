#pragma once
#include <glm/glm.hpp>
#include "TextRenderer.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include <vector>
#include <GL/glew.h>

class InstructionsMenu {
public:
    // Inicializa el menú de instrucciones con el TextRenderer y el ShaderProgram
    void init(TextRenderer& tr, ShaderProgram& shaderProgram);

    // Actualiza cualquier animación o lógica de la pantalla de instrucciones
    void update(int deltaTime);

    // Renderiza el menú de instrucciones en la pantalla
    void render();

private:
    // Crea un cuadrado para el fondo del menú de instrucciones
    void initSquare(const glm::vec2& esquina_ii, const glm::vec2& esquina_id, const glm::vec2& esquina_sd, const glm::vec2& esquina_si);

    Texture instructionsBackground;    // Textura del fondo del menú de instrucciones
    TextRenderer* textRenderer;        // Puntero al TextRenderer para renderizar texto
    ShaderProgram* texProgram;         // Puntero al ShaderProgram para manejar shaders

    // Estructura para almacenar información del quad de fondo
    struct Quad {
        GLuint VAO;                    // Vertex Array Object
        GLuint VBO;                    // Vertex Buffer Object
    };
    std::vector<Quad> quads;
    std::vector<std::vector<float>> verticesList; // Almacena los vértices de cada quad

	float currentTime;                 // Tiempo actual para animaciones o temporizadores
};

