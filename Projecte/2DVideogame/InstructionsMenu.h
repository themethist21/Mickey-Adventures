#pragma once
#include <glm/glm.hpp>
#include "TextRenderer.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include <vector>
#include <GL/glew.h>

class InstructionsMenu {
public:
    // Inicializa el men� de instrucciones con el TextRenderer y el ShaderProgram
    void init(TextRenderer& tr, ShaderProgram& shaderProgram);

    // Actualiza cualquier animaci�n o l�gica de la pantalla de instrucciones
    void update(int deltaTime);

    // Renderiza el men� de instrucciones en la pantalla
    void render();

private:
    // Crea un cuadrado para el fondo del men� de instrucciones
    void initSquare(const glm::vec2& esquina_ii, const glm::vec2& esquina_id, const glm::vec2& esquina_sd, const glm::vec2& esquina_si);

    Texture instructionsBackground;    // Textura del fondo del men� de instrucciones
    TextRenderer* textRenderer;        // Puntero al TextRenderer para renderizar texto
    ShaderProgram* texProgram;         // Puntero al ShaderProgram para manejar shaders

    // Estructura para almacenar informaci�n del quad de fondo
    struct Quad {
        GLuint VAO;                    // Vertex Array Object
        GLuint VBO;                    // Vertex Buffer Object
    };
    std::vector<Quad> quads;
    std::vector<std::vector<float>> verticesList; // Almacena los v�rtices de cada quad

	float currentTime;                 // Tiempo actual para animaciones o temporizadores
};

